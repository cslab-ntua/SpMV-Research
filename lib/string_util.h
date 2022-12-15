#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "macros/cpp_defines.h"
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


#define str_assert_string_compliance(str, N)          \
do {                                                   \
	if (str == NULL)                               \
		error("NULL char pointer");            \
	else if (N <= 0)                               \
		error("empty string given");           \
	else if (str[N-1] != 0)                        \
		error("unterminated string given");    \
} while (0)


static inline
long
str_check_mem_overlap(const char * s1, long s1_n, const char * s2, long s2_n)
{
	if ((s1 == s2)
		|| (s1 < s2 && s1 + s1_n > s2)
		|| (s2 < s1 && s2 + s2_n > s1))
		return 1;
	return 0;
}


//==========================================================================================================================================
//= Character Groups
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
//= Character Conversions
//==========================================================================================================================================


// Returns the number of chars written.
static inline
long
str_char_hex_to_bin_unsafe(const char c_hex, char * buf)
{
	switch (c_hex) {
		case '0':           buf[0] = '0'; buf[1] = '0'; buf[2] = '0'; buf[3] = '0'; break;
		case '1':           buf[0] = '0'; buf[1] = '0'; buf[2] = '0'; buf[3] = '1'; break;
		case '2':           buf[0] = '0'; buf[1] = '0'; buf[2] = '1'; buf[3] = '0'; break;
		case '3':           buf[0] = '0'; buf[1] = '0'; buf[2] = '1'; buf[3] = '1'; break;
		case '4':           buf[0] = '0'; buf[1] = '1'; buf[2] = '0'; buf[3] = '0'; break;
		case '5':           buf[0] = '0'; buf[1] = '1'; buf[2] = '0'; buf[3] = '1'; break;
		case '6':           buf[0] = '0'; buf[1] = '1'; buf[2] = '1'; buf[3] = '0'; break;
		case '7':           buf[0] = '0'; buf[1] = '1'; buf[2] = '1'; buf[3] = '1'; break;
		case '8':           buf[0] = '1'; buf[1] = '0'; buf[2] = '0'; buf[3] = '0'; break;
		case '9':           buf[0] = '1'; buf[1] = '0'; buf[2] = '0'; buf[3] = '1'; break;
		case 'A': case 'a': buf[0] = '1'; buf[1] = '0'; buf[2] = '1'; buf[3] = '0'; break;
		case 'B': case 'b': buf[0] = '1'; buf[1] = '0'; buf[2] = '1'; buf[3] = '1'; break;
		case 'C': case 'c': buf[0] = '1'; buf[1] = '1'; buf[2] = '0'; buf[3] = '0'; break;
		case 'D': case 'd': buf[0] = '1'; buf[1] = '1'; buf[2] = '0'; buf[3] = '1'; break;
		case 'E': case 'e': buf[0] = '1'; buf[1] = '1'; buf[2] = '1'; buf[3] = '0'; break;
		case 'F': case 'f': buf[0] = '1'; buf[1] = '1'; buf[2] = '1'; buf[3] = '1'; break;
		default:
			return 0;
		}
	return 4;
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
	// warning: 'strncpy' output truncated before terminating nul copying 4 bytes from a string of the same length [-Wstringop-truncation]
	// #pragma GCC diagnostic push
	// #pragma GCC diagnostic ignored "-Wstringop-truncation"
	// strncpy(term, str, N);
	memcpy(term, str, N);
	// #pragma GCC diagnostic pop
	pmt = (typeof(pmt)) malloc((N+1) * sizeof(*pmt));     // +1, for backtracking in success, when looking for multiple occurrences.
	pmt[0] = -1;
	for (i=1,j=0;i<N+1;i++,j++)
	{
		if (term[i] == term[j])      // Same as next character after the current common prefix, so the common prefix grows.
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
	if (search_term == NULL)
		return;
	free(search_term->pmt);
	search_term->pmt = NULL;
}


static inline
void
str_search_term_destroy(struct str_Search_Term ** st_ptr)
{
	if (st_ptr == NULL)
		return;
	str_search_term_clean(*st_ptr);
	free(*st_ptr);
	*st_ptr = NULL;
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


static inline
long
str_find_substr_simple(const char * str, long N, const char * substr, long M)
{
	char c_first;
	long i = 0, j;
	if (M <= 0 || M > N)
		return N;
	// char * ss = memmem(str, N, substr, M);         // GNU specific: #include _GNU_SOURCE
	// if (ss == NULL)
		// return N;
	// else
		// return ss - str;
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


#define str_find_substr(str, N, search_term, ...)                                                                                                           \
({                                                                                                                                                          \
	_Generic((search_term),                                                                                                                             \
		struct str_Search_Term *:  str_find_substr_kmp(str, N, (struct str_Search_Term *) search_term),                                             \
		char *:                str_find_substr_simple(str, N, (char *) search_term, DEFAULT_ARG_1(strlen((char *) search_term), ##__VA_ARGS__)),    \
		const char *:          str_find_substr_simple(str, N, (char *) search_term, DEFAULT_ARG_1(strlen((char *) search_term), ##__VA_ARGS__)),    \
		unsigned char *:       str_find_substr_simple(str, N, (char *) search_term, DEFAULT_ARG_1(strlen((char *) search_term), ##__VA_ARGS__)),    \
		const unsigned char *: str_find_substr_simple(str, N, (char *) search_term, DEFAULT_ARG_1(strlen((char *) search_term), ##__VA_ARGS__)),    \
		default: 0                                                                                                                                  \
	);                                                                                                                                                  \
})


//==========================================================================================================================================
//= Path Strings
//==========================================================================================================================================


// Split head (the directory part) and tail (filename / last directory).
// 'head/tail' convention is taken from the python os.path.split documentation.
//
// 'path' argument is unchanged.
// 'head' (the directory part) is without a terminating '/' character, unless it is root
//        (so that an e.g. ls on the tail shows the root dir and not the current dir).
static inline
void
str_path_split_path(const char * path, long N, char * buf_dst, long buf_dst_n, char ** head_ptr_out, char ** tail_ptr_out)
{
	long buf_n = N + 3;
	char * buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
	char * head, * tail;
	long i, pos, head_n, tail_n;
	if (buf_dst_n < N + 3)
		error("'buf_dst_n' must be >= N + 3");
	str_assert_string_compliance(path, N);
	while (N > 2 && path[N-2] == '/')       // Ignore all trailing '/', but stop before the first character, as root dir is handled specially.
		N--;
	pos = str_find_char_from_end(path, N-1, '/');
	if (N <= 1)
	{
		head_n = 2;
		tail_n = 1;
		head = (char *) memcpy(buf, ".", 2);
		tail = (char *) memcpy(buf + head_n, "", tail_n);
	}
	else if (pos < 0)   // in current dir
	{
		head_n = 2;
		tail_n = N;
		head = (char *) memcpy(buf, ".", 2);
		if (tail_n == 2 && path[0] == '.')
			tail = (char *) memcpy(buf + head_n, "", 1);
		else
			tail = (char *) memcpy(buf + head_n, path, tail_n);
	}
	else if (pos == 0)  // in root dir
	{
		head_n = 2;
		tail_n = N - 1;
		head = (char *) memcpy(buf, "/", 2);
		tail = (char *) memcpy(buf + head_n, path + 1, tail_n);
	}
	else
	{
		head_n = pos + 1;
		tail_n = N - head_n;
		head = (char *) memcpy(buf, path, head_n);
		tail = (char *) memcpy(buf + head_n, path + head_n, tail_n);
	}
	while (head_n > 2 && path[head_n-2] == '/')      // Remove all trailing '/', unless it is root.
		head_n--;

	if (head_n + tail_n > buf_n)
		error("buffer overflow");
	head[head_n - 1] = '\0';
	tail[tail_n - 1] = '\0';
	for (i=0;i<head_n;i++)
		buf_dst[i] = head[i];
	for (i=0;i<tail_n;i++)
		buf_dst[head_n + i] = tail[i];
	head = buf_dst;
	tail = buf_dst + head_n;
	if (head_ptr_out != NULL)
		*head_ptr_out = head;
	if (tail_ptr_out != NULL)
		*tail_ptr_out = tail;
	free(buf);
}


// Split the extension and the base (the path part without the extension and the dot).
//
// 'path' and 'buf_dst' buffers can safely overlap.
// 'path' argument is unchanged, unless it overlaps with 'buf_dst'.
//
// 'ext_ptr_out' (the extension part) is returned without the leading dot '.'.
static inline
void
str_path_split_ext(char * path, long N, char * buf_dst, long buf_dst_n, char ** base_ptr_out, char ** ext_ptr_out)
{
	long buf_n = N + 3;
	char * buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
	char * base, * ext;
	long i, pos, base_n, ext_n;
	if (buf_dst_n < N + 3)
		error("'buf_dst_n' must be >= N + 3");
	str_assert_string_compliance(path, N);
	while (N > 2 && path[N-2] == '/')     // Ignore all trailing '/', but stop before the first character, as root dir is handled specially.
		N--;
	pos = str_find_char_from_end(path, N, '.');
	// If a name starts with '.' it is not an extension, it is a hidden dir/file.
	if ((pos <= 0)                        // /dir/file , .file
		|| (pos == N-1)               // /dir/. , /dir/..
		|| (path[pos+1] == '/')       // /dir/./file , /dir/../file
		|| (path[pos-1] == '/')       // /dir/.file
		)
	{
		base_n = N;
		ext_n = 1;
		base = (char *) memcpy(buf, path, base_n);
		ext = (char *) memcpy(buf + base_n, "", 1);
	}
	else
	{
		base_n = pos + 1;
		ext_n = N - base_n;
		base = (char *) memcpy(buf, path, base_n);
		ext = (char *) memcpy(buf + base_n, path + base_n, ext_n);
	}
	if (base_n + ext_n > buf_n)
		error("buffer overflow");
	base[base_n - 1] = '\0';
	ext[ext_n - 1] = '\0';
	for (i=0;i<base_n;i++)
		buf_dst[i] = base[i];
	for (i=0;i<ext_n;i++)
		buf_dst[base_n + i] = ext[i];
	base = buf_dst;
	ext = buf_dst + base_n;
	if (base_ptr_out != NULL)
		*base_ptr_out = base;
	if (ext_ptr_out != NULL)
		*ext_ptr_out = ext;
	free(buf);
}


static inline
char *
str_path_strip_tail(char * str, long N)
{
	long i;
	str_assert_string_compliance(str, N);
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
	str_assert_string_compliance(str, N);
	i = str_find_char_from_end(str, N, '.');
	if (i <= 0 || str[i-1] == '/')      // If path starts with '.' it is not an extension, it is a hidden file.
		return str + N - 1;
	str[i] = 0;
	return str + i + 1;
}


//==========================================================================================================================================
//= Function Declarations
//==========================================================================================================================================


void str_delimiter_eof(char * str, long N);
void str_delimiter_line(char * str, long N);
void str_delimiter_word(char * str, long N);
void str_delimiter_csv(char * str, long N);

void str_tokenize(char * str, long N, void string_delimiter(char *, long), int keep_empty, char *** tokens_out, long ** tokens_lengths_out, long * total_tokens_out);


#endif /* STRING_UTIL_H */

