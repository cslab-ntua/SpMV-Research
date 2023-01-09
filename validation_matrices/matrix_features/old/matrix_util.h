#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

// #include "macrolib.h"
// #include "genlib.h"

#undef  _TYPE_I
#define _TYPE_I  int


template<typename T>
static
void
matrix_min_max(T * a, long n, double * min, double * max)
{
	int num_threads = omp_get_max_threads();
	T min_t[num_threads], max_t[num_threads];
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		T min_partial, max_partial;
		long i;
		min_partial = a[0];
		max_partial = a[0];
		#pragma omp for schedule(static)
		for (i=0;i<n;i++)
		{
			if (a[i] < min_partial)
				min_partial = a[i];
			if (a[i] > max_partial)
				max_partial = a[i];
		}
		min_t[tnum] = min_partial;
		max_t[tnum] = max_partial;
	}
	*min = min_t[0];
	*max = max_t[0];
	for (i=0;i<num_threads;i++)
	{
		if (min_t[i] < *min)
			*min = min_t[i];
		if (max_t[i] > *max)
			*max = max_t[i];
	}
}


__attribute__((unused))
static
double
matrix_mean(double * a, long n)
{
	int num_threads = omp_get_max_threads();
	double sums[num_threads];
	double sum = 0;
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0;
		#pragma omp for schedule(static)
		for (i=0;i<n;i++)
			t_sum += a[i];
		sums[tnum] = t_sum;
	}
	for (i=0;i<num_threads;i++)
		sum += sums[i];
	return sum / n;
}


__attribute__((unused))
static
double
matrix_var_base(double * a, long n, double mean)
{
	int num_threads = omp_get_max_threads();
	double sums[num_threads];
	double sum = 0;
	long i;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i;
		double t_sum = 0, tmp;
		#pragma omp for schedule(static)
		for (i=0;i<n;i++)
		{
			tmp = a[i] - mean;
			t_sum += tmp * tmp;
		}
		sums[tnum] = t_sum;
	}
	for (i=0;i<num_threads;i++)
		sum += sums[i];
	return sum / n;
}


__attribute__((unused))
static
double
matrix_var(double * a, long n)
{
	double mean = matrix_mean(a, n);
	return matrix_var_base(a, n, mean);
}


__attribute__((unused))
static
double
matrix_std_base(double * a, long n, double mean)
{
	return sqrt(matrix_var_base(a, n, mean));
}


__attribute__((unused))
static
double
matrix_std(double * a, long n)
{
	return matrix_std_base(a, n, matrix_mean(a, n));
}


__attribute__((unused))
static
double *
csr_neighbours_distances_frequencies(int * R_offsets, int * C, long m, long n, long nnz, int ignore_big_rows)
{
	long * frequencies = (typeof(frequencies)) malloc(n * sizeof(*frequencies));
	double * frequencies_percentages = (typeof(frequencies_percentages)) malloc(n * sizeof(*frequencies_percentages));
	double nnz_per_row_avg = (double) nnz / (double) m;
	#pragma omp parallel
	{
		long i, j;
		long degree;
		#pragma omp for schedule(static)
		for (i=0;i<n;i++)
			frequencies[i] = 0;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			degree = R_offsets[i+1] - R_offsets[i];
			if (degree <= 0)
				continue;
			if (ignore_big_rows && (degree > 100 * nnz_per_row_avg))            // Filter out big rows.
			{
				__atomic_fetch_add(&frequencies[0], degree, __ATOMIC_RELAXED);
				continue;
			}
			for (j=R_offsets[i];j<R_offsets[i+1]-1;j++)
			{
				__atomic_fetch_add(&frequencies[C[j+1] - C[j]], 1, __ATOMIC_RELAXED);
			}
			__atomic_fetch_add(&frequencies[0], 1, __ATOMIC_RELAXED);     // Add the last element of the row to the 'no-neighbour' (zero) frequency.
		}
		#pragma omp for schedule(static)
		for (i=0;i<n;i++)
			frequencies_percentages[i] = ((double) 100 * frequencies[i]) / nnz;
	}
	free(frequencies);
	return frequencies_percentages;
}


__attribute__((unused))
static
long
csr_clusters_number(int * R_offsets, int * C, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long max_gap_size)
{
	long total_clusters = 0;
	#pragma omp parallel
	{
		long i, j, k, degree;
		long num_clusters = 0;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			degree = R_offsets[i+1] - R_offsets[i];
			if (degree <= 0)
				continue;
			j = R_offsets[i];
			while (j < R_offsets[i+1])
			{
				k = j + 1;
				while ((k < R_offsets[i+1]) && (C[k] - C[k-1] <= max_gap_size + 1))   // distance 1 means gap 0
					k++;
				num_clusters++;
				j = k;
			}
		}
		__atomic_fetch_add(&total_clusters, num_clusters, __ATOMIC_RELAXED);
	}
	return total_clusters;
}


__attribute__((unused))
static
double
csr_avg_row_neighbours(int * R_offsets, int * C, long m, __attribute__((unused)) long n, long nnz, long window_size)
{
	long total_num_neigh = 0;
	#pragma omp parallel
	{
		long i, j, k;
		long num_neigh = 0;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			for (j=R_offsets[i];j<R_offsets[i+1];j++)
			{
				for (k=j+1;k<R_offsets[i+1];k++)
				{
					if (C[k] - C[j] > window_size)
						break;
					num_neigh += 2;
				}
			}
		}
		__atomic_fetch_add(&total_num_neigh, num_neigh, __ATOMIC_RELAXED);
	}
	return ((double) total_num_neigh) / nnz;
}


static
double
csr_cross_row_similarity(int * R_offsets, int * C, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz, long window_size)
{
	int num_threads = omp_get_max_threads();
	double t_row_similarity[num_threads];
	double total_row_similarity;
	long total_num_non_empty_rows = 0;
	#pragma omp parallel
	{
		int tnum = omp_get_thread_num();
		long i, j, k, k_s, k_e, l;
		long degree, num_similarities, column_diff;
		double row_similarity = 0;
		long num_non_empty_rows = 0;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			degree = R_offsets[i+1] - R_offsets[i];
			if (degree <= 0)
				continue;
			for (l=i+1;l<m;l++)       // Find next non-empty row.
				if (R_offsets[l+1] - R_offsets[l] > 0)
					break;
			num_non_empty_rows++;
			if (l < m)
			{
				k_s = R_offsets[l];
				k_e = R_offsets[l+1];
				num_similarities = 0;
				k = k_s;
				for (j=R_offsets[i];j<R_offsets[i+1];j++)
				{
					while (k < k_e)
					{
						column_diff = C[k] - C[j];
						if (labs(column_diff) <= window_size)
						{
							num_similarities++;
							break;
						}
						if (column_diff <= 0)
							k++;
						else
							break;   // went outside of area to examine
					}
				}
				row_similarity += ((double) num_similarities) / degree;
			}
		}
		__atomic_store(&t_row_similarity[tnum], &row_similarity, __ATOMIC_RELAXED);
		__atomic_fetch_add(&total_num_non_empty_rows, num_non_empty_rows, __ATOMIC_RELAXED);
	}
	if (total_num_non_empty_rows == 0)
		return 0;
	total_row_similarity = 0;
	for (long i=0;i<num_threads;i++)
		total_row_similarity += t_row_similarity[i];
	return total_row_similarity / total_num_non_empty_rows;
}


/* Notes: 
 *     - We assume that the matrix is FULLY sorted.
 */
void
csr_degrees_bandwidths_scatters(_TYPE_I * R_offsets, _TYPE_I * C, long m, __attribute__((unused)) long n, __attribute__((unused)) long nnz,
		double ** degrees_out, double ** bandwidths_out, double ** scatters_out)
{
	double * degrees = (typeof(degrees)) malloc(m * sizeof(*degrees));
	double * bandwidths = (typeof(bandwidths)) malloc(m * sizeof(*bandwidths));
	double * scatters = (typeof(scatters)) malloc(m * sizeof(*scatters));
	#pragma omp parallel
	{
		long i;
		double degree;
		double b, s;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			degree = R_offsets[i+1] - R_offsets[i];
			degrees[i] = degree;
			bandwidths[i] = 0;
			scatters[i] = 0;
			if (degree == 0)
				continue;
			b = C[R_offsets[i+1]-1] - C[R_offsets[i]];
			bandwidths[i] = b;
			s = (b > 0) ? degree / b : 0;
			scatters[i] = s;
		}
	}
	// for (i=0;i<m;i++)
		// printf("%lf\n", degrees[i]);
	*degrees_out = degrees;
	*bandwidths_out = bandwidths;
	*scatters_out = scatters;
}


/* Notes: 
 *     - We assume that the matrix is FULLY sorted.
 *     - 'window_size': Distance from left and right.
 */
double *
csr_row_neighbours(_TYPE_I * R_offsets, _TYPE_I * C, long m, __attribute__((unused)) long n, long nnz, long window_size)
{
	double * neigh_num = (typeof(neigh_num)) malloc(nnz * sizeof(*neigh_num));
	#pragma omp parallel
	{
		long i, j, k;
		#pragma omp for schedule(static)
		for (i=0;i<nnz;i++)
			neigh_num[i] = 0;
		#pragma omp for schedule(static)
		for (i=0;i<m;i++)
		{
			for (j=R_offsets[i];j<R_offsets[i+1];j++)
			{
				for (k=j+1;k<R_offsets[i+1];k++)
				{
					if (C[k] - C[j] > window_size)
						break;
					neigh_num[j]++;
					neigh_num[k]++;
				}
			}
		}
	}
	return neigh_num;
}


double get_degree(void * A, long i)
{
	_TYPE_I * R_offsets = (_TYPE_I *) A;
	return (double) R_offsets[i+1] - R_offsets[i];
}


void
csr_matrix_features(__attribute__((unused)) char * title_base, _TYPE_I * R_offsets, _TYPE_I * C, long m, long n, long nnz)
{
	double * degrees_rows;
	double * bandwidths;
	double * scatters;
	double min_degree, max_degree;
	double mean_nnz_per_row, std_nnz_per_row;
	double min_bw, max_bw;
	double mean_bandwidth, std_bandwidth;
	__attribute__((unused)) double mean_scatter, std_scatter;

	long window_size;
	double * neigh_num;
	double mean_neigh;
	__attribute__((unused)) double std_neigh;
	double avg_cross_row_similarity;
	long max_gap_size;
	long total_clusters;

	csr_degrees_bandwidths_scatters(R_offsets, C, m, n, nnz, &degrees_rows, &bandwidths, &scatters);
	matrix_min_max(degrees_rows, m, &min_degree, &max_degree);
	mean_nnz_per_row = ((double) nnz) / m;
	std_nnz_per_row = matrix_std_base(degrees_rows, m, mean_nnz_per_row);

	matrix_min_max(bandwidths, m, &min_bw, &max_bw);
	mean_bandwidth = matrix_mean(bandwidths, m);
	std_bandwidth = matrix_std_base(bandwidths, m, mean_bandwidth);

	mean_scatter = matrix_mean(scatters, m);
	std_scatter = matrix_std_base(scatters, m, mean_scatter);

	free(degrees_rows);
	free(bandwidths);
	free(scatters);

	// window_size = 64 / sizeof(double) / 2;
	// window_size = 8;
	// window_size = 4;
	window_size = 1;
	// printf("window size = %ld\n", window_size);

	neigh_num = csr_row_neighbours(R_offsets, C, m, n, nnz, window_size);
	mean_neigh = matrix_mean(neigh_num, nnz);
	std_neigh = matrix_std_base(neigh_num, nnz, mean_neigh);
	free(neigh_num);

	avg_cross_row_similarity = csr_cross_row_similarity(R_offsets, C, m, n, nnz, window_size);

	max_gap_size = 0;
	total_clusters = csr_clusters_number(R_offsets, C, m, n, nnz, max_gap_size);

	printf("matrix\tnr_rows\tnr_cols\tnr_nnzs\tdensity\tnnz-r-min\tnnz-r-max\tnnz-r-avg\tnnz-r-std\tskew_coeff\tbw-min\tbw-max\tbw-avg\tbw-std\tneigh-avg\tcross_row_sim-avg\tnum_clusters\tnnz_per_cluster-avg\n");
	printf("%s\t", title_base);
	printf("%ld\t", m);
	printf("%ld\t", n);
	printf("%ld\t", nnz);
	printf("%lf\t", nnz / ((double) m*n));
	printf("%lf\t", min_degree);
	printf("%lf\t", max_degree);
	printf("%lf\t", mean_nnz_per_row);
	printf("%lf\t", std_nnz_per_row);
	printf("%lf\t", (max_degree - mean_nnz_per_row) / mean_nnz_per_row);
	printf("%lf\t", min_bw / n);
	printf("%lf\t", max_bw / n);
	printf("%lf\t", mean_bandwidth / n);
	printf("%lf\t", std_bandwidth / n);
	printf("%lf\t", mean_neigh);
	printf("%lf\t", avg_cross_row_similarity);
	printf("%ld\t", total_clusters);
	printf("%lf\t", ((double) nnz) / total_clusters);
	printf("\n");
}

#endif /* MATRIX_UTIL_H */

