#ifndef PARALLEL_IO_H
#define PARALLEL_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "debug.h"
#include "io.h"
#include "string_util.h"


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


void file_atoms_destructor(struct File_Atoms ** obj_ref);
void file_atoms_destroy(struct File_Atoms * obj);

struct File_Atoms * file_to_atoms(const char * filename, long string_delimiter(char *, long), int keep_empty);
struct File_Atoms * file_to_words(const char * filename, int keep_empty);
struct File_Atoms * file_to_lines(const char * filename, int keep_empty);
struct File_Atoms * file_to_string(const char * filename);
struct File_Atoms * file_to_raw_data(const char * filename);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Write File                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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


#endif /* PARALLEL_IO_H */

