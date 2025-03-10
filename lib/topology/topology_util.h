#ifndef _GNU_SOURCE
	#error "Define _GNU_SOURCE at the top level to compile this library."
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <regex.h>
#include <pthread.h>
#include <omp.h>
#include <sched.h>

#include "debug.h"
#include "string_util.h"
#include "pthread_functions.h"
#include "omp_functions.h"
#include "io.h"

#include "hardware_topology.h"


static inline
long
hex_chars_to_bin_chars(char * str_hex, long N, char ** str_bin_ret)
{
	long buf_n = N * 4 + 1;
	char * buf;
	char c;
	long i, j;
	if (str_bin_ret == NULL)
		error("'str_bin_ret' output buffer is NULL");
	buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
	for (i=0,j=0;i<N;i++)
	{
		c = str_hex[i];
		j += str_hex_char_to_bin_char_array(c, buf + j, buf_n - j);
	}
	*str_bin_ret = buf;
	return j;
}


static inline
long
bin_chars_to_int_list(char * bin_str, long N, long ** num_list_ret)
{
	long * num_list;
	long i, j;
	if (num_list_ret == NULL)
		error("'num_list_ret' output buffer is NULL");
	num_list = (typeof(num_list)) malloc(N * sizeof(*num_list));
	for (i=N-1,j=0;i>=0;i--)
	{
		if (bin_str[i] == '1')
			num_list[j++] = N - 1 - i;
	}
	*num_list_ret = num_list;
	return j;
}


static inline
long
hex_chars_to_int_list(char * str_hex, long N, long ** num_list_ret)
{
	char * str_bin;
	long num_int;
	long * int_list;
	long len;
	if (num_list_ret == NULL)
		error("'num_list_ret' output buffer is NULL");
	len = hex_chars_to_bin_chars(str_hex, N, &str_bin);
	num_int = bin_chars_to_int_list(str_bin, len, &int_list);
	free(str_bin);
	*num_list_ret = int_list;
	return num_int;
}


static inline
char *
parse_file_string(const char * filename)
{
	char * str, * ret;
	read_sysfs_file(filename, &str);
	ret = strdup(str);
	free(str);
	return ret;
}


static inline
long
parse_file_int(const char * filename)
{
	char * str;
	long num;
	read_sysfs_file(filename, &str);
	num = atol(str);
	free(str);
	return num;
}


static inline
long
parse_file_int_human_format(const char * filename)
{
	char * str;
	long num;
	long len;
	len = read_sysfs_file(filename, &str);
	num = atol(str);
	switch (str[len-2]) {  // It has a newline character at the end, so the position is at len-2.
		case 'K': num *= 1024LL; break;
		case 'M': num *= 1024LL * 1024LL; break;
		case 'G': num *= 1024LL * 1024LL * 1024LL; break;
		case 'T': num *= 1024LL * 1024LL * 1024LL * 1024LL; break;
	}
	free(str);
	return num;
}


static inline
long
parse_file_hex_num_list(const char * filename, long ** num_list_ret)
{
	char * str_hex;
	long * num_list;
	long len;
	len = read_sysfs_file(filename, &str_hex);
	len = hex_chars_to_int_list(str_hex, len, &num_list);
	free(str_hex);
	if (num_list_ret != NULL)
		*num_list_ret = num_list;
	else
		free(num_list);
	return len;
}


static inline
long
find_sequential_files_num(char * path_prefix)
{
	long buf_n = 1000;
	char buf[buf_n];
	long i;
	i = 0;
	while (1)
	{
		snprintf(buf, buf_n, "%s%ld", path_prefix, i);
		if (!stat_isdir(buf))
			break;
		i++;
	}
	return i;
}


static inline
int
cmp_long(const void * a_ptr, const void * b_ptr)
{
	long a = *((long *) a_ptr);
	long b = *((long *) b_ptr);
	return a > b ? 1 : a < b ? -1 : 0;
}

static inline
long
find_numbered_files_list(char * dirname, char * filename_prefix, long ** number_list_ret)
{
	long buf_n = 1000;
	char buf[buf_n];
	DIR * dr;           // opendir() returns a pointer of DIR type.
	struct dirent *de;  // Pointer for directory entry
	regex_t regex;
	long num_files;
	long * list;
	long i;

	snprintf(buf, buf_n, "%s[[:digit:]]+", filename_prefix);
	if (regcomp(&regex, buf, REG_EXTENDED))
		error("regcomp");

	dr = opendir(dirname);
	if (dr == NULL)
		error("opendir");
	i = 0;
	while (1)
	{
		de = readdir(dr);
		if (de == NULL)
			break;
		if (regexec(&regex, de->d_name, 0, NULL, 0) == 0)
		{
			i++;
		}
	}
	closedir(dr);
	num_files = i;

	if (number_list_ret != NULL)
	{
		list = (typeof(list)) malloc(num_files * sizeof(*list));
		dr = opendir(dirname);
		if (dr == NULL)
			error("opendir");
		i = 0;
		while (1)
		{
			de = readdir(dr);
			if (de == NULL)
				break;
			if (regexec(&regex, de->d_name, 0, NULL, 0) == 0)
			{
				list[i] = atol((de->d_name) + strlen(filename_prefix));
				i++;
			}
		}
		closedir(dr);
		qsort(list, num_files, sizeof(*list), cmp_long);
		*number_list_ret = list;
	}

	regfree(&regex);
	return num_files;
}


// Takes a random stream of numbers and returns a translation to contiguous numbering,
// according to the order/rank of the first occurence of each number in the stream.
static inline
long
get_random_to_contiguous_numbering_translation(void * A, long N, long num_max, long (* get_val)(void * A, long i),
		long ** dict_rand_to_cont_ret)
{
	long * dict_rand_to_cont;
	long N_uniq;
	long i;
	if (dict_rand_to_cont_ret == NULL)
		error("dict_rand_to_cont_ret is NULL");
	dict_rand_to_cont = (typeof(dict_rand_to_cont)) malloc((num_max+1) * sizeof(*dict_rand_to_cont));
	for (i=0;i<num_max+1;i++)
		dict_rand_to_cont[i] = -1;
	for (i=0;i<N;i++)
		dict_rand_to_cont[get_val(A, i)] = 1;
	N_uniq = 0;
	for (i=0;i<=num_max;i++)
	{
		if (dict_rand_to_cont[i] > 0)
		{
			dict_rand_to_cont[i] = N_uniq;
			N_uniq++;
		}
	}
	*dict_rand_to_cont_ret = dict_rand_to_cont;
	return N_uniq;
}


static inline
void
translate(long * A, long N, long * dict,
		long * A_out)
{
	long i;
	for (i=0;i<N;i++)
	{
		A_out[i] = dict[A[i]];
	}
}

