#include "parallel_io.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Read And Parse File To Atoms                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void
file_atoms_clean(struct File_Atoms * obj)
{
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
	file_atoms_clean(*obj_ptr);
	free(*obj_ptr);
	*obj_ptr = NULL;
}


void
file_to_atoms(struct File_Atoms * A, const char * filename, void string_delimiter(char *, long), int keep_empty)
{
	struct stat sb;
	char * mem;
	char * str;
	long N;
	int fd;

	safe_stat(filename, &sb);
	if (!S_ISREG(sb.st_mode))
		error("not a file\n");
	fd = safe_open(filename, O_RDONLY);
	mem = static_cast(char *,  safe_mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
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


// File to space-separated words.
void
file_to_words(struct File_Atoms * A, const char * filename, int keep_empty)
{
	file_to_atoms(A, filename, str_delimiter_word, keep_empty);
}


// File to lines.
void
file_to_lines(struct File_Atoms * A, const char * filename, int keep_empty)
{
	file_to_atoms(A, filename, str_delimiter_line, keep_empty);
}


// Whole file as one string.
void
file_to_string(struct File_Atoms * A, const char * filename)
{
	file_to_atoms(A, filename, str_delimiter_eof, 0);
}


// Whole file as binary data (not a NULL terminated string).
void
file_to_raw_data(struct File_Atoms * A, const char * filename)
{
	file_to_atoms(A, filename, NULL, 0);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Write File                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// MAP_SHARED
//     Share  this mapping.
//     Updates to the mapping are visible to other processes mapping the same region,
//     and in the case of FILE-BACKED MAPPINGS are carried through to the underlying file.
//     (To precisely control when updates are carried through to the underlying file requires the use of msync(2).)


static __attribute__((unused))
void
write_string_to_file(const char * filename, char * str, long str_len)
{
	char * mem;
	int fd;

	// struct stat sb;
	// safe_stat(filename, &sb);
	// if (!S_ISREG(sb.st_mode))
		// error("not a file\n");

	fd = safe_open(filename, O_RDWR | O_TRUNC | O_CREAT);
	lseek(fd, str_len-1, SEEK_SET);
	safe_write(fd, "", 1);
	mem = static_cast(char *,  safe_mmap(NULL, str_len, PROT_WRITE, MAP_SHARED, fd, 0));
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

