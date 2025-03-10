#ifndef PARALLEL_IO_H
#define PARALLEL_IO_H

#include "macros/cpp_defines.h"


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Load File To Memory                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


/* Load file to memory as a string. */
void file_load(char * path, int use_mmap, long auto_decompress, char ** str_out, long * N_out);


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     Read And Parse File To Atoms                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


struct File_Atoms {
	char * string;       // The whole file.
	long len;            // 'string' length (bytes).
	char ** atoms;       // Array with the addresses of the atoms in 'string'.
	long * atom_len;     // Length of each atom.
	long num_atoms;      // Number of atoms.
};


void file_atoms_clean(struct File_Atoms * obj);
void file_atoms_destroy(struct File_Atoms ** obj_ptr);

void file_to_atoms(struct File_Atoms * A, char * path, void string_delimiter(char *, long), int keep_empty);
void file_to_atoms_no_mmap(struct File_Atoms * A, char * path, void string_delimiter(char *, long), int keep_empty);

void file_to_words(struct File_Atoms * A, char * path, int keep_empty);
void file_csv_to_words(struct File_Atoms * A, char * path, int keep_empty);
void file_to_lines(struct File_Atoms * A, char * path, int keep_empty);
void file_to_string(struct File_Atoms * A, char * path);
void file_to_raw_data(struct File_Atoms * A, char * path);


//==========================================================================================================================================
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                              Write File                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//==========================================================================================================================================


#endif /* PARALLEL_IO_H */

