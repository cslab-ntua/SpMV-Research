#include "essexamples.h"
#if HAVE_ESSEX_PHYSICS
#include <essex-physics/matfuncs.h>
#include <essex-physics/cheb_toolbox.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>

static int verbose_flag = 0;
static int rcm_flag = 0;
static int mc_flag  = 0;
static int zoltan_flag = 0;
char *matstr = NULL;
char *matrix_params = NULL;
char *matformatstr = NULL;
static char *mateigrangestr = NULL;
static char *targeteigrangestr = NULL;
static char *spmvmodestr = NULL;
static char *prgname = "a.out";
static int cp_freq = 0;
static char *commgroupsizestr = NULL;
static char *randvecnumstr = NULL;
static char *chebmomentsstr = NULL;
static char *chebmodestr = "m-o-c";
static char *chebdensityfilestr = NULL;
static char *output_filestr = NULL;
static int iterations = 100;
static double gpuweight = 0.;
static double cpuweight = 0.;
static double micweight = 0.;
static int blockvecnum = 1;
static ghost_densemat_storage densemat_storage = GHOST_DENSEMAT_STORAGE_DEFAULT;
static int auto_permute = 0;

int essexamples_process_options(int argc, char **argv)
{
    prgname = argv[0];
    int c, artificial_flag = 0;
    ghost_hwconfig hwconfig = GHOST_HWCONFIG_INITIALIZER;

    while (1)
    {
        static struct option long_options[] =
        {
            {"RCM", no_argument,    &rcm_flag, 1},
            {"MC", no_argument, &mc_flag, 1},
            {"Zoltan", no_argument,    &zoltan_flag, 1},
            {"verbose", no_argument,    0, 'v'},
            {"help",     no_argument,       0, 'h'},
            {"cores",  required_argument, 0, 'c'},
            {"densemat_storage",  required_argument, 0, 'd'},
            {"threadspercore",    required_argument, 0, 't'},
            {"gpu",    required_argument, 0, 'g'},
            {"comm_group_size",    required_argument, 0, 'G'},
            {"matrix",  required_argument, 0, 'm'},
            {"eig_range",  required_argument, 0, 'r'},
            {"target_range",  required_argument, 0, 'T'},
            {"random_vectors",  required_argument, 0, 'R'},
            {"chebyshev_moments",  required_argument, 0, 'M'},
            {"chebyshev_mode",  required_argument, 0, 'C'},
            {"matformat",  required_argument, 0, 'f'},
            {"spmvmode",  required_argument, 0, 's'},
            {"output_file",  required_argument, 0, 'o'},
            {"iterations",  required_argument, 0, 'i'},
            {"weight",  required_argument, 0, 'w'},
            {"cp_freq",  required_argument, 0, 'x'},
            {"blockvecnum",  required_argument, 0, 'b'},
            {"artif_args",  required_argument, 0, 'a'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

       // c = getopt_long (argc, argv, "hvYc:d:f:g:G:m:r:s:t:M:R:C:o:i:w:x:b:X",
       c = getopt_long (argc, argv, "hvYc:d:f:g:G:m:r:s:t:M:R:C:o:i:w:x:b:X:a",
               long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'c':
                hwconfig.ncore = atoi(optarg);
                break;

            case 't':
                hwconfig.nsmt = atoi(optarg);
                break;

            case 'g':
                hwconfig.cudevice = atoi(optarg);
                break;
            
            case 'd':
                if (!strncasecmp(optarg,"row",3)) {
                    densemat_storage = GHOST_DENSEMAT_ROWMAJOR;
                } else if (!strncasecmp(optarg,"col",3)) {
                    densemat_storage = GHOST_DENSEMAT_COLMAJOR;
                } else {
                    printf("Invalid densemat storage order requested!");
                }
                break;


            case 'm':
                matstr = optarg;
                break;

            case 'a':
                matrix_params = optarg;
                artificial_flag = 1;
                break;

            case 'r':
                mateigrangestr = optarg;
                break;

            case 'T':
                targeteigrangestr = optarg;
                break;

            case 'M':
                chebmomentsstr = optarg;
                break;

            case 'R':
                randvecnumstr = optarg;
                break;

            case 'G':
                commgroupsizestr = optarg;
                break;

            case 'C':
                chebmodestr = optarg;
                break;

            case 'f':
                matformatstr = optarg;
                break;

            case 's':
                spmvmodestr = optarg;
                break;

            case 'o':
                output_filestr = optarg;
                break;

            case 'i':
                iterations = atoi(optarg);
                break;

            case 'h':
                essexamples_print_usage(argv[0]);
                exit(EXIT_SUCCESS);

            case 'v':
                verbose_flag = 1;
                break;

            case 'w':
                {
                   char *str = strtok(optarg,":");
                   cpuweight = atof(str);
                   str = strtok(NULL,":");
                   if (str) {
                       gpuweight = atof(str);
                   }
                   str = strtok(NULL,":");
                   if (str) {
                    micweight = atof(str);
                   }
                   break;
                }
            
            case 'x':
                cp_freq = atoi(optarg);
                break;
            
            case 'b':
                blockvecnum = atoi(optarg);
                break;


            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }
    ghost_hwconfig_set(hwconfig);
    return artificial_flag;
}

void essexamples_set_spmv_flags(ghost_spmv_flags *flags)
{
    if (!spmvmodestr) {
        return;
    }

    if (!strcasecmp(spmvmodestr,"task")) {
        *flags |= GHOST_SPMV_MODE_TASK;
    } else if (!strcasecmp(spmvmodestr,"overlap")) {
        *flags |= GHOST_SPMV_MODE_OVERLAP;
    } else if (!strcasecmp(spmvmodestr,"nocomm")) {
        *flags |= GHOST_SPMV_MODE_NOCOMM;
    } else if (!strcasecmp(spmvmodestr,"pipelined")) {
        *flags |= GHOST_SPMV_MODE_PIPELINED;
    }
}

void essexamples_set_auto_permute( int val )
{
    auto_permute = val;
}

void essexamples_get_extremal_eig_range( double * low, double * high )
{
    
    if( (mateigrangestr != NULL) ){
       char *str = strtok(mateigrangestr,",");
       *low = atof(str);
       str = strtok(NULL,",");
       *high = atof(str);
    }
    //*high = atof(mateigrangestr);
    
}

void essexamples_get_target_eig_range( double * low, double * high )
{
    
    if( (targeteigrangestr != NULL) ){
       char *str = strtok(targeteigrangestr,",");
       *low = atof(str);
       str = strtok(NULL,",");
       *high = atof(str);
    }
    //*high = atof(mateigrangestr);
    
}

void essexamples_get_matstr(char **str)
{
  if(matstr != NULL) {
     *str = matstr;
  }
}

#if HAVE_ESSEX_PHYSICS
void essexamples_get_cheb_mode( ChebLoop_Options * opt )
{
    
    *opt = ChebLoop_DEFAULT;
	char * chebmodestrCpy = (char *) malloc( sizeof(char)*256);
	sprintf(chebmodestrCpy, "%s", chebmodestr);
    if( (chebmodestrCpy != NULL) ){
        char *str = strtok(chebmodestrCpy,"-");
        switch (*str)
        {
            case 's':
           	  *opt |= ChebLoop_SINGLEVEC;
              break;
            case 'm':
              break;
            default:
                abort ();
        }
        str = strtok(NULL,"-");
        switch (*str)
        {
            case 'n':
              *opt |= ChebLoop_NAIVE_KERNEL;
              break;
            case 'o':
              break;
            default:
                abort ();
        }
        str = strtok(NULL,"-");
        switch (*str)
        {
            case 's':
              *opt |= ChebLoop_REDUCE_OFTEN;
              break;
            case 'c':
              break;
            default:
                abort ();
        }
    }
}
#endif

void essexamples_get_blockvecnum( int *vecnum )
{
      *vecnum = blockvecnum;
}


void essexamples_get_randvecnum( int * R )
{
    if( (randvecnumstr != NULL) ){
       *R = atof(randvecnumstr);
    }
}

void essexamples_get_chebmoments( int * M )
{
    if( (chebmomentsstr != NULL) ){
       *M = atof(chebmomentsstr);
    }
}

void essexamples_get_cp_freq(int * f)
{
    *f = cp_freq;
}

void essexamples_get_verbose(int * v)
{
    *v = verbose_flag;
}

void essexamples_get_output_file( char ** filename )
{
    if( (output_filestr != NULL) ){
       *filename = output_filestr;
    }
}

void essexamples_create_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits)
{

    if (rcm_flag) {
	    mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_RCM;
    }
    if (zoltan_flag) {
        mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_ZOLTAN;
    }
    if (mc_flag) {
        mtraits-> flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_COLOR; //Multicoloring added
    }
    if (matformatstr) {
        char matformatcpy[strlen(matformatstr)+1];
        strncpy(matformatcpy,matformatstr,strlen(matformatstr));
        matformatcpy[strlen(matformatstr)]='\0';
        char *str = strtok(matformatcpy,"-");
        str = strtok(NULL,"-");
        mtraits->C = atoi(str);
        str = strtok(NULL,"-");
        mtraits->sortScope = atoi(str);

        if (mtraits->sortScope > 1) {
            mtraits->flags |= GHOST_SPARSEMAT_PERMUTE;
        }
    }

    if (!matstr) {
        printf("No matrix specified!\n");
        essexamples_print_usage();
        exit(EXIT_FAILURE);
    }

    double weight = 1.0;
    ghost_type mytype;
    ghost_type_get(&mytype);
    
	ghost_mpi_comm comm_parent = MPI_COMM_NULL;
	ghost_mpi_comm comm        = MPI_COMM_WORLD;
#ifdef GHOST_HAVE_MPI
    if( commgroupsizestr ){
		int group_size = atoi( commgroupsizestr );
		int mpi_rank, mpi_size;
		ghost_nrank( &mpi_size, comm );
		ghost_rank(  &mpi_rank, comm );
		
		if( mpi_size%group_size ) {
			GHOST_WARNING_LOG("comm_group_size do not match with number of processes -- no splitting of the communicator")
		}
		else if( mpi_size/group_size > 1 )
		{
			int color = mpi_rank/group_size;
			
			comm_parent = comm;
			
			MPI_Comm_split( comm_parent, color, mpi_rank, &comm);
			
			int row_rank, row_size;
			MPI_Comm_rank(comm, &row_rank);
			MPI_Comm_size(comm, &row_size);
			GHOST_INFO_LOG("split MPI_COMM_WORLD into %d colos (%4d/%d  -> %2d/%d of color %2d) ", mpi_size/group_size, mpi_rank,mpi_size, row_rank,row_size, color)

		}
	}
#endif

    if (mytype == GHOST_TYPE_CUDA) {
        weight = gpuweight;
    } else if (mytype == GHOST_TYPE_WORK) {
#ifdef GHOST_HAVE_MIC
        weight = micweight;
#else
        weight = cpuweight;
#endif
    }
#if HAVE_ESSEX_PHYSICS
    char matstrcpy[strlen(matstr)+1];
    strncpy(matstrcpy,matstr,strlen(matstr));
    matstrcpy[strlen(matstr)]='\0';
    if (!strncasecmp("spin",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        ghost_gidx DIM;
        ghost_lidx spin_config[3];

        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");

        spin_config[0]= atoi(str); // Lower
        spin_config[1]= spin_config[0]/2; // Upper
        spin_config[2]= 1;  // PBC/OBC

        str = strtok(NULL,"-");
        double disorder = atof(str);
        str = strtok(NULL,"-");
        ghost_lidx disorder_seed = atoi(str);
        
        //info.row_nnz = (2*spin_config[0] +1);
        //info.eig_down = -35.;
        //info.eig_top = 40.;
        matfunc = SpinChainSZ;
        matfunc( -5, &disorder_seed, NULL, &disorder, NULL );
        matfunc( -2, spin_config, &DIM,  NULL, NULL );

        #ifdef GHOST_HAVE_ZOLTAN
        if (auto_permute){
            mtraits->flags |= GHOST_SPARSEMAT_SAVE_ORIG_COLS|GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_ZOLTAN;
        }
        #endif
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = (2*spin_config[0] +1);
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("id",matstrcpy,2)) {
        mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        ghost_gidx DIM = atoi(str);
        
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = diagonal_testmatrix;
        src.maxrowlen = 1;
        src.gnrows = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("zid",matstrcpy,2)) {
        mtraits->datatype = (ghost_datatype)(GHOST_DT_COMPLEX|GHOST_DT_DOUBLE);
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        ghost_gidx DIM = atoi(str);
        
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = zdiagonal_testmatrix;
        src.maxrowlen = 1;
        src.gnrows = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("graphene_dots",matstrcpy,13)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE); 
        ghost_sparsemat_rowfunc matfunc;

        //char *str = strtok(matstrcpy,"-");
        //str = strtok(NULL,"-");

        
        double graphene_dot[4];
        graphene_dot[0] = 38.2;
        graphene_dot[1] = 38.2;
        graphene_dot[2] = 9.55;
        graphene_dot[3] = 0.08546;
        ghost_gidx size_out[2];
        
        crsGraphene( -3, NULL, size_out, graphene_dot, NULL);
        ghost_gidx DIM = size_out[0]*size_out[1];
        //printf("size: %ld x %ld = %ld\n", size_out[0], size_out[1], DIM);
        
        matfunc = crsGraphene;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 13;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("graphene",matstrcpy,8)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        ghost_lidx WL[2];

        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        WL[0] = atoi(str);

        str = strtok(NULL,"-");
        WL[1] = atoi(str);
        ghost_gidx DIM = 0;// = WL[0]*WL[1];

        crsGraphene( -2, WL, &DIM, NULL, NULL);
        //crsGraphene( -1, NULL, NULL, &info);
        matfunc = crsGraphene;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 13;// info.row_nnz;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("topi",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        init_Topi_Sheet( 1, 1, 1);
        
        //crsTopi_set_size(nx,ny,nz);
        set_local_size_Topi_Sheet(nx,ny,nz);

        //mtraits->datatype = crsTopi_get_DT();
        mtraits->datatype = crs_Topi_Sheet_get_DT();

        //ghost_gidx DIM = crsTopi_get_dim();
        ghost_gidx DIM = getDIM_Topi_Sheet();

        //matfunc = crsTopi;
        matfunc = crsTopi_sheet;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        //src.maxrowlen = crsTopi_get_max_nnz();
        src.maxrowlen = crs_Topi_Sheet_get_max_nnz();
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("enh_topi",matstrcpy,8)) {
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int mx = atoi(str);
        str = strtok(NULL,"-");
        int my = atoi(str);
        str = strtok(NULL,"-");
        int mz = atoi(str);

        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        init_Topi_Sheet( mx, my, mz);

        //crsTopi_set_size(nx,ny,nz);
        set_local_size_Topi_Sheet(nx,ny,nz);

        //mtraits->datatype = crsTopi_get_DT();
        mtraits->datatype = crs_Topi_Sheet_get_DT();

        //ghost_gidx_t DIM = crsTopi_get_dim();
        ghost_gidx DIM = getDIM_Topi_Sheet();

        //matfunc = crsTopi;
        matfunc = crsTopi_sheet;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        //src.maxrowlen = crsTopi_get_max_nnz();
        src.maxrowlen = crs_Topi_Sheet_get_max_nnz();
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("hpcg",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        ghost_gidx dim[3] = {nx,ny,nz};
        hpccg_onthefly(-2,NULL,dim,NULL,NULL);
        
        ghost_gidx DIM = nx*ny*nz;

        matfunc = hpccg_onthefly;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 27;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else 
#endif
    if (!strncasecmp(matstr+strlen(matstr)-4,".mtx",4) ||
            !strncasecmp(matstr+strlen(matstr)-3,".mm",3)) {

            ghost_sparsemat_create(mat,ctx,mtraits,1);
            ghost_sparsemat_init_mm((*mat),(char *)matstr, comm, weight);
    } else {
	   
	    ghost_sparsemat_create(mat,ctx,mtraits,1);
      ghost_sparsemat_init_bin(*mat,(char *)matstr, comm, weight);
    }
}

// void essexamples_create_artificial_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, ghost_gidx nr_rows, ghost_gidx nr_cols, ghost_gidx *row_ptr, ghost_gidx *col_ind, double *values)
void essexamples_create_artificial_matrix(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, ghost_gidx nr_rows, ghost_gidx nr_cols, ghost_lidx *row_ptr, ghost_lidx *col_ind, double *values)
{
    if (rcm_flag) {
        mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_RCM;
    }
    if (zoltan_flag) {
        mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_ZOLTAN;
    }
    if (mc_flag) {
        mtraits-> flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_COLOR; //Multicoloring added
    }
    if (matformatstr) {
        char matformatcpy[strlen(matformatstr)+1];
        strncpy(matformatcpy,matformatstr,strlen(matformatstr));
        matformatcpy[strlen(matformatstr)]='\0';
        char *str = strtok(matformatcpy,"-");
        str = strtok(NULL,"-");
        mtraits->C = atoi(str);
        str = strtok(NULL,"-");
        mtraits->sortScope = atoi(str);

        if (mtraits->sortScope > 1) {
            mtraits->flags |= GHOST_SPARSEMAT_PERMUTE;
        }
    }

    mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);

    double weight = 1.0;
    ghost_type mytype;
    ghost_type_get(&mytype);
    
    ghost_mpi_comm comm        = MPI_COMM_WORLD;

    if (mytype == GHOST_TYPE_CUDA) {
        weight = gpuweight;
    } else if (mytype == GHOST_TYPE_WORK) {
#ifdef GHOST_HAVE_MIC
        weight = micweight;
#else
        weight = cpuweight;
#endif
    }

    ghost_sparsemat_create(mat,ctx,mtraits,1);
    ghost_sparsemat_init_crs((*mat), 0, nr_rows, nr_cols, col_ind, values, row_ptr, comm, weight);
}

/*
void essexamples_create_matrix_ft(ghost_sparsemat **mat, ghost_context *ctx, ghost_sparsemat_traits *mtraits, MPI_Comm *ftMpiComm )
{

    if (rcm_flag) {
	    mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_RCM;
    }
    if (zoltan_flag) {
        mtraits->flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_ZOLTAN;
    }
    if (mc_flag) {
        mtraits-> flags |= GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_COLOR; //Multicoloring added
    }
    if (matformatstr) {
        char matformatcpy[strlen(matformatstr)+1];
        strncpy(matformatcpy,matformatstr,strlen(matformatstr));
        matformatcpy[strlen(matformatstr)]='\0';
        char *str = strtok(matformatcpy,"-");
        str = strtok(NULL,"-");
        mtraits->C = atoi(str);
        str = strtok(NULL,"-");
        mtraits->sortScope = atoi(str);

        if (mtraits->sortScope > 1) {
            mtraits->flags |= GHOST_SPARSEMAT_PERMUTE;
        }
    }

    if (!matstr) {
        printf("No matrix specified!\n");
        essexamples_print_usage();
        exit(EXIT_FAILURE);
    }

    double weight = 1.0;
    ghost_type mytype;
    ghost_type_get(&mytype);
    
	ghost_mpi_comm comm_parent = MPI_COMM_NULL;
	ghost_mpi_comm comm        = *ftMpiComm;
#ifdef GHOST_HAVE_MPI
    if( commgroupsizestr ){
		int group_size = atoi( commgroupsizestr );
		int mpi_rank, mpi_size;
		ghost_nrank( &mpi_size, comm );
		ghost_rank(  &mpi_rank, comm );
		
		if( mpi_size%group_size ) {
			GHOST_WARNING_LOG("comm_group_size do not match with number of processes -- no splitting of the communicator")
		}
		else if( mpi_size/group_size > 1 )
		{
			int color = mpi_rank/group_size;
			
			comm_parent = comm;
			
			MPI_Comm_split( comm_parent, color, mpi_rank, &comm);
			
			int row_rank, row_size;
			MPI_Comm_rank(comm, &row_rank);
			MPI_Comm_size(comm, &row_size);
			GHOST_INFO_LOG("split MPI_COMM_WORLD into %d colos (%4d/%d  -> %2d/%d of color %2d) ", mpi_size/group_size, mpi_rank,mpi_size, row_rank,row_size, color)

		}
	}
#endif

    if (mytype == GHOST_TYPE_CUDA) {
        weight = gpuweight;
    } else if (mytype == GHOST_TYPE_WORK) {
#ifdef GHOST_HAVE_MIC
        weight = micweight;
#else
        weight = cpuweight;
#endif
    }
#if HAVE_ESSEX_PHYSICS
    char matstrcpy[strlen(matstr)+1];
    strncpy(matstrcpy,matstr,strlen(matstr));
    matstrcpy[strlen(matstr)]='\0';
    if (!strncasecmp("spin",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        ghost_gidx DIM;
        ghost_lidx spin_config[3];

        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");

        spin_config[0]= atoi(str); // Lower
        spin_config[1]= spin_config[0]/2; // Upper
        spin_config[2]= 1;  // PBC/OBC

        str = strtok(NULL,"-");
        double disorder = atof(str);
        str = strtok(NULL,"-");
        ghost_lidx disorder_seed = atoi(str);
        
        //info.row_nnz = (2*spin_config[0] +1);
        //info.eig_down = -35.;
        //info.eig_top = 40.;
        matfunc = SpinChainSZ;
        matfunc( -5, &disorder_seed, NULL, &disorder, NULL );
        matfunc( -2, spin_config, &DIM,  NULL, NULL );

        #ifdef GHOST_HAVE_ZOLTAN
        if (auto_permute){
            mtraits->flags |= GHOST_SPARSEMAT_SAVE_ORIG_COLS|GHOST_SPARSEMAT_PERMUTE|GHOST_SPARSEMAT_ZOLTAN;
        }
        #endif
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = (2*spin_config[0] +1);
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("id",matstrcpy,2)) {
        mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        ghost_gidx DIM = atoi(str);
        
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = diagonal_testmatrix;
        src.maxrowlen = 1;
        src.gnrows = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("zid",matstrcpy,2)) {
        mtraits->datatype = (ghost_datatype)(GHOST_DT_COMPLEX|GHOST_DT_DOUBLE);
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        ghost_gidx DIM = atoi(str);
        
        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = zdiagonal_testmatrix;
        src.maxrowlen = 1;
        src.gnrows = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("graphene_dots",matstrcpy,13)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE); 
        ghost_sparsemat_rowfunc matfunc;

        //char *str = strtok(matstrcpy,"-");
        //str = strtok(NULL,"-");

        
        double graphene_dot[4];
        graphene_dot[0] = 38.2;
        graphene_dot[1] = 38.2;
        graphene_dot[2] = 9.55;
        graphene_dot[3] = 0.08546;
        ghost_gidx size_out[2];
        
        crsGraphene( -3, NULL, size_out, graphene_dot, NULL);
        ghost_gidx DIM = size_out[0]*size_out[1];
        //printf("size: %ld x %ld = %ld\n", size_out[0], size_out[1], DIM);
        
        matfunc = crsGraphene;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 13;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("graphene",matstrcpy,8)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        ghost_lidx WL[2];

        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        WL[0] = atoi(str);

        str = strtok(NULL,"-");
        WL[1] = atoi(str);
        ghost_gidx DIM = 0;// = WL[0]*WL[1];

        crsGraphene( -2, WL, &DIM, NULL, NULL);
        //crsGraphene( -1, NULL, NULL, &info);
        matfunc = crsGraphene;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 13;// info.row_nnz;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("topi",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        init_Topi_Sheet( 1, 1, 1);
        
        //crsTopi_set_size(nx,ny,nz);
        set_local_size_Topi_Sheet(nx,ny,nz);

        //mtraits->datatype = crsTopi_get_DT();
        mtraits->datatype = crs_Topi_Sheet_get_DT();

        //ghost_gidx DIM = crsTopi_get_dim();
        ghost_gidx DIM = getDIM_Topi_Sheet();

        //matfunc = crsTopi;
        matfunc = crsTopi_sheet;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        //src.maxrowlen = crsTopi_get_max_nnz();
        src.maxrowlen = crs_Topi_Sheet_get_max_nnz();
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("enh_topi",matstrcpy,8)) {
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int mx = atoi(str);
        str = strtok(NULL,"-");
        int my = atoi(str);
        str = strtok(NULL,"-");
        int mz = atoi(str);

        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        init_Topi_Sheet( mx, my, mz);

        //crsTopi_set_size(nx,ny,nz);
        set_local_size_Topi_Sheet(nx,ny,nz);

        //mtraits->datatype = crsTopi_get_DT();
        mtraits->datatype = crs_Topi_Sheet_get_DT();

        //ghost_gidx_t DIM = crsTopi_get_dim();
        ghost_gidx DIM = getDIM_Topi_Sheet();

        //matfunc = crsTopi;
        matfunc = crsTopi_sheet;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        //src.maxrowlen = crsTopi_get_max_nnz();
        src.maxrowlen = crs_Topi_Sheet_get_max_nnz();
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else if (!strncasecmp("hpcg",matstrcpy,4)) {
	   mtraits->datatype = (ghost_datatype)(GHOST_DT_REAL|GHOST_DT_DOUBLE);
        ghost_sparsemat_rowfunc matfunc;
        //matfuncs_info_t info;
        char *str = strtok(matstrcpy,"-");
        str = strtok(NULL,"-");
        int nx = atoi(str);
        str = strtok(NULL,"-");
        int ny = atoi(str);
        str = strtok(NULL,"-");
        int nz = atoi(str);

        ghost_gidx dim[3] = {nx,ny,nz};
        hpccg_onthefly(-2,NULL,dim,NULL,NULL);
        
        ghost_gidx DIM = nx*ny*nz;

        matfunc = hpccg_onthefly;

        ghost_sparsemat_src_rowfunc src = GHOST_SPARSEMAT_SRC_ROWFUNC_INITIALIZER;
        src.func = matfunc;
        src.maxrowlen = 27;
        src.gnrows = DIM;
        src.gncols = DIM;

        ghost_sparsemat_create(mat,ctx,mtraits,1);
        ghost_sparsemat_init_rowfunc(*mat, &src, comm, weight);
    } else 
#endif
    if (!strncasecmp(matstr+strlen(matstr)-4,".mtx",4) ||
            !strncasecmp(matstr+strlen(matstr)-3,".mm",3)) {

            ghost_sparsemat_create(mat,ctx,mtraits,1);
            ghost_sparsemat_init_mm((*mat),(char *)matstr, comm, weight);
    } else {
	   
	    ghost_sparsemat_create(mat,ctx,mtraits,1);
      ghost_sparsemat_init_bin(*mat,(char *)matstr, comm, weight);
    }
}
*/

void essexamples_create_densemat(ghost_densemat **dm, ghost_densemat_traits *traits, ghost_map *map)
{
    if (traits->storage == GHOST_DENSEMAT_STORAGE_DEFAULT) {
        traits->storage = densemat_storage;
    }
    ghost_densemat_create(dm,map,*traits);

}

void essexamples_get_iterations(int *nIter)
{
    *nIter = iterations;
}


void essexamples_print_info(ghost_sparsemat *mat, int printrank)
{
    int rank = 0,nrank = 1;
    if (mat) {
        ghost_rank(&rank, mat->context->mpicomm);
        ghost_nrank(&nrank, mat->context->mpicomm);
    }
    char *matInfoStr = NULL;
    char *ghostInfoStr = NULL;
    char *pumapInfoStr = NULL;
    char *timingSummary = NULL;
    char *datatransfersSummary = NULL;
    char *machineInfoStr = NULL;
    
    ghost_string(&ghostInfoStr);
    if (mat) {
        ghost_sparsemat_info_string(&matInfoStr,mat);
    }
    ghost_pumap_string(&pumapInfoStr);
    ghost_timing_summarystring(&timingSummary);
    ghost_datatransfer_summarystring(&datatransfersSummary);
    ghost_machine_string(&machineInfoStr);

    if (verbose_flag) {
        if (printrank == ESSEXAMPLES_PRINTRANK_ALL || printrank == rank || (printrank == ESSEXAMPLES_PRINTRANK_MIDDLE && rank == nrank/2)) {
            printf("\n%s\n",ghostInfoStr);
            printf("\n%s\n",machineInfoStr);
            printf("\n%s\n",pumapInfoStr);
            if (mat) {
                printf("\n%s\n",matInfoStr);
            }
#ifdef GHOST_INSTR_TIMING
            printf("\n%s\n",timingSummary);
#endif
#ifdef GHOST_TRACK_DATATRANSFERS
            printf("\n%s\n",datatransfersSummary);
#endif
        }
    }
    free(ghostInfoStr);
    free(matInfoStr);
    free(pumapInfoStr);
    free(machineInfoStr);
    free(timingSummary);
    free(datatransfersSummary);
}

void essexamples_print_usage()
{
    printf("Usage: %s [OPTION]...\n",prgname);
    printf("Valid options are:\n");
    printf(" -c, --cores=CORES\t\tNumber of cores to be used\n");
    printf(" -d, --densemat_storage=ORDER\tDensemat storage order, valid values:\n");
    printf("\t\t\t\t  (1) row (row-major, default for densemats with ncols>1)\n");
    printf("\t\t\t\t  (2) col (column-major, default for densemats with ncols=1)\n");
    printf(" -b, --num columns\t\tNumber of columns in a block vector\n");
    printf(" -g, --gpu=GPU\t\t\tGPU to be used for CUDA runs\n");
    printf(" -f, --matformat=FORMAT\t\tSparse matrix storage format\n");
    printf(" -i, --iterations=NITER\t\tNumber of iterations for benchmarking (default: 100)\n");
    printf(" -m, --matrix=MATRIX\t\tSparse matrix to be used, valid values:\n");
    printf("\t\t\t\t  (1) Any Matrix Market file (must end with .mm or .mtx)\n");
    printf("\t\t\t\t  (2) Any binary CRS file\n");
    printf("\t\t\t\t  (3) Spin-<NUp>-<disorder>-<seed>\n");
    printf("\t\t\t\t  (4) Topi-<Nx>-<Ny>-<Nz>\n");
    printf("\t\t\t\t  (5) Graphene-<Nx>-<Ny>\n");
    printf("\t\t\t\t  (5) HPCG-<Nx>-<Ny>-<NZ>\n");
    printf(" -s, --spmvmode=MODE\t\tSparse matrix vector solver, valid values:\n");
    printf("\t\t\t\t  \"Vector\" (blocking communication, followed by computation) (default)\n");
    printf("\t\t\t\t  \"Overlap\" (implicitly overlap communication and computation with non-blocking MPI)\n");
    printf("\t\t\t\t  \"Task\" (explicitly overlap communication and computation with GHOST tasks)\n");
    printf("\t\t\t\t  \"Nocomm\" (no communication of vector data)\n");
    printf("\t\t\t\t  \"Pipelined\" (pipelined block vector communication, EXPERIMENTAL)\n");
    printf(" -r, --eig_range=<lo>,<hi>\tEstimation of extremal eigenvalues  for Chebyshev or Feast Methods\n");
    printf(" -R, --random_vectors=NVECS\tNumber of random vectors used in Density Chebyshev Methods\n");
    printf(" --RCM\t\t\t\tApply RCM permutation to the sparse matrix\n");
    printf(" -M, --chebyshev_moments=NMOMENTS\tNumber of Chebyshev moments used in Chebyshev Methods, or subspace dimension in FEAST method\n");
    printf(" -C, --chebyshev_mode=MODESTRING\tChebyshev method config string: {s,m}-{n,o}-{s,c}\n");
    printf("\t\t\t\t  s,m = single/block vector\n");
    printf("\t\t\t\t  n,o = naive/optimized kernel (optimized kernel uses augmented SpM(M)V\n");
    printf("\t\t\t\t  s,c = reduce dot products in each iteration/once at the end\n");
    printf(" -o, --output_file=FILENAME\tOutput file for computed data\n");
    printf(" -t, --threadspercore=THREADS\tNumber of threads per core\n");
    printf(" -v, --verbose\n");
    printf(" -w, --weights=CPU:GPU:MIC\tWeight of CPU, GPU and MIC processes\n");
    printf(" -x, --cp_freq=FREQUENCY\tCheckpointing frequency\n");
    printf(" --Zoltan\t\t\t\tApply Zoltan partitioning for communication minimization\n");
    printf(" -a, --artif_args=PARAMETERS\tArtificial matrix generation parameters\n");
}
