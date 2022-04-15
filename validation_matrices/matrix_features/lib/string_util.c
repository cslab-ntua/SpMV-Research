#include "string_util.h"

#include "macros/cpp_defines.h"
#include "parallel_util.h"


//==========================================================================================================================================
//= String In-Place Delimiters
//==========================================================================================================================================


long
str_delimiter_word(char * str, long N)
{
	long num = 0;
	long i;
	for (i=0;i<N;i++)
		if (str_char_is_ws(str[i]))
		{
			str[i] = 0;
			num++;
		}
	return num;
}


long
str_delimiter_line(char * str, long N)
{
	long num = 0;
	long i;
	for (i=0;i<N;i++)
		if (str[i] == '\n' || str[i] == '\0')
		{
			str[i] = 0;
			num++;
		}
	return num;
}


long
str_delimiter_eof(__attribute__((unused)) char * str, long N)
{
	return N > 0;
}


//==========================================================================================================================================
//= String Tokenizer
//==========================================================================================================================================


/*
 * str:
 *     A NULL terminated string.
 * N:
 *     The size of the string (including the terminating NULL character).
 * string_delimiter:
 *     A function that finds and sets the delimiters in a string given to '\0', and returns the number of tokens found.
 * keep_empty:
 *     Keep tokens of zero length.
 * tokens_out:
 *     Array with the addresses of the tokens in 'string'.
 * tokens_lengths_out:
 *     Length of each token, i.e. excluding the delimiters and NULL terminating characters.
 */
void
str_tokenize(char * str, long N, long string_delimiter(char *, long), int keep_empty,
		char *** tokens_out, long ** tokens_lengths_out, long * total_tokens_out)
{
	int num_threads = omp_get_max_threads();
	char ** tokens;
	long * tokens_lengths;

	long t_base[num_threads];

	// Delimit tokens, replacing delimiter-chars with NULL.
	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long i, j, num_tokens, pos, len;
		long i_s, i_e;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);

		num_tokens = string_delimiter(str + i_s, i_e - i_s);

		// If not keeping empty tokens, count them again.
		// 'j' here is always at the end of the previous token.
		if (!keep_empty)
		{
			num_tokens = 0;
			for (j=i_s-1;((j >= 0) && (str[j] != 0));j--)   // Find the start of the first token.
				;
			for (i=i_s;i<i_e;i++)
			{
				if (str[i] != 0)
					continue;
				len = i - j - 1;
				if (len != 0)
					num_tokens++;
				j = i;
			}
		}

		t_base[tnum] = num_tokens;

		// Total tokens calculation and memory allocation.
		#pragma omp barrier
		#pragma omp single
		{
			long tmp;
			num_tokens = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = t_base[i];
				t_base[i] = num_tokens;
				num_tokens += tmp;
			}
			alloc(tokens, num_tokens);
			alloc(tokens_lengths, num_tokens);

			*tokens_out = tokens;
			*tokens_lengths_out = tokens_lengths;
			*total_tokens_out = num_tokens;
		}
		#pragma omp barrier

		// Calculate the addresses and lengths of the tokens.
		pos = t_base[tnum];
		for (j=i_s-1;((j >= 0) && (str[j] != 0));j--)   // Find the start of the first token.
			;
		for (i=i_s;i<i_e;i++)
		{
			if (str[i] != 0)
				continue;
			len = i - j - 1;
			if (keep_empty || (len != 0))
			{
				tokens[pos] = &(str[j+1]);
				tokens_lengths[pos] = len;
				pos++;
			}
			j = i;
		}
	}
}

