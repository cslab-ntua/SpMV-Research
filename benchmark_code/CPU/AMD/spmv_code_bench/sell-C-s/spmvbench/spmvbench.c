#include <ghost.h>
#include <stdio.h>
#include <string.h>
#include "essexamples.h"
#include <omp.h>

#include "artificial_matrix_generation.h"
// #include "monitoring/power/rapl.h"

extern char *matstr;
extern char *matformatstr;
extern char *matrix_params;

char * distribution = NULL;
char * placement = NULL;

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

int artificial_flag;

static void compute(char * matrix_file, ghost_sparsemat *mat, struct csr_matrix * csr, ghost_densemat *x, ghost_densemat *y,
                    ghost_spmv_opts spmvtraits, ghost_densemat_traits vtraits,
                    double time_balance, int loop, int iter)
{
    double start, end;

    // Warm up cpu.
    ghost_timing_wcmilli(&start);
    int num_threads = omp_get_max_threads();
    volatile double warmup_total;
    long A_warmup_n = (1<<20) * num_threads;
    double * A_warmup;
    A_warmup = (double*) malloc(A_warmup_n * sizeof(double));
    _Pragma("omp parallel for")
    for (long i=0;i<A_warmup_n;i++)
        A_warmup[i] = 0;
    for (long j=0;j<16;j++)
    {
        _Pragma("omp parallel for")
        for (long i=1;i<A_warmup_n;i++)
        {
            A_warmup[i] += A_warmup[i-1] * 7 + 3;
        }
    }
    warmup_total = A_warmup[A_warmup_n];
    free(A_warmup);
    ghost_timing_wcmilli(&end);
    double time_A_warmup = end-start;

    // WARM UP ITERATIONS
    ghost_timing_wcmilli(&start);
    ghost_spmv(y,mat,x,spmvtraits);
    ghost_timing_wcmilli(&end);
    double time_warm_up = end-start;

    ghost_timing_wcmilli(&start);
    ghost_spmv(y,mat,x,spmvtraits);
    ghost_timing_wcmilli(&end);
    double time_after_warm_up = end-start;

    ghost_barrier();

    /*****************************************************************************************/
    // struct RAPL_Register * regs;
    // long regs_n;
    // char * reg_ids;

    // reg_ids = NULL;
    // reg_ids = (char *) getenv("RAPL_REGISTERS");

    // rapl_open(reg_ids, &regs, &regs_n);
    /*****************************************************************************************/

    ghost_timing_wcmilli(&start);

    for (size_t i = 0; i < loop; i++) {
        // rapl_read_start(regs, regs_n);

        ghost_spmv(y,mat,x,spmvtraits);

        // rapl_read_end(regs, regs_n);
    }

    ghost_barrier();
    ghost_timing_wcmilli(&end);
    double time = (end-start)/1000.0;

    /*****************************************************************************************/
    double J_estimated = 0;
    // for (int i=0;i<regs_n;i++){
    //     // printf("'%s' total joule = %g\n", regs[i].type, ((double) regs[i].uj_accum) / 1000000);
    //     J_estimated += ((double) regs[i].uj_accum) / 1e6;
    // }
    // rapl_close(regs, regs_n);
    // free(regs);
    double W_avg = J_estimated / time;
    // printf("J_estimated = %lf\tW_avg = %lf\n", J_estimated, W_avg);
    /*****************************************************************************************/

    // double maddflops = 2;    
    // double gflops = (mat->context->gnnz/1.e9*vtraits.ncols*loop*maddflops)/time;
    double gflops = (double) (2*loop*mat->context->gnnz)/((double) (1e9*time));
    double mem_footprint = (SPM_NNZ(mat) * (sizeof(double) + sizeof(int)) + (SPM_NROWS(mat)+1) * sizeof(int))/(1024.0*1024);

    if(iter==1){ // need to output it only at first iteration (of the many "prefetch_distance" iterations)
        essexamples_print_info(mat,0);
        fprintf(stdout,"Preprocessing: %lf secs\n", time_balance);
        fprintf(stdout,"SPMV time: %lf secs\n", time);
        fprintf(stdout,"(matrix_file = %s, format = %s) GFLOPS: %lf\n", matrix_file, matformatstr, gflops);
    }
    if(artificial_flag == 0){
        fprintf(stderr, "%s,", matrix_file);
        fprintf(stderr, "%d,", omp_get_max_threads());
        fprintf(stderr, "%u,", SPM_NROWS(mat));
        fprintf(stderr, "%u,", SPM_NCOLS(mat));
        fprintf(stderr, "%u,", SPM_NNZ(mat));
        fprintf(stderr, "%lf,", time);
        fprintf(stderr, "%lf,", gflops);
        fprintf(stderr, "%lf,", mem_footprint);
        fprintf(stderr, "%lf,", W_avg);
        fprintf(stderr, "%lf,", J_estimated);
        fprintf(stderr, "%s,", matformatstr);
        fprintf(stderr, "%u,", SPM_NROWS(mat));
        fprintf(stderr, "%u,", SPM_NCOLS(mat));
        fprintf(stderr, "%u,", SPM_NNZ(mat));
        fprintf(stderr, "%lf\n", mem_footprint);
        // fprintf(stderr, "%lf,", time_balance);
        // fprintf(stderr, "%lf,", time_warm_up);
        // fprintf(stderr, "%lf\n", time_after_warm_up);
    }
    else{
        fprintf(stdout,"(avg_nnz_per_row = %.4lf, std_nnz_per_row = %.4lf, skew = %.4lf\n)",csr->avg_nnz_per_row, csr->std_nnz_per_row, csr->skew);
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
        fprintf(stderr, "%s,", matformatstr);
        fprintf(stderr, "%lf,", time);
        fprintf(stderr, "%lf,", gflops);
        fprintf(stderr, "%lf,", W_avg);
        fprintf(stderr, "%lf\n", J_estimated);
    }
}

static void *mainTask(void *arg)
{

    ghost_spmv_opts spmvtraits = GHOST_SPMV_OPTS_INITIALIZER;
    int ferr, n, iteration, loop = 128;
    int rank;
    const int printrank = 0;
    double start, end;

    ghost_sparsemat *mat;
    struct csr_matrix * csr;
    ghost_densemat *x, *y, *tmp;
    
    ghost_sparsemat_traits mtraits = GHOST_SPARSEMAT_TRAITS_INITIALIZER;
    ghost_densemat_traits vtraits = GHOST_DENSEMAT_TRAITS_INITIALIZER;

    essexamples_get_randvecnum(&vtraits.ncols);
    essexamples_set_spmv_flags(&spmvtraits.flags);

    long nr_rows, nr_cols, seed;
    double avg_nnz_per_row, std_nnz_per_row, bw, skew, avg_num_neighbours, cross_row_similarity;
    
    // distribution = (char*)malloc(20*sizeof(char));
    // placement = (char*)malloc(20*sizeof(char));

    ghost_timing_wcmilli(&start);

    if(artificial_flag == 1){
        char *matrix_params2 = (char*) malloc(strlen(matrix_params)+1);
        strcpy(matrix_params2, matrix_params);
        removeChar(matrix_params2, '"');

        char *p = strtok (matrix_params2, " ");
        int i=0, nargs=11;

        nr_rows = atoi(p);
        
        p = strtok (NULL, " ");
        nr_cols = atoi(p);
        
        p = strtok (NULL, " ");
        avg_nnz_per_row = strtof(p, NULL);
        
        p = strtok (NULL, " ");
        std_nnz_per_row = strtof(p, NULL);
        
        p = strtok (NULL, " ");
        distribution = (char*)p;
        
        p = strtok (NULL, " ");
        placement = (char*)p;
        
        p = strtok (NULL, " ");
        bw = atof(p);
        
        p = strtok (NULL, " ");
        skew = atof(p);
        
        p = strtok (NULL, " ");
        avg_num_neighbours = strtof(p, NULL);
        
        p = strtok (NULL, " ");
        cross_row_similarity = strtof(p, NULL);
        
        p = strtok (NULL, " ");
        seed = atoi(p);

        p = strtok (NULL, " ");

        csr = artificial_matrix_generation(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, seed, placement, bw, skew, avg_num_neighbours, cross_row_similarity);
        essexamples_create_artificial_matrix(&mat,NULL,&mtraits, csr->nr_rows, csr->nr_cols, csr->row_ptr, csr->col_ind, csr->values);
    }
    else{
        essexamples_create_matrix(&mat,NULL,&mtraits);
    }
    ghost_timing_wcmilli(&end);
    double time_balance = (end-start)/1000.0;

    // essexamples_get_iterations(&loop);

    // we want to compute y = y+Ax
    spmvtraits.flags |= GHOST_SPMV_AXPY;
   
    ghost_rank(&rank, mat->context->mpicomm);

    vtraits.datatype = mat->traits.datatype;
    essexamples_create_densemat(&x,&vtraits,mat->context->col_map);
    essexamples_create_densemat(&y,&vtraits,mat->context->row_map);

    ghost_densemat_init_rand(y); // y = rand
    ghost_densemat_init_rand(x); // x = rand

    ghost_barrier();

    /*****************************************************/

    ghost_timing_wcmilli(&start);
    int prefetch_distance = 1;
    compute(matstr, mat, csr, x, y, spmvtraits, vtraits, time_balance, loop, prefetch_distance);
    ghost_timing_wcmilli(&end);
    double time_p = (end-start)/1000.0; // in seconds

    if (atoi(getenv("COOLDOWN")) == 1)
    {
        printf("time total = %g, sleeping\n", time_p);
        usleep((long) (time_p * 1000000));
    }

    if(artificial_flag==1)
        free_csr_matrix(csr);

    ghost_densemat_destroy(x);
    ghost_densemat_destroy(y);
    ghost_sparsemat_destroy(mat);
    return NULL;
}

int main(int argc, char* argv[])
{
    // Just print the labels and exit.
    artificial_flag = atoi(getenv("USE_ARTIFICIAL_MATRICES"));
    if(argc==1){
        if(artificial_flag){
            fprintf(stderr, "%s,", "matrix_name");
            fprintf(stderr, "%s,", "distribution");
            fprintf(stderr, "%s,", "placement");
            fprintf(stderr, "%s,", "seed");
            fprintf(stderr, "%s,", "nr_rows");
            fprintf(stderr, "%s,", "nr_cols");
            fprintf(stderr, "%s,", "nr_nzeros");
            fprintf(stderr, "%s,", "density");
            fprintf(stderr, "%s,", "mem_footprint");
            fprintf(stderr, "%s,", "mem_range");
            fprintf(stderr, "%s,", "avg_nnz_per_row");
            fprintf(stderr, "%s,", "std_nnz_per_row");
            fprintf(stderr, "%s,", "avg_bw");
            fprintf(stderr, "%s,", "std_bw");
            fprintf(stderr, "%s,", "avg_bw_scaled");
            fprintf(stderr, "%s,", "std_bw_scaled");
            fprintf(stderr, "%s,", "avg_sc");
            fprintf(stderr, "%s,", "std_sc");
            fprintf(stderr, "%s,", "avg_sc_scaled");
            fprintf(stderr, "%s,", "std_sc_scaled");
            fprintf(stderr, "%s,", "skew");
            fprintf(stderr, "%s,", "avg_num_neighbours");
            fprintf(stderr, "%s,", "cross_row_similarity");
            fprintf(stderr, "%s,", "format_name");
            fprintf(stderr, "%s,", "time");
            fprintf(stderr, "%s,", "gflops");
            fprintf(stderr, "%s,", "W_avg");
            fprintf(stderr, "%s", "J_estimated");
            fprintf(stderr, "\n");
        }
        else{
            fprintf(stderr,"%s,", "matrix_name");
            fprintf(stderr,"%s,", "num_threads");
            fprintf(stderr,"%s,", "csr_m");
            fprintf(stderr,"%s,", "csr_n");
            fprintf(stderr,"%s,", "csr_nnz");
            fprintf(stderr,"%s,", "time");
            fprintf(stderr,"%s,", "gflops");
            fprintf(stderr,"%s,", "csr_mem_footprint");
            fprintf(stderr,"%s,", "W_avg");
            fprintf(stderr,"%s,", "J_estimated");
            fprintf(stderr,"%s,", "format_name");
            fprintf(stderr,"%s,", "m");
            fprintf(stderr,"%s,", "n");
            fprintf(stderr,"%s,", "nnz");
            fprintf(stderr,"%s,", "mem_footprint");
            fprintf(stderr, "\n");
        }
        return 1;
    }
    artificial_flag = essexamples_process_options(argc,argv);

    ghost_init(argc,argv); // has to be the first call
    ghost_task *t;
    // ghost_task_create(&t,GHOST_TASK_FILL_ALL,0,&mainTask,NULL,GHOST_TASK_DEFAULT, NULL, 0);
    ghost_task_create(&t,GHOST_TASK_FILL_ALL,0,&mainTask,matstr,GHOST_TASK_DEFAULT, NULL, 0);
    ghost_task_enqueue(t);
    ghost_task_wait(t);
    ghost_task_destroy(t);

    ghost_finalize();
    
    return EXIT_SUCCESS;
}
