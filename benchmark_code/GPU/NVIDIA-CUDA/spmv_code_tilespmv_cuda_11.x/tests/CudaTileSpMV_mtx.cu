#include <iostream>
#include <numeric>
#include <fstream>

#include "gpu_utils.hpp"
#include "spmv_utils.hpp"
#include "cuSPARSE.hpp"
#include "nvem.hpp"

//From cuda 11 - cuSPARSE
#include <cuda_runtime_api.h> // cudaMalloc, cudaMemcpy, etc.
#include <cusparse.h>         // cusparseSpMV
#include <stdio.h>            // printf
#include <stdlib.h>           // EXIT_FAILURE

#include "common.h"
#include "mmio_highlevel.h"
#include "csr2tile.h"
#include "tilespmv_cpu.h"
#include "tilespmv_cuda.h"
#define INDEX_DATA_TYPE unsigned char
#define DEBUG_FORMATCOST 0

#include <utils.h>

using namespace std;

#ifndef VALUE_TYPE_AX
#error
#endif

#ifndef VALUE_TYPE_Y
#error
#endif

#ifndef VALUE_TYPE_COMP
#error
#endif

#ifndef NR_ITER
#error
#endif


#define CHECK_CUDA(func)                                                       \
{                                                                              \
    cudaError_t status = (func);                                               \
    if (status != cudaSuccess) {                                               \
        printf("CUDA API failed at line %d with error: %s (%d)\n",             \
               __LINE__, cudaGetErrorString(status), status);                  \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

#define CHECK_CUSPARSE(func)                                                   \
{                                                                              \
    cusparseStatus_t status = (func);                                          \
    if (status != CUSPARSE_STATUS_SUCCESS) {                                   \
        printf("CUSPARSE API failed at line %d with error: %s (%d)\n",         \
               __LINE__, cusparseGetErrorString(status), status);              \
        return EXIT_FAILURE;                                                   \
    }                                                                          \
}

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

/* Some example here */
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_AX))
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_Y))
#pragma message(VAR_NAME_VALUE(VALUE_TYPE_COMP))

//Add here any supported combinations. CUDA data types I hate you for this. 
cudaDataType CUDA_VALUE_TYPE_AX, CUDA_VALUE_TYPE_Y, CUDA_VALUE_TYPE_COMP;
void cpp_compargs_to_cuda_dtype(){
	if (std::is_same<VALUE_TYPE_AX, int8_t>::value) CUDA_VALUE_TYPE_AX = CUDA_R_8I;
	else if (std::is_same<VALUE_TYPE_AX, int>::value) CUDA_VALUE_TYPE_AX = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_AX, float>::value) CUDA_VALUE_TYPE_AX = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_AX");
	
	if (std::is_same<VALUE_TYPE_Y, int>::value) CUDA_VALUE_TYPE_Y = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_Y, float>::value) CUDA_VALUE_TYPE_Y = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_Y, double>::value) CUDA_VALUE_TYPE_Y = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_Y");
	
	if (std::is_same<VALUE_TYPE_COMP, int>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_32I;
	else if (std::is_same<VALUE_TYPE_COMP, float>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_32F;
	else if (std::is_same<VALUE_TYPE_COMP, double>::value) CUDA_VALUE_TYPE_COMP = CUDA_R_64F;
	else massert(0, "cpp_compargs_to_cuda_dtype: Invalid/not implemented VALUE_TYPE_COMP");
	cout << "CUDA_VALUE_TYPE_AX: " << CUDA_VALUE_TYPE_AX << ", CUDA_VALUE_TYPE_Y: " << CUDA_VALUE_TYPE_Y << ", CUDA_VALUE_TYPE_COMP: " << CUDA_VALUE_TYPE_COMP << endl;
}
	
int main(int argc, char **argv) {
	/// Check Input
	massert(argc == 3,
	  "Incorrect arguments.\nUsage:\t./Executable logfilename Matrix_name.mtx");
	  
	// Set/Check for device
	int device_id = 0;
	cudaSetDevice(device_id);
	cudaGetDevice(&device_id);
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, device_id);
	cout << "Device [" <<  device_id << "] " << deviceProp.name << ", " << " @ " << deviceProp.clockRate * 1e-3f << "MHz. " << endl;

	char *name = argv[2], *outfile = argv[1];
	double cpu_timer, gpu_timer, exc_timer = 0, trans_timer[4] = {0, 0, 0, 0}, gflops_s = -1.0;

	FILE *fp = fopen(name, "r");
	massert(fp && strstr(name, ".mtx") && !fclose(fp), "Invalid .mtx File");

	/// Mix C & C++ file inputs, because...?
	ofstream foutp;
	foutp.open(outfile, ios::out | ios::app ); 
	massert(foutp.is_open() , "Invalid output File");
	// print_devices();

	exc_timer = csecond();
	SpmvOperator op(name);
	
	//int true_nz = op.nz; 
	//op.nz = (op.nz / BLOCK_SIZE) * BLOCK_SIZE;
	exc_timer = csecond() - exc_timer;

	fprintf(stdout,
	  "File=%s ( distribution = %s, placement = %s, seed = %d ) -> Input time=%lf s\n\t\
	  nr_rows(m)=%d, nr_cols(n)=%d, bytes = %d, density =%lf, mem_footprint = %lf MB, mem_range=%s\n\t\
	  nr_nnzs=%d, avg_nnz_per_row=%lf, std_nnz_per_row=%lf\n\t\
	  avg_bw=%lf, std_bw = %lf, avg_bw_scaled = %lf, std_bw_scaled = %lf\n\t\
	  avg_sc=%lf, std_sc=%lf, avg_sc_scaled = %lf, std_sc_scaled = %lf\
	  \n\t, skew =%lf, avg_num_neighbours =%lf, cross_row_similarity =%lf\n",
	  op.mtx_name, op.distribution, op.placement, op.seed, exc_timer, 
	  op.m, op.n, op.bytes, op.density, op.mem_footprint, op.mem_range,
	  op.nz, op.avg_nnz_per_row,  op.std_nnz_per_row, 
	  op.avg_bw,  op.std_bw, op.avg_bw_scaled, op.std_bw_scaled,
	  op.avg_sc,  op.std_sc, op.avg_sc_scaled, op.std_sc_scaled, 
	  op.skew, op.avg_num_neighbours, op.cross_row_similarity);
	  
	VALUE_TYPE_AX *x = (VALUE_TYPE_AX *)malloc(op.n * sizeof(VALUE_TYPE_AX));
	VALUE_TYPE_Y *out = (VALUE_TYPE_Y *)calloc(op.m, sizeof(VALUE_TYPE_Y));
	vec_init_rand<VALUE_TYPE_AX>(x, op.n, 0);
	op.vec_alloc(x);
    
	op.cuSPARSE_init();
	SpmvCsrData *data = (SpmvCsrData *)op.format_data;
    VALUE_TYPE_COMP alpha = (VALUE_TYPE_COMP) 1.0;
    VALUE_TYPE_COMP beta = (VALUE_TYPE_COMP) 0.0;
    cout << "alpha: " << alpha << ", beta: " << beta << endl;

    srand(time(NULL));

    cout << " ( " << op.m << ", " << op.n << " ) nnz = " << op.nz << endl;
    
     int err = 0;
    cudaError_t err_cuda = cudaSuccess;

    // Define pointers of matrix A, vector x and y
    int *d_csrRowPtrA;
    int *d_csrColIdxA;
    VALUE_TYPE_AX *d_csrValA;
    VALUE_TYPE_AX *dX;
    VALUE_TYPE_Y *dY;

    // Matrix A
    CHECK_CUDA(cudaMalloc((void **)&d_csrRowPtrA, (op.m+1) * sizeof(int)));
    CHECK_CUDA(cudaMalloc((void **)&d_csrColIdxA, op.nz  * sizeof(int)));
    CHECK_CUDA(cudaMalloc((void **)&d_csrValA,    op.nz  * sizeof(VALUE_TYPE_AX)));

    CHECK_CUDA(cudaMemcpy(d_csrRowPtrA, data->rowPtr, (op.m+1) * sizeof(int),   cudaMemcpyHostToDevice));
    CHECK_CUDA(cudaMemcpy(d_csrColIdxA, data->colInd, op.nz  * sizeof(int),   cudaMemcpyHostToDevice));
    CHECK_CUDA(cudaMemcpy(d_csrValA,    data->values,    op.nz  * sizeof(VALUE_TYPE_AX),   cudaMemcpyHostToDevice));

    // Vector x
    CHECK_CUDA(cudaMalloc((void **)&dX, op.n * sizeof(VALUE_TYPE_AX)));
    CHECK_CUDA(cudaMemcpy(dX, x, op.n * sizeof(VALUE_TYPE_AX), cudaMemcpyHostToDevice));

    // Vector y
    CHECK_CUDA(cudaMalloc((void **)&dY, op.m  * sizeof(VALUE_TYPE_Y)));
    CHECK_CUDA(cudaMemcpy(dY, out, op.m * sizeof(VALUE_TYPE_Y), cudaMemcpyHostToDevice));

	op.timer = csecond();
    // Get amount of temporary storage needed
    Tile_matrix *matrixA = (Tile_matrix *)malloc(sizeof (Tile_matrix));

    //format conversion

    Tile_create(matrixA, 
                op.m, op.n, op.nz,
                data->rowPtr,
                data->colInd,
                data->values);
	
	int tilenum = matrixA->tilenum;
	int * ptroffset1 = (int *)malloc(sizeof(int) * tilenum);
    int * ptroffset2 = (int *)malloc(sizeof(int) * tilenum);
    memset(ptroffset1, 0, sizeof(int) * tilenum);
    memset(ptroffset2, 0, sizeof(int) * tilenum);

    int rowblkblock = 0;

    unsigned int * blkcoostylerowidx;
    int * blkcoostylerowidx_colstart;
    int * blkcoostylerowidx_colstop;
    
    int tilem = matrixA->tilem;
    int tilen = matrixA->tilen;
    MAT_PTR_TYPE *tile_ptr = matrixA->tile_ptr;
    
        // balance analysis
    int rowblkblock_tmp = 0;
    for (int blki = 0; blki < tilem; blki++)
    {
        int balancenumblk = tile_ptr[blki + 1] - tile_ptr[blki];
        if (balancenumblk <= PREFETCH_SMEM_TH)
            rowblkblock_tmp++;
        else
        {
            rowblkblock_tmp += ceil((double)balancenumblk / (double)PREFETCH_SMEM_TH);
        }
    }
    rowblkblock = rowblkblock_tmp;


    blkcoostylerowidx = (unsigned int *)malloc(sizeof(unsigned int) * rowblkblock);
    unsigned int *blkcoostylerowidx_tmp = blkcoostylerowidx;
    memset(blkcoostylerowidx_tmp, 0, sizeof(unsigned int) * rowblkblock);

    blkcoostylerowidx_colstart = (int *)malloc(sizeof(int) * rowblkblock);
    int *blkcoostylerowidx_colstart_tmp = blkcoostylerowidx_colstart;
    memset(blkcoostylerowidx_colstart_tmp, 0, sizeof(int) * rowblkblock);
    blkcoostylerowidx_colstop = (int *)malloc(sizeof(int) * rowblkblock);
    int *blkcoostylerowidx_colstop_tmp = blkcoostylerowidx_colstop;
    memset(blkcoostylerowidx_colstop_tmp, 0, sizeof(int) * rowblkblock);

    int rowblkblockcnt = 0;
    for (int blki = 0; blki < tilem; blki++)
    {
        int balancenumblk = tile_ptr[blki + 1] - tile_ptr[blki];
        if (balancenumblk <= PREFETCH_SMEM_TH)
        {
            blkcoostylerowidx_tmp[rowblkblockcnt] = blki;
            rowblkblockcnt++;
        }
        else
        {
            int numblklocal = ceil((double)balancenumblk / (double)PREFETCH_SMEM_TH);
            int lenblklocal = ceil((double)balancenumblk / (double)numblklocal);
            for (int iii = 0; iii < numblklocal; iii++)
            {
                blkcoostylerowidx_tmp[rowblkblockcnt] = blki | 0x80000000; // can generate -0
                blkcoostylerowidx_colstart_tmp[rowblkblockcnt] = tile_ptr[blki] + iii * lenblklocal;
                if (iii == numblklocal - 1)
                    blkcoostylerowidx_colstop_tmp[rowblkblockcnt] = tile_ptr[blki] + balancenumblk;
                else
                    blkcoostylerowidx_colstop_tmp[rowblkblockcnt] = tile_ptr[blki] + (iii + 1) * lenblklocal;

                rowblkblockcnt++;
            }
        }
    }
    
    op.timer = csecond() - op.timer;
    cout << "TileSpMV preproc time time cpu = " << op.timer*1000 << " ms." << endl;

	op.timer = csecond();
    int *tile_columnidx = matrixA->tile_columnidx;
    int *tile_nnz = matrixA->tile_nnz;
    char *Format = matrixA->Format;
    int *blknnz = matrixA->blknnz;
    unsigned char *blknnznnz = matrixA->blknnznnz;
    char *tilewidth = matrixA->tilewidth;
    int *csr_offset = matrixA->csr_offset;
    int *csrptr_offset = matrixA->csrptr_offset;
    int *coo_offset = matrixA->coo_offset;
    int *ell_offset = matrixA->ell_offset;
    int *hyb_offset = matrixA->hyb_offset;
    int *hyb_coocount = matrixA->hyb_coocount;
    int *dns_offset = matrixA->dns_offset;
    int *dnsrowptr = matrixA->dnsrowptr;
    int *dnsrow_offset = matrixA->dnsrow_offset;
    int *dnscolptr = matrixA->dnscolptr;
    int *dnscol_offset = matrixA->dnscol_offset;
    int *new_coocount = matrixA->new_coocount;
    VALUE_TYPE_COMP *Blockcsr_Val = matrixA->Blockcsr_Val;
    unsigned char *csr_compressedIdx = matrixA->csr_compressedIdx;
    unsigned char *Blockcsr_Ptr = matrixA->Blockcsr_Ptr;
    VALUE_TYPE_COMP *Blockcoo_Val = matrixA->Blockcoo_Val;
    unsigned char *coo_compressed_Idx = matrixA->coo_compressed_Idx;
    VALUE_TYPE_COMP *Blockell_Val = matrixA->Blockell_Val;
    unsigned char *ell_compressedIdx = matrixA->ell_compressedIdx;
    VALUE_TYPE_COMP *Blockhyb_Val = matrixA->Blockhyb_Val;
    unsigned char *hybIdx = matrixA->hybIdx;
    VALUE_TYPE_COMP *Blockdense_Val = matrixA->Blockdense_Val;
    VALUE_TYPE_COMP *Blockdenserow_Val = matrixA->Blockdenserow_Val;
    char *denserowid = matrixA->denserowid;
    VALUE_TYPE_COMP *Blockdensecol_Val = matrixA->Blockdensecol_Val;
    char *densecolid = matrixA->densecolid;
    int csrsize = matrixA->csrsize;
    int csrptrlen = matrixA->csrptrlen;
    int coosize = matrixA->coosize;
    int ellsize = matrixA->ellsize;
    int hybcoosize = matrixA->hybcoosize;
    int hybellsize = matrixA->hybellsize;
    int dense_size = matrixA->dnssize;
    int denserow_size = matrixA->dnsrowsize;
    int densecol_size = matrixA->dnscolsize;
    int coototal = matrixA->coototal;
    MAT_PTR_TYPE *deferredcoo_ptr = matrixA->deferredcoo_ptr;
    int *deferredcoo_colidx = matrixA->deferredcoo_colidx;
    VALUE_TYPE_COMP *deferredcoo_val = matrixA->deferredcoo_val;

    int csr_csize = csrsize % 2 == 0 ? csrsize / 2 : csrsize / 2 + 1;
    int ell_csize = ellsize % 2 == 0 ? ellsize / 2 : ellsize / 2 + 1;
    int hyb_size = hybellsize % 2 == 0 ? hybellsize / 2 : (hybellsize / 2) + 1;

    // tile matrix

    MAT_PTR_TYPE *d_tile_ptr;
    int *d_tile_columnidx;
    char *d_Format;
    int *d_blknnz;
    unsigned char *d_blknnznnz;

    cudaMalloc((void **)&d_tile_ptr, (tilem + 1) * sizeof(MAT_PTR_TYPE));
    cudaMalloc((void **)&d_tile_columnidx, tilenum * sizeof(int));
    cudaMalloc((void **)&d_Format, tilenum * sizeof(char));
    cudaMalloc((void **)&d_blknnz, (tilenum + 1) * sizeof(int));
    cudaMalloc((void **)&d_blknnznnz, (tilenum + 1) * sizeof(unsigned char));

    cudaMemcpy(d_tile_ptr, tile_ptr, (tilem + 1) * sizeof(MAT_PTR_TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_tile_columnidx, tile_columnidx, tilenum * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Format, Format, tilenum * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_blknnz, blknnz, (tilenum + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_blknnznnz, blknnznnz, (tilenum + 1) * sizeof(unsigned char), cudaMemcpyHostToDevice);
	cudaCheckErrors();
 
    // CSR
    unsigned char *d_csr_compressedIdx = (unsigned char *)malloc((csr_csize) * sizeof(unsigned char));
    VALUE_TYPE_COMP *d_Blockcsr_Val;
    unsigned char *d_Blockcsr_Ptr;

    cudaMalloc((void **)&d_csr_compressedIdx, (csr_csize) * sizeof(unsigned char));
    cudaMalloc((void **)&d_Blockcsr_Val, (csrsize) * sizeof(VALUE_TYPE_COMP));
    cudaMalloc((void **)&d_Blockcsr_Ptr, (csrptrlen) * sizeof(unsigned char));

    cudaMemcpy(d_csr_compressedIdx, csr_compressedIdx, (csr_csize) * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockcsr_Val, Blockcsr_Val, (csrsize) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockcsr_Ptr, Blockcsr_Ptr, (csrptrlen) * sizeof(unsigned char), cudaMemcpyHostToDevice);
	cudaCheckErrors();

    // COO
    unsigned char *d_coo_compressed_Idx;
    VALUE_TYPE_COMP *d_Blockcoo_Val;

    cudaMalloc((void **)&d_coo_compressed_Idx, (coosize) * sizeof(unsigned char));
    cudaMalloc((void **)&d_Blockcoo_Val, (coosize) * sizeof(VALUE_TYPE_COMP));

    cudaMemcpy(d_coo_compressed_Idx, coo_compressed_Idx, (coosize) * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockcoo_Val, Blockcoo_Val, (coosize) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    // ELL
    unsigned char *d_ell_compressedIdx;
    VALUE_TYPE_COMP *d_Blockell_Val;

    cudaMalloc((void **)&d_ell_compressedIdx, (ell_csize) * sizeof(unsigned char));
    cudaMalloc((void **)&d_Blockell_Val, (ellsize) * sizeof(VALUE_TYPE_COMP));

    cudaMemcpy(d_ell_compressedIdx, ell_compressedIdx, (ell_csize) * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockell_Val, Blockell_Val, (ellsize) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
	cudaCheckErrors();

    // HYB
    unsigned char *d_hybIdx;
    char *d_tilewidth;
    VALUE_TYPE_COMP *d_Blockhyb_Val;

    cudaMalloc((void **)&d_hybIdx, (hyb_size + hybcoosize) * sizeof(unsigned char));
    cudaMalloc((void **)&d_tilewidth, tilenum * sizeof(char));
    cudaMalloc((void **)&d_Blockhyb_Val, (hybellsize + hybcoosize) * sizeof(VALUE_TYPE_COMP));

    cudaMemcpy(d_hybIdx, hybIdx, (hyb_size + hybcoosize) * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_tilewidth, tilewidth, tilenum * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockhyb_Val, Blockhyb_Val, (hybellsize + hybcoosize) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    // dense
    VALUE_TYPE_COMP *d_Blockdense_Val;

    cudaMalloc((void **)&d_Blockdense_Val, (dense_size) * sizeof(VALUE_TYPE_COMP));

    cudaMemcpy(d_Blockdense_Val, Blockdense_Val, (dense_size) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    // denserow
    int *d_dnsrowptr;
    VALUE_TYPE_COMP *d_Blockdenserow_Val;
    char *d_denserowid;

    cudaMalloc((void **)&d_dnsrowptr, (tilenum + 1) * sizeof(int));
    cudaMalloc((void **)&d_Blockdenserow_Val, (denserow_size) * sizeof(VALUE_TYPE_COMP));
    cudaMalloc((void **)&d_denserowid, dnsrowptr[tilenum] * sizeof(char));

    cudaMemcpy(d_dnsrowptr, dnsrowptr, (tilenum + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockdenserow_Val, Blockdenserow_Val, (denserow_size) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
    cudaMemcpy(d_denserowid, denserowid, dnsrowptr[tilenum] * sizeof(char), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    // dense column
    int *d_dnscolptr;
    VALUE_TYPE_COMP *d_Blockdensecol_Val;
    char *d_densecolid;

    cudaMalloc((void **)&d_dnscolptr, (tilenum + 1) * sizeof(int));
    cudaMalloc((void **)&d_Blockdensecol_Val, (densecol_size) * sizeof(VALUE_TYPE_COMP));
    cudaMalloc((void **)&d_densecolid, dnscolptr[tilenum] * sizeof(char));

    cudaMemcpy(d_dnscolptr, dnscolptr, (tilenum + 1) * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Blockdensecol_Val, Blockdensecol_Val, (densecol_size) * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);
    cudaMemcpy(d_densecolid, densecolid, dnscolptr[tilenum] * sizeof(char), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    unsigned int *d_blkcoostylerowidx;
    int *d_blkcoostylerowidx_colstart;
    int *d_blkcoostylerowidx_colstop;

    cudaMalloc((void **)&d_blkcoostylerowidx, rowblkblock * sizeof(unsigned int));
    cudaMalloc((void **)&d_blkcoostylerowidx_colstart, rowblkblock * sizeof(int));
    cudaMalloc((void **)&d_blkcoostylerowidx_colstop, rowblkblock * sizeof(int));

    cudaMemcpy(d_blkcoostylerowidx, blkcoostylerowidx, rowblkblock * sizeof(unsigned int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_blkcoostylerowidx_colstart, blkcoostylerowidx_colstart, rowblkblock * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_blkcoostylerowidx_colstop, blkcoostylerowidx_colstop, rowblkblock * sizeof(int), cudaMemcpyHostToDevice);

    int *d_ptroffset1;
    int *d_ptroffset2;

    cudaMalloc((void **)&d_ptroffset1, tilenum * sizeof(int));
    cudaMalloc((void **)&d_ptroffset2, tilenum * sizeof(int));
    cudaMemcpy(d_ptroffset1, ptroffset1, tilenum * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_ptroffset2, ptroffset2, tilenum * sizeof(int), cudaMemcpyHostToDevice);
	cudaCheckErrors();
	
    // deferred coo
    MAT_PTR_TYPE *d_deferredcoo_ptr;
    int *d_deferredcoo_colidx;
    VALUE_TYPE_COMP *d_deferredcoo_val;

    cudaMalloc((void **)&d_deferredcoo_ptr, (op.m + 1) * sizeof(MAT_PTR_TYPE));
    cudaMalloc((void **)&d_deferredcoo_colidx, (coototal) * sizeof(int));
    cudaMalloc((void **)&d_deferredcoo_val, (coototal) * sizeof(VALUE_TYPE_COMP));

    cudaMemcpy(d_deferredcoo_ptr, deferredcoo_ptr, (op.m + 1) * sizeof(MAT_PTR_TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_deferredcoo_colidx, deferredcoo_colidx, coototal * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_deferredcoo_val, deferredcoo_val, coototal * sizeof(VALUE_TYPE_COMP), cudaMemcpyHostToDevice);

    int *d_coodeferoffset;
    int *d_deferbuf_coooff;
    int *d_deferbuf_dxoff;

    cudaMalloc((void **)&d_coodeferoffset, rowblkblock * sizeof(int));
    cudaMemset(d_coodeferoffset, 0, rowblkblock * sizeof(int));

    cudaMalloc((void **)&d_deferbuf_coooff, rowblkblock * PREFETCH_SMEM_TH * COO_NNZ_TH * sizeof(int));
    cudaMemset(d_deferbuf_coooff, 0, rowblkblock * PREFETCH_SMEM_TH * COO_NNZ_TH * sizeof(int));
    cudaMalloc((void **)&d_deferbuf_dxoff, rowblkblock * PREFETCH_SMEM_TH * COO_NNZ_TH * sizeof(int));
    cudaMemset(d_deferbuf_dxoff, 0, rowblkblock * PREFETCH_SMEM_TH * COO_NNZ_TH * sizeof(int));
	cudaCheckErrors();
	
	op.timer = csecond() - op.timer;
    cout << "TileSpMV transfers and weird stuff = " << op.timer*1000 << " ms." << endl;
    
    int num_threads = WARP_PER_BLOCK * WARP_SIZE;
    int num_blocks = ceil((double)rowblkblock / (double)(num_threads / WARP_SIZE));

    stir_spmv_cuda_kernel_v5<<<num_blocks, num_threads>>>(tilem, tilen, op.m, op.n,
                                                          d_tile_ptr, d_tile_columnidx, d_Format, d_blknnz, d_blknnznnz,
                                                          d_csr_compressedIdx, d_Blockcsr_Val, d_Blockcsr_Ptr,
                                                          d_coo_compressed_Idx, d_Blockcoo_Val,
                                                          d_tilewidth, d_ell_compressedIdx, d_Blockell_Val,
                                                          d_hybIdx, d_Blockhyb_Val,
                                                          d_Blockdense_Val,
                                                          d_dnsrowptr, d_Blockdenserow_Val, d_denserowid,
                                                          d_dnscolptr, d_Blockdensecol_Val, d_densecolid,
                                                          d_ptroffset1, d_ptroffset2,
                                                          rowblkblock, d_blkcoostylerowidx, d_blkcoostylerowidx_colstart, d_blkcoostylerowidx_colstop,
                                                          dX, dY, 7, d_coodeferoffset, d_deferbuf_coooff, d_deferbuf_dxoff, 1);
    cudaDeviceSynchronize();       
    cudaCheckErrors();                                               
#ifdef TEST

	VALUE_TYPE_Y *out1 = (VALUE_TYPE_Y *)calloc(op.m, sizeof(VALUE_TYPE_Y));
	fprintf(stdout,"Serial-CSR: ");
	op.timer = csecond();
	spmv_csr<VALUE_TYPE_AX, VALUE_TYPE_Y, VALUE_TYPE_COMP>(data->rowPtr, data->colInd, data->values, x,
		   out1, op.m);
	op.timer = csecond() - op.timer;
	report_results(op.timer * NR_ITER, op.flops, op.bytes);
	fprintf(stdout,"\n");

	fprintf(stdout,"\nRunning tests.. \n");

    cudaMemset(dY, 0,  op.m * sizeof(VALUE_TYPE_COMP));  
	fprintf(stdout,"Testing TileSpMV_11...\t");
    // execute SpMV
    stir_spmv_cuda_kernel_v6<<<num_blocks, num_threads>>>(tilem, tilen, op.m, op.n, op.nz,
                                                          d_tile_ptr, d_tile_columnidx, d_Format, d_blknnz, d_blknnznnz,
                                                          d_csr_compressedIdx, d_Blockcsr_Val, d_Blockcsr_Ptr,
                                                          d_coo_compressed_Idx, d_Blockcoo_Val,
                                                          d_tilewidth, d_ell_compressedIdx, d_Blockell_Val,
                                                          d_hybIdx, d_Blockhyb_Val,
                                                          d_Blockdense_Val,
                                                          d_dnsrowptr, d_Blockdenserow_Val, d_denserowid,
                                                          d_dnscolptr, d_Blockdensecol_Val, d_densecolid,
                                                          d_ptroffset1, d_ptroffset2,
                                                          rowblkblock, d_blkcoostylerowidx, d_blkcoostylerowidx_colstart, d_blkcoostylerowidx_colstop,
                                                          dX, dY, 7, d_coodeferoffset, d_deferbuf_coooff, d_deferbuf_dxoff, 1);

    
	cudaDeviceSynchronize();
    cudaCheckErrors();     
    // device result check
    cudaMemcpy(out, dY, op.m * sizeof(VALUE_TYPE_Y), cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();
	check_result<VALUE_TYPE_Y>((VALUE_TYPE_Y*)out, out1, op.m);
	free(out1);
#endif

    // warm up by running 10000 times
    if (NR_ITER)
    {
        for (int i = 0; i < 10000; i++)
		// execute SpMV
		stir_spmv_cuda_kernel_v6<<<num_blocks, num_threads>>>(tilem, tilen, op.m, op.n, op.nz,
                                                          d_tile_ptr, d_tile_columnidx, d_Format, d_blknnz, d_blknnznnz,
                                                          d_csr_compressedIdx, d_Blockcsr_Val, d_Blockcsr_Ptr,
                                                          d_coo_compressed_Idx, d_Blockcoo_Val,
                                                          d_tilewidth, d_ell_compressedIdx, d_Blockell_Val,
                                                          d_hybIdx, d_Blockhyb_Val,
                                                          d_Blockdense_Val,
                                                          d_dnsrowptr, d_Blockdenserow_Val, d_denserowid,
                                                          d_dnscolptr, d_Blockdensecol_Val, d_densecolid,
                                                          d_ptroffset1, d_ptroffset2,
                                                          rowblkblock, d_blkcoostylerowidx, d_blkcoostylerowidx_colstart, d_blkcoostylerowidx_colstop,
                                                          dX, dY, 7, d_coodeferoffset, d_deferbuf_coooff, d_deferbuf_dxoff, 1);


    }

    err_cuda = cudaDeviceSynchronize();
    cudaCheckErrors();     
    
	short CUDA_VALUE_TYPE_AX;
	if (std::is_same<VALUE_TYPE_AX, float>::value)  CUDA_VALUE_TYPE_AX = 0;
	else if (std::is_same<VALUE_TYPE_AX, double>::value) CUDA_VALUE_TYPE_AX = 1;
	char powa_filename[256];
	sprintf(powa_filename, "TILE_CUDA_SPMV_11_mtx_dtype-%d.log", CUDA_VALUE_TYPE_AX);
	NvemStartMeasure(device_id, powa_filename, 0); // Set to 1 for NVEM log messages. ;
	op.timer = csecond();

    // time spmv by running NR_ITER times
    for (int i = 0; i < NR_ITER; i++){
		stir_spmv_cuda_kernel_v6<<<num_blocks, num_threads>>>(tilem, tilen, op.m, op.n, op.nz,
                                                          d_tile_ptr, d_tile_columnidx, d_Format, d_blknnz, d_blknnznnz,
                                                          d_csr_compressedIdx, d_Blockcsr_Val, d_Blockcsr_Ptr,
                                                          d_coo_compressed_Idx, d_Blockcoo_Val,
                                                          d_tilewidth, d_ell_compressedIdx, d_Blockell_Val,
                                                          d_hybIdx, d_Blockhyb_Val,
                                                          d_Blockdense_Val,
                                                          d_dnsrowptr, d_Blockdenserow_Val, d_denserowid,
                                                          d_dnscolptr, d_Blockdensecol_Val, d_densecolid,
                                                          d_ptroffset1, d_ptroffset2,
                                                          rowblkblock, d_blkcoostylerowidx, d_blkcoostylerowidx_colstart, d_blkcoostylerowidx_colstop,
                                                          dX, dY, 7, d_coodeferoffset, d_deferbuf_coooff, d_deferbuf_dxoff, 1);
    	err_cuda = cudaDeviceSynchronize();
    }
	op.timer = (csecond() - op.timer)/NR_ITER;     
    cudaCheckErrors();     
    unsigned int extra_itter = 0;
	if (op.timer*NR_ITER < 1.0){
		extra_itter = ((unsigned int) 1.0/op.timer) - NR_ITER;
		fprintf(stdout,"Performing extra %d itter for more power measurments (min benchmark time : 1s)...\n", extra_itter);
		for (int i = 0; i <  extra_itter; i++) {
    		stir_spmv_cuda_kernel_v6<<<num_blocks, num_threads>>>(tilem, tilen, op.m, op.n, op.nz,
                                                          d_tile_ptr, d_tile_columnidx, d_Format, d_blknnz, d_blknnznnz,
                                                          d_csr_compressedIdx, d_Blockcsr_Val, d_Blockcsr_Ptr,
                                                          d_coo_compressed_Idx, d_Blockcoo_Val,
                                                          d_tilewidth, d_ell_compressedIdx, d_Blockell_Val,
                                                          d_hybIdx, d_Blockhyb_Val,
                                                          d_Blockdense_Val,
                                                          d_dnsrowptr, d_Blockdenserow_Val, d_denserowid,
                                                          d_dnscolptr, d_Blockdensecol_Val, d_densecolid,
                                                          d_ptroffset1, d_ptroffset2,
                                                          rowblkblock, d_blkcoostylerowidx, d_blkcoostylerowidx_colstart, d_blkcoostylerowidx_colstop,
                                                          dX, dY, 7, d_coodeferoffset, d_deferbuf_coooff, d_deferbuf_dxoff, 1);

    		err_cuda = cudaDeviceSynchronize();
		}
		cudaCheckErrors();
	}
    NvemStats_p nvem_data = NvemStopMeasure(device_id, "Energy measure TileSpMV_11_mtx");
	gflops_s = op.flops*1e-9/op.timer;
	double W_avg = nvem_data->W_avg, J_estimated = nvem_data->J_estimated/(NR_ITER+extra_itter); 
	fprintf(stdout, "TileSpMV_11: t = %lf ms (%lf Gflops/s ). Average Watts = %lf, Estimated Joules = %lf\n", op.timer*1000, gflops_s, W_avg, J_estimated);
	foutp << op.mtx_name << "," << op.distribution << "," << op.placement << "," << op.seed <<
	"," << op.m << "," << op.n << "," << op.nz << "," << op.density << 
	"," << op.mem_footprint << "," << op.mem_range << "," << op.avg_nnz_per_row << "," << op.std_nnz_per_row <<
	"," << op.avg_bw << "," << op.std_bw <<
	"," << op.avg_bw_scaled << "," << op.std_bw_scaled <<
	"," << op.avg_sc << "," << op.std_sc <<
	"," << op.avg_sc_scaled << "," << op.std_sc_scaled <<
	"," << op.skew << "," << op.avg_num_neighbours << "," << op.cross_row_similarity <<
	"," << "TileSpMV_11" <<  "," << op.timer << "," << gflops_s << "," << W_avg <<  "," << J_estimated << endl;

    CHECK_CUDA(cudaFree(d_csrRowPtrA));
    CHECK_CUDA(cudaFree(d_csrColIdxA));
    CHECK_CUDA(cudaFree(d_csrValA));
    CHECK_CUDA(cudaFree(dX));
    CHECK_CUDA(cudaFree(dY));
    foutp.close();

    free(x);
    free(out);


    return 0;
}

