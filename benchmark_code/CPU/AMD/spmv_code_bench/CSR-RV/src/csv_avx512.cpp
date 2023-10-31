#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <algorithm>
#include <unordered_map>

#include <omp.h>

// #include <immintrin.h>
#include <x86intrin.h>

#define FIELD_LENGTH 128
#define floatType double
#define SPMV_TEST_NUM 1000
#define SPMV_WARMUP_NUM 100
#define CONVERTION_WARMUP_NUM 10
#define MICRO_IN_SEC 1000000.00
#define THREAD_NUM 4

__m512d _m512zero = _mm512_setzero_pd();
__m128i _m128zero = _mm_setzero_si128();
__m256i _m256zero = _mm256_setzero_si256();
__m128i _m128cmp = _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0);
__mmask16 _mmask_load = 0x00ff;

using namespace std;

double microtime()
{
	int tv_sec, tv_usec;
	double time;
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);

	return (tv.tv_sec + tv.tv_usec / MICRO_IN_SEC) * 1000; // ms
}

struct Coordinate
{
	int x;
	int y;
	float val;
};

inline int coordcmp(const void *v1, const void *v2)
{
	struct Coordinate *c1 = (struct Coordinate *)v1;
	struct Coordinate *c2 = (struct Coordinate *)v2;

	if (c1->x != c2->x)
	{
		return (c1->x - c2->x);
	}
	else
	{
		return (c1->y - c2->y);
	}
}

void readMatrix(string filename, floatType **val_ptr, int **cols_ptr,
		int **rowDelimiters_ptr, int *n, int *numRows, int *numCols)
{
	std::string line;
	char id[FIELD_LENGTH];
	char object[FIELD_LENGTH];
	char format[FIELD_LENGTH];
	char field[FIELD_LENGTH];
	char symmetry[FIELD_LENGTH];

	std::ifstream mfs(filename);
	if (!mfs.good())
	{
		std::cerr << "Error: unable to open matrix file " << filename << std::endl;
		exit(1);
	}

	int symmetric = 0;
	int pattern = 0;
	int field_complex = 0;

	int nRows, nCols, nElements;

	struct Coordinate *coords;

	// read matrix header
	if (getline(mfs, line).eof())
	{
		std::cerr << "Error: file " << filename << " does not store a matrix" << std::endl;
		exit(1);
	}

	sscanf(line.c_str(), "%s %s %s %s %s", id, object, format, field, symmetry);

	if (strcmp(object, "matrix") != 0)
	{
		fprintf(stderr, "Error: file %s does not store a matrix\n", filename.c_str());
		exit(1);
	}

	if (strcmp(format, "coordinate") != 0)
	{
		fprintf(stderr, "Error: matrix representation is dense\n");
		exit(1);
	}

	if (strcmp(field, "pattern") == 0)
	{
		pattern = 1;
	}

	if (strcmp(field, "complex") == 0)
	{
		field_complex = 1;
	}

	if (strcmp(symmetry, "symmetric") == 0)
	{
		symmetric = 1;
	}

	while (!getline(mfs, line).eof())
	{
		if (line[0] != '%')
		{
			break;
		}
	}

	// read the matrix size and number of non-zero elements
	sscanf(line.c_str(), "%d %d %d", &nRows, &nCols, &nElements);

	int valSize = nElements * sizeof(struct Coordinate);

	if (symmetric)
	{
		valSize *= 2;
	}

	//    coords = new Coordinate[valSize];
	coords = (struct Coordinate *)malloc(valSize);

	int index = 0;
	float xx99 = 0;
	while (!getline(mfs, line).eof())
	{
		if (pattern)
		{
			sscanf(line.c_str(), "%d %d", &coords[index].x, &coords[index].y);
			coords[index].val = index % 13;
		}
		else if (field_complex)
		{
			// read the value from file
			sscanf(line.c_str(), "%d %d %f %f", &coords[index].x, &coords[index].y,
					&coords[index].val, &xx99);
		}
		else
		{
			// read the value from file
			sscanf(line.c_str(), "%d %d %f", &coords[index].x, &coords[index].y,
					&coords[index].val);
		}

		coords[index].y -= 1;
		coords[index].x -= 1;

		index++;

		// add the mirror element if not on main diagonal
		if (symmetric && coords[index - 1].x != coords[index - 1].y)
		{
			coords[index].x = coords[index - 1].y;
			coords[index].y = coords[index - 1].x;
			coords[index].val = coords[index - 1].val;
			index++;
		}
	}

	nElements = index;

	std::cout << "===========================================================================" << std::endl;
	std::cout << "=========*********  Informations of the sparse matrix   *********==========" << std::endl;
	std::cout << std::endl;
	std::cout << "     Number of Rows is :" << nRows << std::endl;
	std::cout << "  Number of Columns is :" << nCols << std::endl;
	std::cout << " Number of Elements is :" << nElements << std::endl;
	std::cout << std::endl;
	std::cout << "===========================================================================" << std::endl;

	std::cout << "............ Converting the Raw matrix to CSR ................." << std::endl;

	// sort the elements
	qsort(coords, nElements, sizeof(struct Coordinate), coordcmp);

	// create CSR data structures
	*n = nElements;
	*numRows = nRows;
	*numCols = nCols;

	*val_ptr = (floatType *)aligned_alloc(64, sizeof(floatType) * nElements);
	*cols_ptr = (int *)aligned_alloc(64, sizeof(int) * nElements);
	*rowDelimiters_ptr = (int *)aligned_alloc(64, sizeof(int) * (nRows + 1));

	floatType *val = *val_ptr;
	int *cols = *cols_ptr;
	int *rowDelimiters = *rowDelimiters_ptr;

	int row = 1, i = 1;

	rowDelimiters[0] = 0;
	for (i = 1; i < nElements; i++)
	{
		if (coords[i].x != coords[i - 1].x)
		{
			rowDelimiters[row++] = i;
		}
	}
	rowDelimiters[row] = nElements;

	#pragma omp parallel
	{
		long i;
		#pragma omp for schedule(static)
		for (i = 0; i < nElements; i++)
		{
			val[i] = coords[i].val;
			cols[i] = coords[i].y;
		}
	}

	free(coords);
}

// custom multi-thread parallel reduction function for unordered map
#pragma omp declare reduction(merge                                 \
		: std::unordered_map <floatType, int> \
		: omp_out.insert(omp_in.begin(), omp_in.end()))

void csr2csrv(const floatType *h_vals, const int &nItems, void *csv_vals_ptr, floatType *csv_vals,
		int &numVals, int &val_type)
{
	std::unordered_map<floatType, int> value_hashmap;

	int i;
	#pragma omp parallel reduction(merge \
			: value_hashmap)
	{
		long i;
		#pragma omp for schedule(static)
		for (i = 0; i < nItems; i++)
		{
			value_hashmap[h_vals[i]] = 0;
		}
	}

	numVals = value_hashmap.size();

	i = 0;
	for (auto it = value_hashmap.begin(); it != value_hashmap.end(); it++, i++)
	{
		csv_vals[i] = it->first;
		it->second = i;
	}

	// int8
	if (numVals < 256)
	{
		val_type = 0;
		uint8_t *vals_ptr = (uint8_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
	// int16
	else if (numVals < 65536)
	{
		val_type = 1;
		uint16_t *vals_ptr = (uint16_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
	// int32
	else
	{
		val_type = 2;
		int *vals_ptr = (int *)csv_vals_ptr;

		#pragma omp parallel
		{
			long i;
			#pragma omp for schedule(static) nowait
			for (i = 0; i < nItems; i++)
			{
				vals_ptr[i] = value_hashmap[h_vals[i]];
			}
		}
	}
}

//======================================>avx512<=========================================

void spmv(const double *csv_vals, const void *csv_vals_ptr, const int *h_cols, const int *h_rowDelimiters,
		const int &numRows, const int &numVal, const floatType *x, floatType *y)
{
	if (numVal <= 256)
	{
		uint8_t *val_ptr = (uint8_t *)csv_vals_ptr;

		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m128i _val_ptr_uint8;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, row, i;
			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr_uint8 = _mm_loadu_epi8(&val_ptr[i]);

					// _val_ptr_uint8 = _mm_maskz_loadu_epi8(_mmask_load, &val_ptr[i]);

					_val_ptr = _mm256_cvtepu8_epi32(_val_ptr_uint8);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr_uint8 = _mm_maskz_loadu_epi8(_mask, &val_ptr[end]);
				_val_ptr = _mm256_cvtepu8_epi32(_val_ptr_uint8);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
	else if (numVal <= 65536)
	{
		uint16_t *val_ptr = (uint16_t *)csv_vals_ptr;
		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m128i _val_ptr_uint16;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, row, i;

			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr_uint16 = _mm_loadu_epi16(&val_ptr[i]); // 4*32

					_val_ptr = _mm256_cvtepu16_epi32(_val_ptr_uint16);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr_uint16 = _mm_maskz_loadu_epi16(_mask, &val_ptr[end]);
				_val_ptr = _mm256_cvtepu16_epi32(_val_ptr_uint16);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				_y = _mm512_fmadd_pd(_x, _val, _y);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
	else
	{
		int *val_ptr = (int *)csv_vals_ptr;

		#pragma omp parallel
		{
			__m256i _col_ptr, _val_ptr;
			__m512d _x, _y, _val;
			__mmask8 _mask;
			int start, padding_end, end, num, row, i;
			#pragma omp for schedule(static) nowait
			for (row = 0; row < numRows; row++)
			{
				_mm_prefetch((const char *)&y[row], _MM_HINT_T1);

				_y = _mm512_setzero_pd();

				start = h_rowDelimiters[row];
				padding_end = h_rowDelimiters[row + 1];
				end = start + (padding_end - start) / 8 * 8;

				for (i = start; i < end; i += 8)
				{
					_val_ptr = _mm256_loadu_epi32(&val_ptr[i]);

					_val = _mm512_i32gather_pd(_val_ptr, csv_vals, 8);

					_col_ptr = _mm256_loadu_epi32(&h_cols[i]);

					_x = _mm512_i32gather_pd(_col_ptr, x, 8);

					_y = _mm512_fmadd_pd(_x, _val, _y);
				}

				_mask = _mm_cmp_epi16_mask(_m128cmp, _mm_set1_epi16(padding_end - end), 1);

				_val_ptr = _mm256_maskz_loadu_epi32(_mask, &val_ptr[end]);
				_val = _mm512_mask_i32gather_pd(_m512zero, _mask, _val_ptr, csv_vals, 8);

				_col_ptr = _mm256_maskz_loadu_epi32(_mask, &h_cols[end]);
				_x = _mm512_mask_i32gather_pd(_m512zero, _mask, _col_ptr, x, 8);

				_y = _mm512_fmadd_pd(_x, _val, _y);

				y[row] = _mm512_reduce_add_pd(_y);
			}
		}
	}
}

void spmv_csr(floatType *h_val, int *h_cols, int *h_rowDelimiters, int &numRows, floatType *x, floatType *y)
{
	#pragma omp parallel
	{
		int i, row;
		#pragma omp for schedule(static) nowait
		for (row = 0; row < numRows; row++)
		{
			for (i = h_rowDelimiters[row]; i < h_rowDelimiters[row + 1]; i += 1)
			{
				y[row] += h_val[i] * x[h_cols[i]];
			}
		}
	}
}

void test_file(string matrix, double *pre_pro_time, double *time, double *band)
{
	// csr
	floatType *h_val;
	int *h_cols;
	int *h_rowDelimiters;

	// Number of non-zero elements in the matrix
	int nItems;
	int numRows;
	int numCols;

	// csv
	int numVals;
	int val_type;
	floatType *csv_vals;
	void *csv_vals_ptr;

	// x and y
	floatType *x;
	floatType *y;
	floatType *csr_y;

	//====================>get time matire<========================
	readMatrix(matrix, &h_val, &h_cols, &h_rowDelimiters, &nItems, &numRows, &numCols);

	x = (floatType *)aligned_alloc(64, sizeof(floatType) * numCols);

	for (int i = 0; i < numCols; i++)
	{
		x[i] = i % 10;
	}

	y = (floatType *)aligned_alloc(64, sizeof(floatType) * numRows);
	memset(y, 0, sizeof(floatType) * numRows);

	//====================>warm up<========================
	csv_vals_ptr = aligned_alloc(64, sizeof(int) * nItems);
	csv_vals = (floatType *)aligned_alloc(64, sizeof(floatType) * nItems);

	// for (int i = 0; i < CONVERTION_WARMUP_NUM; i++)
	// {
		// csr2csrv(h_val, nItems, csv_vals_ptr, csv_vals, numVals, val_type);
	// }

	//====================>pro-precessing time<========================
	double kk0 = microtime();

	csr2csrv(h_val, nItems, csv_vals_ptr, csv_vals, numVals, val_type);

	// cout << numVals << endl;

	*pre_pro_time = microtime() - kk0;

	cout << "The proprecessing time is " << *pre_pro_time << " ms." << endl;

	//====================>warm up<========================
	// omp_set_num_threads(THREAD_NUM);
	cout << "csv_avx512, threads: " << omp_get_max_threads() << endl;

	for (int i = 0; i < SPMV_WARMUP_NUM; i++)
	{
		spmv(csv_vals, csv_vals_ptr, h_cols, h_rowDelimiters, numRows, numVals, x, y);
	}

	printf("test\n");
	//====================>spmv_kernel<========================
	kk0 = microtime();

	for (int i = 0; i < SPMV_TEST_NUM; i++)
	{
		spmv(csv_vals, csv_vals_ptr, h_cols, h_rowDelimiters, numRows, numVals, x, y);
	}

	*time = (microtime() - kk0) / SPMV_TEST_NUM;

	cout << "The SpMV Time is " << *time << " ms." << endl;

	double gflops = nItems / (*time/1000) * 2 * 1e-9;    // Use csr_nnz to be sure we have the initial nnz (there is no coo for artificial AM).
	cout << "gflops = " << gflops << endl;

	// omp_set_num_threads(48);

	//====================>accuracy<========================
	csr_y = (floatType *)aligned_alloc(64, sizeof(floatType) * numRows);
	memset(csr_y, 0, sizeof(floatType) * numRows);
	spmv_csr(h_val, h_cols, h_rowDelimiters, numRows, x, csr_y);

	int error_count = 0;

	for (int i = 0; i < numRows; i++)
	{
		if (abs(csr_y[i] - y[i]) >= 10e-9)
		{
			error_count++;
		}
	}

	cout << "The spmv error count is: " << error_count << endl;

	//================================================================
	free(h_rowDelimiters);
	free(h_cols);
	free(h_val);
	free(x);
	free(y);
	free(csr_y);
	free(csv_vals_ptr);
	free(csv_vals);
}


int
main(int argc, char **argv)
{
	// omp_set_num_threads(48);

	// string input_file_path = "./in/matrix101.txt";
	// string output_file_path = "./out/matrix101_csv512_prefetch_0425_thread4txt";
	// string matrix_dir_path = "../matrix101/";
	// string start_file = "stormG2_1000.mtx";

	// string input_file_path = "./in/matrix1.txt";
	string output_file_path = "./out.txt";
	// string matrix_dir_path = "../matrix93/";

	// std::ifstream ifs(input_file_path, std::ifstream::in);
	std::ofstream ofs(output_file_path, std::ostream::app);

	double pre_pro_time, time, band;
	// string file_name, file_path;
	string file_path;

	file_path = argv[1];

	// while (file_name != start_file)
	// {
	//     ifs >> file_name;
	// }
	// cout << file_name << endl;
	// file_path = matrix_dir_path + file_name;
	// test_file(file_path, &pre_pro_time, &time, &band);
	// ofs << file_name << " " << pre_pro_time << " "<< time << endl;

	// while (ifs >> file_name)
	// {
		// cout << file_name << endl;
		// file_path = matrix_dir_path + file_name;
		// ofs << file_name << " " << pre_pro_time << " " << time << endl;
	// }
	cout << file_path << endl;
	test_file(file_path, &pre_pro_time, &time, &band);
	ofs << file_path << " " << pre_pro_time << " " << time << endl;

	ofs << endl;

	// test_file("../matrix101/IEEE8500node_A_td_0.mtx", &pre_pro_time, &time, &band);
	// test_file("../matrix101/test1.mtx", &pre_pro_time, &time, &band);
	// test_file("../matrix101/test2.mtx", &pre_pro_time, &time, &band);
	// cout << time << endl;
	// test_file("/home/forrestyan/projects/matrix/pts5ldd27.mtx", &pre_pro_time, &time, &band);
	// ofs << file_name << " ,csv time: " << time << "ms." << endl;
}

// icc csv_avx512.cpp -o csv -O3 -std=c++11 -fopenmp
// srun -p v5_192 -N 1 -n 1 -c 48 csv

