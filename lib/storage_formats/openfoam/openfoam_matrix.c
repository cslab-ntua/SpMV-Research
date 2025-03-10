#include <stdlib.h>
#include <stdio.h>

#include "debug.h"
#include "string_util.h"
#include "parallel_io.h"

#include "openfoam_matrix.h"


/*
 * 2 lduMatrix
 * The lduMatrix class stores the matrix coefficients in three arrays:
 *     scalarField *lowerPtr_;
 *     scalarField *diagPtr_;
 *     scalarField *upperPtr_;
 * These arrays are Fields that contain all the non-zero entries in the coefficient matrix.
 * 
 * The access of the coefficients goes over the functions:
 *     scalarField& upper()
 *     scalarField& lower()
 *     scalarField& diag()
 *
 * The size of the arrays lowerPtr_ and upperPtr_ are equal to the number of INTERNAL faces.
 * The size of the array diagPtr_ is equal to the number of cells.
 */


void
read_openfoam_matrix(char * filename_owner, char * filename_neigh,
		int ** rowind_out, int ** colind_out, long * N_out, long * nnz_non_diag_out)
{
	__attribute__((cleanup(file_atoms_destroy))) struct File_Atoms * A_owner;
	__attribute__((cleanup(file_atoms_destroy))) struct File_Atoms * A_neigh;
	long __attribute__((unused)) num_points = 0, num_cells = 0, __attribute__((unused)) num_faces = 0, num_internal_faces = 0;
	int * rowind, * colind;
	char * line, * str;
	long line_len;
	long i, j, k;
	long i_base, j_base;
	long nnz;

	A_owner = (typeof(A_owner)) malloc(sizeof(*A_owner));
	A_neigh = (typeof(A_neigh)) malloc(sizeof(*A_neigh));

	file_to_lines(A_owner, filename_owner, 0);
	file_to_lines(A_neigh, filename_neigh, 0);

	__attribute__((cleanup(str_search_term_destroy))) struct str_Search_Term * st_note, * st_points, * st_cells, * st_faces, * st_ifaces;
	str = "note";
	st_note = str_search_term_new(str, strlen(str));
	str = "nPoints:";
	st_points = str_search_term_new(str, strlen(str));
	str = "nCells:";
	st_cells = str_search_term_new(str, strlen(str));
	str = "nFaces:";
	st_faces = str_search_term_new(str, strlen(str));
	str = "nInternalFaces:";
	st_ifaces = str_search_term_new(str, strlen(str));
	for (i=0;i<A_owner->num_atoms;i++)
	{
		line = A_owner->atoms[i];
		line_len = A_owner->atom_len[i] + 1;
		k = str_find_substr(line, line_len, st_note);
		if (k < line_len)     // note        "nPoints:6304  nCells:5014  nFaces:16257  nInternalFaces:13827";
		{
			k = str_find_substr(line, line_len, st_points) + st_points->n;
			num_points = (k < line_len) ? atol(line+k) : 0;

			k = str_find_substr(line, line_len, st_cells) + st_cells->n;
			num_cells = (k < line_len) ? atol(line+k) : 0;

			k = str_find_substr(line, line_len, st_faces) + st_faces->n;
			num_faces = (k < line_len) ? atol(line+k) : 0;

			k = str_find_substr(line, line_len, st_ifaces) + st_ifaces->n;
			num_internal_faces = (k < line_len) ? atol(line+k) : 0;
		}
		if (line[0] == '(')
			break;
	}
	i++;
	i_base = i;

	for (j=0;j<A_neigh->num_atoms;j++)
		if (A_neigh->atoms[j][0] == '(')
			break;
	j++;
	j_base = j;

	// fprintf(stderr, "points=%ld , cells=%ld , faces=%ld , int_faces=%ld\n", num_points, num_cells, num_faces, num_internal_faces);

	nnz = num_cells + 2 * num_internal_faces;

	rowind = malloc(nnz * sizeof(*rowind));
	colind = malloc(nnz * sizeof(*colind));

	// Non-diagonal elements.
	for (i=0;i<num_internal_faces;i++)
	{
		rowind[2*i] = colind[2*i+1] = atoi(A_owner->atoms[i_base+i]);
		colind[2*i] = rowind[2*i+1] = atoi(A_neigh->atoms[j_base+i]);
	}

	// Diagonal elements.
	for (i=0;i<num_cells;i++)
	{
		rowind[2*num_internal_faces + i] = i;
		colind[2*num_internal_faces + i] = i;
	}

	*rowind_out = rowind;
	*colind_out = colind;
	*N_out = num_cells;
	*nnz_non_diag_out = 2 * num_internal_faces;
}


void
read_openfoam_matrix_dir(char * dir,
		int ** rowind_out, int ** colind_out, long * N_out, long * nnz_non_diag_out)
{
	long buf_n = 1000;
	char filename_owner[buf_n];
	char filename_neigh[buf_n];
	snprintf(filename_owner, buf_n, "%s/owner", dir);
	snprintf(filename_neigh, buf_n, "%s/neighbour", dir);
	read_openfoam_matrix(filename_owner, filename_neigh, rowind_out, colind_out, N_out, nnz_non_diag_out);
}

