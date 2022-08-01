/*
 * Copyright (C) 2014, Computing Systems Laboratory (CSLab), NTUA
 * Copyright (C) 2014, Athena Elafrou
 * All rights reserved.
 *
 * This file is distributed under the BSD License. See LICENSE.txt for details.
 */

/**
 * \file sparsex_test.c
 * \brief Simple program for testing the SparseX API
 *
 * \author Athena Elafrou
 * \date 2014
 * \copyright This file is distributed under the BSD License. See LICENSE.txt
 * for details.
 */

#include <sparsex/sparsex.h>
#include "CsxCheck.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


#include <math.h>
#include <omp.h>

#include "random.h"
#include "ordered_set.h"

#include "artificial_matrix_generation.h"
#include "matrix_util.h"

#include "debug.h"
#include "time_it.h"

static const char *program_name;

static struct option long_options[] = {
	{"option",              required_argument,  0, 'o'},
	{"parameters",          required_argument,  0, 'p'},
	{"enable-timing",       no_argument,        0, 't'},
	{"enable-reordering",   no_argument,        0, 'r'},
	{"enable-verbose",      no_argument,        0, 'v'},
	{"help",                no_argument,        0, 'h'}
};

static void print_usage()
{
	fprintf(
			stdout,
			"Usage: %s [-o <option=value>]... [-t] <mmf_file>\n\n"
			"\t-o, --option <option=value>\tset a preprocessing option\n"
			"\t-p, --parameters \t\tParameters for artificial matrix generator\n"
			"\t-r, --enable-timing \t\tenable timing of the SpMV kernel\n"
			"\t-t, --enable-reordering \tenable reordering of the input matrix\n"   
			"\t-v, --enable-verbose \tenable verbose output\n"   
			"\t-h, --help\t\t\tprint this help message and exit\n",
			basename(program_name));
}

static void set_option(const char *arg)
{
	// copy arg first to work with strtok
	char *temp = malloc(strlen(arg) + 1);
	temp = strncpy(temp, arg, strlen(arg) + 1);
	// printf("\ttemp = %s\n", temp);
	char *option = strtok(temp, "=");
	char *value = strtok(NULL, "=");
	spx_option_set(option, value);
	free(temp);
}

int main(int argc, char **argv)
{
	const size_t loops = 128;

	spx_init();
	// spx_log_verbose_console();

	char c;
	char *matrix_file = NULL;
	spx_input_t *input;
	char *option;
	int option_index = 0;
	int enable_timing = 0;
	int enable_verbose = 0;
	int enable_reordering = 0;
	const spx_value_t alpha = 0.5;

	// printf("-------------------\n");
	// for(int iii=0; iii<argc; iii++){
	// 	printf("argv[%d] = %s\n", iii, argv[iii]);
	// }
	// printf("-------------------\n");

	// _Pragma("omp parallel")
	// {
	// 	int tnum = omp_get_thread_num();
	// 	printf("tnum = %d\n", tnum);
	// }
	// _Pragma("omp barrier")

    // artificial matrix generation on the way!
    struct csr_matrix * csr;
    long nr_rows;
    long nr_cols;
    double avg_nnz_per_row, std_nnz_per_row;
    unsigned int seed;
    char * distribution;
    char * placement;
    double bw;
    double skew;
    double avg_num_neighbours;
    double cross_row_similarity;
    
    int cnt = 0;
    int artificial_flag = 0;
    
	program_name = argv[0];
	while ((c = getopt_long(argc, argv, "o:p:rthv", long_options, &option_index)) != -1) {
		// printf("c = %d\n", c);
		switch (c) {
			case 'o':
				option = optarg;
				set_option(option);
				break;
			case 'p':
				artificial_flag = 1;
				option = optarg;
				// printf("optarg = %s\n", option);

				char * pch;
				pch = strtok (option, " ");
				nr_rows = atoi(pch);
				// printf("nr_rows = %ld\n", nr_rows);

				pch = strtok (NULL, " ");
			    nr_cols = atoi(pch);
			    // printf("nr_cols = %ld\n", nr_cols);

			    pch = strtok (NULL, " ");
			    avg_nnz_per_row = strtof(pch, NULL);
			    // printf("avg_nnz_per_row = %lf\n", avg_nnz_per_row);

			    pch = strtok (NULL, " ");
			    std_nnz_per_row = strtof(pch, NULL);
			    // printf("std_nnz_per_row = %lf\n", std_nnz_per_row);

			    pch = strtok (NULL, " ");
			    distribution = (char*)pch;
			    // printf("distribution = %s\n", distribution);

			    pch = strtok (NULL, " ");
			    placement = (char*)pch;
			    // printf("placement = %s\n", placement);

			    pch = strtok (NULL, " ");
			    bw = strtof(pch, NULL);
			    // printf("bw = %lf\n", bw);

			    pch = strtok (NULL, " ");
			    skew = strtof(pch, NULL);
			    // printf("skew = %lf\n", skew);

			    pch = strtok (NULL, " ");
			    avg_num_neighbours = strtof(pch, NULL);
			    // printf("avg_num_neighbours = %lf\n", avg_num_neighbours);

			    pch = strtok (NULL, " ");
			    cross_row_similarity = strtof(pch, NULL);
			    // printf("cross_row_similarity = %lf\n", cross_row_similarity);

			    pch = strtok (NULL, " ");
			    seed = atoi(pch);
			    // printf("seed = %ld\n", seed);

			    pch = strtok (NULL, " ");
				break;
			case 'r':
				enable_reordering = 1;
				break;
			case 't':
				enable_timing = 1;
				break;
			case 'v':
				enable_verbose = 1;
				spx_log_verbose_console();
				break;
			case 'h':
				print_usage();
				exit(0);
			// default:
				// print_usage();
				// exit(1);
		}
		if(c==255)
			break;
	}

	// int remargs = argc - optind;
	// if (remargs < 1) {
	// 	print_usage();
	// 	exit(1);
	// }	

	if(artificial_flag == 0){
		argv = &argv[optind];
		matrix_file = argv[0];
		// printf("matrix_file = %s\n\n", matrix_file);

		/* Load matrix from MMF file */
		input = spx_input_load_mmf(matrix_file);
		if (input == SPX_INVALID_INPUT) {
			SETERROR_0(SPX_ERR_INPUT_MAT);
			exit(1);
		}
	}
	else{
		
		// printf("1\n");
		csr = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);
		// csr_matrix_print(csr);

		input = spx_input_load_csr(csr->row_ptr, csr->col_ind, csr->values,
								   csr->nr_rows, csr->nr_cols);

		if (input == SPX_INVALID_INPUT) {
			SETERROR_0(SPX_ERR_INPUT_MAT);
			exit(1);
		}
	}

	/******************************************************************************/
	spx_timer_t t_b;
	spx_timer_clear(&t_b);
	spx_timer_start(&t_b);		

	/* Transform to CSX */
	spx_matrix_t *A = SPX_INVALID_MAT;
	if (enable_reordering) {
		A = spx_mat_tune(input, SPX_MAT_REORDER);
	} else {
		A = spx_mat_tune(input);
	}

	if (A == SPX_INVALID_MAT) {
		SETERROR_0(SPX_ERR_TUNED_MAT);
		exit(1);
	}

	spx_partition_t *parts = spx_mat_get_partition(A);
	if (parts == SPX_INVALID_PART) {
		SETERROR_0(SPX_ERR_PART);
		exit(1);
	}

	/* Create random x and y vectors */
	spx_vector_t *x = spx_vec_create_random(spx_mat_get_ncols(A), parts);
	if (x == SPX_INVALID_VEC) {
		SETERROR_0(SPX_ERR_VEC);
		exit(1);
	}

	spx_vector_t *y = spx_vec_create(spx_mat_get_nrows(A), parts);
	if (y == SPX_INVALID_VEC) {
		SETERROR_0(SPX_ERR_VEC);
		exit(1);
	}

	/* Reorder vectors */
	spx_perm_t *p = SPX_INVALID_PERM;
	if (enable_reordering) {
		p = spx_mat_get_perm(A);
		if (p == SPX_INVALID_PERM) {
			SETERROR_0(SPX_ERR_PERM);
			exit(1);
		}
		spx_vec_reorder(x, p);
	}

	spx_timer_pause(&t_b);
	double time_balance = spx_timer_get_secs(&t_b);
	/******************************************************************************/

	/* Warm up */
	spx_timer_t t_w;
	spx_timer_clear(&t_w);
	spx_timer_start(&t_w);
	spx_matvec_mult(alpha, A, x, y);
	spx_timer_pause(&t_w);
	double time_warm_up = spx_timer_get_secs(&t_w);

	/* After warm up */
	spx_timer_t t_aw;
	spx_timer_clear(&t_aw);
	spx_timer_start(&t_aw);
	spx_matvec_mult(alpha, A, x, y);
	spx_timer_pause(&t_aw);
	double time_after_warm_up = spx_timer_get_secs(&t_aw);

	/******************************************************************************/

	/* Run a matrix-vector multiplication: y <-- alpha*A*x */
	spx_timer_t t;
	spx_timer_clear(&t);
	spx_timer_start(&t);

	for (size_t i = 0; i < loops; i++) {
		spx_matvec_mult(alpha, A, x, y);
	}

	spx_timer_pause(&t);
	double time = spx_timer_get_secs(&t);

	int m = spx_mat_get_nrows(A);
	int n = spx_mat_get_ncols(A);
	int nnz = spx_mat_get_nnz(A);

	double gflops = (double) (2*loops*nnz + m) / ((double) 1e9*time);
	double mem_footprint = (double) (nnz*(sizeof(double) + sizeof(int)) + (m+1)*sizeof(int))/(1024*1024);

	fprintf(stdout,"SPMV time: %lf secs\n", time);
	fprintf(stdout,"GFLOPS: %lf\n", gflops);
	double W_avg = 250;
	double J_estimated = W_avg*time;

	if(artificial_flag == 0){
		fprintf(stderr, "%s,", matrix_file);
		fprintf(stderr, "%d,", omp_get_max_threads());
		fprintf(stderr, "%u,", m);
		fprintf(stderr, "%u,", n);
		fprintf(stderr, "%u,", nnz);
		fprintf(stderr, "%lf,", time);
		fprintf(stderr, "%lf,", gflops);
		fprintf(stderr, "%lf,", mem_footprint);
		fprintf(stderr, "%lf,", time_balance);
		fprintf(stderr, "%lf,", time_warm_up);
		fprintf(stderr, "%lf\n", time_after_warm_up);
	}
	else{
		fprintf(stderr, "synthetic,");
		fprintf(stderr, "%s,", csr->distribution);
		fprintf(stderr, "%s,", csr->placement);
		fprintf(stderr, "%d,", csr->seed);
		fprintf(stderr, "%u,", csr->nr_rows);
		fprintf(stderr, "%u,", csr->nr_cols);
		fprintf(stderr, "%u,", csr->nr_nzeros);
		fprintf(stderr, "%lf,", csr->density);
		fprintf(stderr, "%lf,", csr->mem_footprint);
		fprintf(stderr, "%s,", csr->mem_range);
		fprintf(stderr, "%lf,", csr->avg_nnz_per_row);
		fprintf(stderr, "%lf,", csr->std_nnz_per_row);
		fprintf(stderr, "%lf,", csr->avg_bw);
		fprintf(stderr, "%lf,", csr->std_bw);
		fprintf(stderr, "%lf,", csr->avg_bw_scaled);
		fprintf(stderr, "%lf,", csr->std_bw_scaled);
		fprintf(stderr, "%lf,", csr->avg_sc);
		fprintf(stderr, "%lf,", csr->std_sc);
		fprintf(stderr, "%lf,", csr->avg_sc_scaled);
		fprintf(stderr, "%lf,", csr->std_sc_scaled);
		fprintf(stderr, "%lf,", csr->skew);
		fprintf(stderr, "%lf,", csr->avg_num_neighbours);
		fprintf(stderr, "%lf,", csr->cross_row_similarity);
		fprintf(stderr, "SparseX,");
		fprintf(stderr, "%lf,", time);
		fprintf(stderr, "%lf,", gflops);
		fprintf(stderr, "%lf,", W_avg);
		fprintf(stderr, "%lf\n", J_estimated);
	}
	/******************************************************************************/


	/* Restore original ordering of resulting vector */
	if (enable_reordering) {
		spx_vec_inv_reorder(y, p);
		spx_vec_inv_reorder(x, p);
	}

	/* Check the result */
	if(artificial_flag==0)
		check_result(y, alpha, x, matrix_file);
	// else
	// 	check_result(y, alpha, x, csr->row_ptr, csr->col_ind, csr->values, csr->nr_rows, csr->nr_cols);
	
	/* Cleanup */
	spx_input_destroy(input);
	spx_mat_destroy(A);
	spx_partition_destroy(parts);
	spx_vec_destroy(x);
	spx_vec_destroy(y);
	if(artificial_flag == 1)
		free_csr_matrix(csr);
	return 0;
}
