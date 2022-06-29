#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "debug.h"
#include "macros/macrolib.h"
#include "omp_functions.h"


/*
 * The strings are ALWAYS handled like arrays of characters, nothing is assumed missing.
 *
 * For the 'size' arguments we need to take into account all the relevant characters and only them:
 *     - For strings include the terminating '\0' (i.e. pass strlen() + 1).
 *     - For character sequences (e.g. for a substring to search for) pass only their number (i.e. exclude '\0' if not relevant).
 */


//==========================================================================================================================================
//= Utilities
//==========================================================================================================================================


#define str_assert_string_termination(str, N)          \
do {                                                   \
	if (N <= 0)                                    \
		error("empty string given");           \
	else if (str[N-1] != 0)                        \
		error("unterminated string given");    \
} while (0)


//==========================================================================================================================================
//= Characters Groups
//==========================================================================================================================================


static inline
int
str_char_is_ws(const char c)
{
	// #pragma GCC diagnostic push
	// #pragma GCC diagnostic ignored "-Woverride-init"
	// static const int t[UCHAR_MAX] = {
		// [0 ... UCHAR_MAX-1] = 0,
		// [' ']=1, ['\t']=1, ['\n']=1, ['\r']=1, ['\0']=1,
	// };
	// #pragma GCC diagnostic pop
	// return t[(unsigned char) c];
	switch (c) {
		case ' ': case '\t': case '\n': case '\r': case '\0':
			return 1;
		default:
			return 0;
	}
}


//==========================================================================================================================================
//= Find Character
//==========================================================================================================================================


static inline
long
str_find_char_from_start(const char * str, long N, char c)
{
	long i;
	for (i=0;i<N;i++)
		if (str[i] == c)
			break;
	return i;
}


static inline
long
str_find_char_from_end(const char * str, long N, char c)
{
	long i;
	for (i=N-1;i>=0;i--)
		if (str[i] == c)
			break;
	return i;
}


static inline
long
str_find_ws(const char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str_char_is_ws(str[i]))
			break;
	return i;
}


static inline
long
str_find_non_ws(const char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (!str_char_is_ws(str[i]))
			break;
	return i;
}


static inline
long
str_find_eol(const char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str[i] == '\n')
			break;
	return i;
}


static inline
long
str_next_word(const char * str, long N)
{
	long i = 0;
	i = str_find_ws(str, N);               // Find end of current word.
	i += str_find_non_ws(str+i, N-i);
	return i;
}


//==========================================================================================================================================
//= Substring
//==========================================================================================================================================


struct str_Search_Term {
	long n;                // The string length.
	const char * term;     // The string.
	long * pmt;            // Partial Match Table, backtracking to the LAST CHARACTER of the longest common prefix when failing to match character.
};


static inline
void
str_search_term_init(struct str_Search_Term * search_term, const char * str, long N)
{
	long i = 0, j;
	long * pmt;
	char * term;
	term = (typeof(term)) malloc(N * sizeof(*term));
	strncpy(term, str, N);
	pmt = (typeof(pmt)) malloc((N+1) * sizeof(*pmt));     // +1, for backtracking in success, when looking for multiple occurrences.
	pmt[0] = -1;
	for (i=1,j=0;i<N+1;i++,j++)
	{
		if (term[i] == term[j])        // Same as next character after the current common prefix, so the common prefix grows.
			pmt[i] = pmt[j];     // If we fail here we fail twice and backtrack again.
		else
		{
			pmt[i] = j;
			while (j >= 0)
			{
				j = pmt[j];                     // Find the new common prefix, using the known backtracking for all the shorter prefixes.
				if (term[i] == term[j])
					break;
			}
		}
	}
	search_term->n = N;
	search_term->term = (typeof(search_term->term)) term;
	search_term->pmt = pmt;
}


static inline
struct str_Search_Term *
str_search_term_new(const char * str, long N)
{
	struct str_Search_Term * search_term;
	search_term = (typeof(search_term)) malloc(sizeof(*search_term));
	str_search_term_init(search_term, str, N);
	return search_term;
}


static inline
void
str_search_term_clean(struct str_Search_Term * search_term)
{
	free(search_term->pmt);
	search_term->pmt = NULL;
}


static inline
void
str_search_term_destroy(struct str_Search_Term ** st_ptr)
{
	str_search_term_clean(*st_ptr);
	free(*st_ptr);
	*st_ptr = NULL;
}


static inline
long
str_find_substr_simple(const char * str, long N, const char * substr, long M)
{
	char c_first;
	long i = 0, j;
	printf("lalala\n");
	if (M <= 0 || M > N)
		return N;
	c_first = substr[0];
	while (1)
	{
		i += str_find_char_from_start(str+i, N-i, c_first);
		if (i + M > N)
			return N;
		for (j=0;j<M;j++)
			if (str[i+j] != substr[j])
				break;
		if (j == M)
			return i;
		i++;
	}
}


// Knuth–Morris–Pratt
static inline
long
str_find_substr_kmp(const char * str, long N, struct str_Search_Term * search_term)
{
	long M = search_term->n;
	const char * term = search_term->term;
	long * pmt = search_term->pmt;   // Partial Match Table, backtracking to the LAST CHARACTER of the longest common prefix when failing to match character.
	long i = 0, j;
	if (M <= 0 || M > N)
		return N;
	i = 0;
	j = 0;
	while (i < N)
	{
		// printf("str='%s', str[i]='%c', term='%s', term[j]='%c', i=%ld, j=%ld\n", str, str[i], term, term[j], i, j);
		if (str[i] == term[j])
		{
			i++;
			j++;
			if (j >= M)
				break;
		}
		else                      // Backtrack until same character.
		{
			j = pmt[j];
			if (j < 0)
			{
				i++;
				j = 0;
			}
		}
	}
	return (i < N) ? i - M : N;
}


#define str_find_substr(str, N, search_term, ...)                                                                                            \
({                                                                                                                                           \
	_Generic((search_term),                                                                                                              \
		struct str_Search_Term *:  str_find_substr_kmp(str, N, (struct str_Search_Term *) search_term),                              \
		char *: str_find_substr_simple(str, N, (char *) search_term, DEFAULT_ARG_1(strlen((char *) search_term), ##__VA_ARGS__)),    \
		default: NULL                                                                                                                \
	);                                                                                                                                   \
})


//==========================================================================================================================================
//= Path Strings
//==========================================================================================================================================


static inline
char *
str_path_strip_tail(char * str, long N)
{
	long i;
	str_assert_string_termination(str, N);
	i = str_find_char_from_end(str, N, '/');
	if (i < 0)
		return str + N - 1;
	str[i] = 0;
	return str + i + 1;
}


static inline
char *
str_path_strip_ext(char * str, long N)
{
	long i;
	str_assert_string_termination(str, N);
	i = str_find_char_from_end(str, N, '.');
	if (i <= 0 || str[i-1] == '/')      // If tail starts with '.' it is not an extension, it is a hidden file.
		return str + N - 1;
	str[i] = 0;
	return str + i + 1;
}


//==========================================================================================================================================
//= Function Declarations
//==========================================================================================================================================


void str_delimiter_word(char * str, long N);
void str_delimiter_line(char * str, long N);
void str_delimiter_eof(char * str, long N);

void str_tokenize(char * str, long N, void string_delimiter(char *, long), int keep_empty, char *** tokens_out, long ** tokens_lengths_out, long * total_tokens_out);


#endif /* STRING_UTIL_H */

