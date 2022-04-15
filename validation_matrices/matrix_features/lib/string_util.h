#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "debug.h"


/*
 * The strings are ALWAYS handled like char arrays.
 * Therefore string sizes given should always include the terminating '\0' (i.e. pass strlen() + 1).
 * This way, if proper string termination is needed the functions are able to test it.
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
str_char_is_ws(char c)
{
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Woverride-init"
	static const int t[CHAR_MAX] = {
		[0 ... CHAR_MAX-1] = 0,
		[' ']=1, ['\t']=1, ['\n']=1, ['\r']=1, ['\0']=1,
	};
	#pragma GCC diagnostic pop
	return t[(int) c];
	// return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0';
}


//==========================================================================================================================================
//= Find
//==========================================================================================================================================


static inline
long
str_find_char_first(char * str, long N, char c)
{
	long i;
	for (i=0;i<N;i++)
		if (str[i] == c)
			break;
	return i;
}


static inline
long
str_find_char_last(char * str, long N, char c)
{
	long i;
	for (i=N-1;i>=0;i--)
		if (str[i] == c)
			break;
	return i;
}


static inline
long
str_find_ws(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str_char_is_ws(str[i]))
			break;
	return i;
}


static inline
long
str_find_non_ws(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (!str_char_is_ws(str[i]))
			break;
	return i;
}


static inline
long
str_find_eol(char * str, long N)
{
	long i;
	for (i=0;i<N;i++)
		if (str[i] == '\n')
			break;
	return i;
}


static inline
long
str_next_word(char * str, long N)
{
	long i = 0;
	i = str_find_ws(str, N);               // Find end of current word.
	i += str_find_non_ws(str+i, N-i);
	return i;
}


//==========================================================================================================================================
//= Path Strings
//==========================================================================================================================================


static inline
char *
str_path_strip_tail(char * str, long N)
{
	long i;
	str_assert_string_termination(str, N);
	i = str_find_char_last(str, N, '/');
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
	i = str_find_char_last(str, N, '.');
	if (i <= 0 || str[i-1] == '/')      // If tail starts with '.' it is not an extension, it is a hidden file.
		return str + N - 1;
	str[i] = 0;
	return str + i + 1;
}


//==========================================================================================================================================
//= Function Declarations
//==========================================================================================================================================


long str_delimiter_word(char * str, long N);
long str_delimiter_line(char * str, long N);
long str_delimiter_eof(char * str, long N);

void str_tokenize(char * str, long N, long string_delimiter(char *, long), int keep_empty, char *** tokens_out, long ** tokens_lengths_out, long * total_tokens_out);


#endif /* STRING_UTIL_H */

