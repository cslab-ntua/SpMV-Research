#ifndef CUSPARSE_UTIL_H
#define CUSPARSE_UTIL_H

// https://stackoverflow.com/a/14038590
#define gpuCusparseErrorCheck(ans) { gpuCusparseAssert((ans), __FILE__, __LINE__); }
inline void gpuCusparseAssert(cusparseStatus_t code, const char *file, int line, bool abort=true)
{
	if (code != CUSPARSE_STATUS_SUCCESS)
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cusparseGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}



#endif /* CUSPARSE_UTIL_H */

