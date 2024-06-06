#ifndef NVPL_SPARSE_UTIL_H
#define NVPL_SPARSE_UTIL_H

// https://stackoverflow.com/a/14038590
#define gpuNVPLSparseErrorCheck(ans) { gpuNVPLSparseAssert((ans), __FILE__, __LINE__); }
inline void gpuNVPLSparseAssert(nvpl_sparse_status_t code, const char *file, int line, bool abort=true)
{
	if (code != NVPL_SPARSE_STATUS_SUCCESS)
	{
		// fprintf(stderr,"ERROR @ %s %s %d\n", cusparseGetErrorString(code), file, line);
		fprintf(stderr,"ERROR @ %s %d\n", file, line);
		if (abort) exit(code);
	}
}



#endif /* NVPL_SPARSE_UTIL_H */

