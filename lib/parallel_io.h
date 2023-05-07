#ifndef PARALLEL_IO_H
#define PARALLEL_IO_H

#include "macros/cpp_defines.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Read And Parse File To Atoms                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct File_Atoms {
	char * string;       // The whole file.
	long len;            // 'string' length (bytes).
	char ** atoms;       // Array with the addresses of the atoms in 'string'.
	long * atom_len;     // Length of each atom.
	long num_atoms;      // Number of atoms.
};


void file_atoms_clean(struct File_Atoms * obj);
void file_atoms_destroy(struct File_Atoms ** obj_ptr);

void file_to_atoms(struct File_Atoms * A, const char * filename, void string_delimiter(char *, long), int keep_empty);
void file_to_atoms_no_mmap(struct File_Atoms * A, const char * filename, void string_delimiter(char *, long), int keep_empty);

void file_to_words(struct File_Atoms * A, const char * filename, int keep_empty);
void file_csv_to_words(struct File_Atoms * A, const char * filename, int keep_empty);
void file_to_lines(struct File_Atoms * A, const char * filename, int keep_empty);
void file_to_string(struct File_Atoms * A, const char * filename);
void file_to_raw_data(struct File_Atoms * A, const char * filename);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Write File                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#if 0
#ifndef VECTOR_GEN_H_I
#define VECTOR_GEN_H_I
#undef  VECTOR_GEN_TYPE_1
#undef  VECTOR_GEN_SUFFIX
#define VECTOR_GEN_TYPE_1  int
#define VECTOR_GEN_SUFFIX  _parallel_io_i
#include "data_structures/vector_gen.h"
#endif /* VECTOR_GEN_H_I */


#ifndef VECTOR_GEN_H_C
#define VECTOR_GEN_H_C
#undef  VECTOR_GEN_TYPE_1
#undef  VECTOR_GEN_SUFFIX
#define VECTOR_GEN_TYPE_1  char
#define VECTOR_GEN_SUFFIX  _parallel_io_c
#include "data_structures/vector_gen.h"
#endif /* VECTOR_GEN_H_C */


#ifndef VECTOR_GEN_H_V
#define VECTOR_GEN_H_V
#undef  VECTOR_GEN_TYPE_1
#undef  VECTOR_GEN_SUFFIX
#define VECTOR_GEN_TYPE_1  struct Vector_c
#define VECTOR_GEN_SUFFIX  _parallel_io_v
#include "data_structures/vector_gen.h"
#endif /* VECTOR_GEN_H_V */


struct File_Content {
	struct Vector_parallel_io_v * content;
	struct Vector_parallel_io_i * content_len;
	long n;
};
#endif


#endif /* PARALLEL_IO_H */

