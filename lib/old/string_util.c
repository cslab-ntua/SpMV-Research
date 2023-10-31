#include "string_util.h"

#include "debug.h"
#include "parallel_util.h"


//==========================================================================================================================================
//= String In-Place Delimiters
//==========================================================================================================================================


/* Delimiter functions find the delimiters and replace them with null bytes ('\0').
 * Null bytes ('\0') will always count as delimiters.
 * The array boundaries (outside the bounding elements) will always count as delimiters.
 */


void
str_delimiter_eof(__attribute__((unused)) char * str, __attribute__((unused)) long N)
{
	return;
}


void
str_delimiter_line(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str[i] == '\n' || str[i] == '\0')
		{
			str[i] = 0;
		}
}


void
str_delimiter_word(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str_char_is_ws(str[i]))
		{
			str[i] = 0;
		}
}


void
str_delimiter_csv(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str_char_is_ws(str[i]) || str[i] == ',')
		{
			str[i] = 0;
		}
}


//==========================================================================================================================================
//= String Tokenizer
//==========================================================================================================================================


/* str:
 *     A sequence of characters (char array).
 * N:
 *     The size of the string (including the terminating NULL character).
 * string_delimiter:
 *     A function that finds and sets the delimiters in a string given to '\0'.
 * keep_empty:
 *     Keep tokens of zero length.
 * tokens_out:
 *     Array with the addresses of the tokens in 'string'.
 * tokens_lengths_out:
 *     Length of each token, i.e. excluding the delimiters and NULL terminating characters.
 */
void
str_tokenize(char * str, long N, void string_delimiter(char *, long), int keep_empty,
		char *** tokens_out, long ** tokens_lengths_out, long * total_tokens_out)
{
	int num_threads = safe_omp_get_num_threads_external();
	char ** tokens;
	long * tokens_lengths;

	long t_i_first_delimiter[num_threads];
	long t_base[num_threads];

	if (N <= 0)
	{
		*tokens_out = NULL;
		*tokens_lengths_out = NULL;
		*total_tokens_out = 0;
		return;
	}

	_Pragma("omp parallel")
	{
		int tnum = omp_get_thread_num();
		long i, j, has_delimiters, num_tokens, pos, len;
		long i_s, i_e, i_e_initial;
		__attribute__((unused)) long i_s_initial;

		loop_partitioner_balance_iterations(num_threads, tnum, 0, N, &i_s, &i_e);
		i_s_initial = i_s;
		i_e_initial = i_e;

		string_delimiter(str + i_s, i_e - i_s);              // Delimit tokens, replacing delimiter-chars with '\0'.

		// Lose the first token.
		for (i=i_s;((i < i_e) && (str[i] != 0));i++)         // Find first delimiter.
			;
		if (tnum == 0)
			has_delimiters = 1;
		else if (i < i_e)
		{
			has_delimiters = 1;
			i_s = i + 1;                                 // Without the last '\0'.
			__atomic_store_n(&t_i_first_delimiter[tnum], i_s, __ATOMIC_RELAXED);
		}
		else
		{
			has_delimiters = 0;
			i_s = i_e;
			__atomic_store_n(&t_i_first_delimiter[tnum], -1, __ATOMIC_RELAXED);
		}

		#pragma omp barrier

		// Get the first token of the next thread with > 0 num_tokens.
		if (has_delimiters)
		{
			for (j=tnum+1;j<num_threads;j++)                                              // Find next thread with > 0 delimiters.
			{
				i = __atomic_load_n(&t_i_first_delimiter[j], __ATOMIC_RELAXED);
				if (i >= 0)
					break;
			}
			i_e = (j < num_threads) ? i : N;
		}

		// Count the number of tokens.
		// 'j' here is always at the end of the previous token (at a '\0' byte, or at -1).
		num_tokens = 0;
		if (has_delimiters)
		{
			j = i_s - 1;
			for (i=i_s;i<i_e_initial;i++) // There is no delimiter in the range (i_e_initial ... i_e-1).
			{
				if (str[i] != 0)
					continue;
				len = i - j - 1;
				if (keep_empty || (len > 0))
					num_tokens++;
				j = i;
			}

			/* 'j' is in the last delimiter before i_e_initial, or -1.
			 * Even if the only delimiter was given to a previous thread, 'j' is at i_s-1, which is that delimiter.
			 * No delimiter in the range (i_e_initial ... i_e-1).
			 * There is a delimiter at i_e-1 except (maybe) if i_e == N.
			 * If no delimiter at i_e-1, we still can have a token [j+1 ... i_e == N).
			 * Never keep the last empty token (outside the boundary 'N').
			 */
			len = i_e-1 - j - 1;
			if (str[i_e-1] != '\0')
				len++;
			if ((keep_empty && i_e < N) || (len > 0))
				num_tokens++;
		}

		// printf("%2d: i_s_initial=%2ld i_s=%2ld i_e_initial=%2ld i_e=%2ld len=%2ld num_tok=%2ld\n", tnum, i_s_initial, i_s, i_e_initial, i_e, i_e-i_s, num_tokens);

		__atomic_store_n(&t_base[tnum], num_tokens, __ATOMIC_RELAXED);

		// Total tokens calculation and memory allocation.
		#pragma omp barrier
		#pragma omp single
		{
			long tmp;
			num_tokens = 0;
			for (i=0;i<num_threads;i++)
			{
				tmp = __atomic_load_n(&t_base[i], __ATOMIC_RELAXED);
				__atomic_store_n(&t_base[i], num_tokens, __ATOMIC_RELAXED);
				num_tokens += tmp;
			}
			tokens = (typeof(tokens)) malloc(num_tokens * sizeof(*tokens));
			tokens_lengths = (typeof(tokens_lengths)) malloc(num_tokens * sizeof(*tokens_lengths));

			*tokens_out = tokens;
			*tokens_lengths_out = tokens_lengths;
			*total_tokens_out = num_tokens;
		}
		#pragma omp barrier

		if (has_delimiters)
		{
			// Calculate the addresses and lengths of the tokens.
			pos = __atomic_load_n(&t_base[tnum], __ATOMIC_RELAXED);
			j = i_s - 1;
			for (i=i_s;i<i_e;i++)
			{
				if (str[i] != 0)
					continue;
				len = i - j - 1;
				if (keep_empty || (len > 0))
				{
					tokens[pos] = &(str[j+1]);
					tokens_lengths[pos] = len;
					pos++;
				}
				j = i;
			}

			// The last token might not be null terminated.
			// If empty, always ignored.
			len = i_e-1 - j - 1;
			if (str[i_e-1] != '\0')
				len++;
			if (len > 0)
			{
				tokens[pos] = &(str[j+1]);
				tokens_lengths[pos] = len;
			}
		}
	}
}

