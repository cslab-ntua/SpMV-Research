#ifndef CUBLAS_UTIL_H
#define CUBLAS_UTIL_H

// https://stackoverflow.com/a/14038590
#define gpuCublasErrorCheck(ans) { gpuCublasAssert((ans), __FILE__, __LINE__); }
inline void gpuCublasAssert(cublasStatus_t code, const char *file, int line, bool abort=true)
{
	if (code != CUBLAS_STATUS_SUCCESS)
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cublasGetStatusString(code), file, line);
		if (abort) exit(code);
	}
}



#endif /* CUBLAS_UTIL_H */

