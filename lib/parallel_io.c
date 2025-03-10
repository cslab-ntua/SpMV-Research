#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <omp.h>

#include "debug.h"
#include "io.h"
#include "string_util.h"

#include "parallel_io.h"


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Load File To Memory                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


static
long
extract_cmd(char * buf, long buf_n, char * ext, char * extract_dir)
{
	long i;

	i = 0;
	if (!strcmp(ext, "gz") || !strcmp(ext, "zst"))
	{
		i += snprintf(buf, buf_n, "zstdmt -d --stdout ");
		// i += snprintf(buf, buf_n, "pigz -dc ");
	}
	else if (!strcmp(ext, "tar"))
	{
		i += snprintf(buf, buf_n, "tar -x -O -C \"%s\" ", extract_dir);
	}
	return i;
}


/* Load file to memory as a string. */
void
file_load(char * path, int use_mmap, long auto_decompress,
		char ** str_out, long * N_out)
{
	struct stat sb;
	char * mem;
	char * str;
	long N;
	int fd;

	long compressed_file = 0;
	[[gnu::cleanup(cleanup_free)]] char * basename = NULL, * basename_no_ext = NULL, * ext = NULL, * path_extracted = NULL;
	[[gnu::cleanup(cleanup_free)]] char * tmp_dir = NULL;
	long buf_n = 100000 + strlen(path);
	[[gnu::cleanup(cleanup_free)]] char * buf = (typeof(buf)) malloc(buf_n * sizeof(*buf));
	[[gnu::cleanup(cleanup_free)]] char * cmd = (typeof(cmd)) malloc(buf_n * sizeof(*cmd));
	long i;

	if (auto_decompress)
	{
		compressed_file = 0;

		tmp_dir = (typeof(tmp_dir)) malloc(buf_n * sizeof(*tmp_dir));

		// printf("path='%s'\n", path);
		snprintf(tmp_dir, buf_n, "/tmp/temp_XXXXXX");
		if (mkdtemp(tmp_dir) == NULL)
			error("mkdtemp(): %s", tmp_dir);

		str_path_split_path(path, strlen(path) + 1, buf, buf_n, NULL, &basename);
		basename = strdup(basename);

		i = 0;
		// i += snprintf(cmd + i, buf_n - i, "time -v cat \"%s\" ", path);
		i += snprintf(cmd + i, buf_n - i, "cat \"%s\" ", path);
		while(1)
		{
			// printf("cmd='%s'\n", cmd);
			free(basename_no_ext);
			free(ext);
			str_path_split_ext(basename, strlen(basename) + 1, buf, buf_n, &basename_no_ext, &ext);
			basename_no_ext = strdup(basename_no_ext);
			ext = strdup(ext);
			// printf("basename='%s' basename_no_ext='%s' ext='%s'\n", basename, basename_no_ext, ext);
			if (!strcmp(ext, "tar") || !strcmp(ext, "gz") || !strcmp(ext, "zst"))
			{
				compressed_file = 1;
				i += snprintf(cmd + i, buf_n - i, " | ");
				i += extract_cmd(cmd + i, buf_n - i, ext, tmp_dir);
				free(basename);
				basename = strdup(basename_no_ext);
				continue;
			}
			break;
		}

		if (compressed_file)
		{
			snprintf(buf, buf_n, "%s/%s", tmp_dir, basename);
			path_extracted = strdup(buf);
			path = path_extracted;
			i += snprintf(cmd + i, buf_n - i, " > \"%s\" ", path_extracted);
			printf("cmd='%s'\n", cmd);
			if (system(cmd))
				error("system");
		}
	}

	safe_stat(path, &sb);
	if (!S_ISREG(sb.st_mode))
		error("not a file");
	fd = safe_open(path, O_RDONLY);

	if (use_mmap)
	{
		mem = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (mem == MAP_FAILED)
		{
			// Some files cannot be mapped, e.g. sysfs files.
			// If 'mmap()' fails with ENODEV, try simple 'read()'.
			//     ENODEV: The underlying filesystem of the specified file does not support memory mapping.
			if (errno == ENODEV)
				use_mmap = 0;
			else
				error("mmap()");
		}
	}

	if (use_mmap) // Note: 'use_mmap' can change above.
	{
		safe_close(fd);

		N = sb.st_size+1;    // Will add a '\0' character.
		str = (typeof(str)) malloc(N * sizeof(*str));
		str[N-1] = 0;
		_Pragma("omp parallel")
		{
			long i;
			_Pragma("omp for schedule(static)")
			for (i=0;i<sb.st_size;i++)
			{
				str[i] = mem[i];
			}
		}

		safe_munmap(mem, sb.st_size);
	}
	else
	{
		N = read_until_EOF(fd, &str);
		safe_close(fd);
	}

	if (auto_decompress)
	{
		remove_recursive(tmp_dir);
	}

	*str_out = str;
	*N_out = N;
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Read And Parse File To Atoms                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


void
file_atoms_clean(struct File_Atoms * obj)
{
	if (obj == NULL)
		return;
	free(obj->atom_len);
	obj->atom_len = NULL;
	free(obj->atoms);
	obj->atoms = NULL;
	free(obj->string);
	obj->string = NULL;
}


// Pass a reference (**) to be able to set to NULL,
// and also so that it can be used with '__attribute__((cleanup()))'.
void
file_atoms_destroy(struct File_Atoms ** obj_ptr)
{
	if (obj_ptr == NULL)
		return;
	file_atoms_clean(*obj_ptr);
	free(*obj_ptr);
	*obj_ptr = NULL;
}


static
void
file_to_atoms_base(struct File_Atoms * A, char * path, void string_delimiter(char *, long), int keep_empty, int use_mmap)
{
	char * str;
	long N;
	file_load(path, use_mmap, 1, &str, &N);

	A->string = str;
	A->len = N;
	A->atoms = NULL;
	A->atom_len = NULL;
	A->num_atoms = 0;

	if (string_delimiter == NULL)  // No parsing (e.g. binary file).
	{
		A->len--;              // Remove inserted '\0'.
		return;
	}
	str_tokenize(str, N, string_delimiter, keep_empty, &A->atoms, &A->atom_len, &A->num_atoms);
}


void
file_to_atoms(struct File_Atoms * A, char * path, void string_delimiter(char *, long), int keep_empty)
{
	file_to_atoms_base(A, path, string_delimiter, keep_empty, 1);
}


void
file_to_atoms_no_mmap(struct File_Atoms * A, char * path, void string_delimiter(char *, long), int keep_empty)
{
	file_to_atoms_base(A, path, string_delimiter, keep_empty, 0);
}


// File to space-separated words.
void
file_to_words(struct File_Atoms * A, char * path, int keep_empty)
{
	file_to_atoms(A, path, str_delimiter_word, keep_empty);
}


// CSV file to space-separated words.
void
file_csv_to_words(struct File_Atoms * A, char * path, int keep_empty)
{
	file_to_atoms(A, path, str_delimiter_csv, keep_empty);
}


// File to lines.
void
file_to_lines(struct File_Atoms * A, char * path, int keep_empty)
{
	file_to_atoms(A, path, str_delimiter_line, keep_empty);
}


// Whole file as one string.
void
file_to_string(struct File_Atoms * A, char * path)
{
	file_to_atoms(A, path, str_delimiter_eof, 0);
}


// Whole file as binary data (not a NULL terminated string).
void
file_to_raw_data(struct File_Atoms * A, char * path)
{
	file_to_atoms(A, path, NULL, 0);
}


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Write File                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


// MAP_SHARED
//     Share  this mapping.
//     Updates to the mapping are visible to other processes mapping the same region,
//     and in the case of FILE-BACKED MAPPINGS are carried through to the underlying file.
//     (To precisely control when updates are carried through to the underlying file requires the use of msync(2).)


static __attribute__((unused))
void
write_string_to_file(char * path, char * str, long str_len)
{
	char * mem;
	int fd;

	// struct stat sb;
	// safe_stat(path, &sb);
	// if (!S_ISREG(sb.st_mode))
		// error("not a file");

	fd = safe_open(path, O_RDWR | O_TRUNC | O_CREAT);
	lseek(fd, str_len-1, SEEK_SET);
	safe_write(fd, "", 1);
	mem = (char *) safe_mmap(NULL, str_len, PROT_WRITE, MAP_SHARED, fd, 0);
	safe_close(fd);

	#pragma omp parallel
	{
		long i;
		#pragma omp for
		for (i=0;i<str_len;i++)
		{
			mem[i] = str[i];
		}
	}
	msync(mem, str_len, MS_SYNC);
	munmap(mem, str_len);
}

