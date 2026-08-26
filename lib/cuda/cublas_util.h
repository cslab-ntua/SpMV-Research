#ifndef CUBLAS_UTIL_H
#define CUBLAS_UTIL_H

#include "debug.h"

// https://stackoverflow.com/a/14038590
// DEPRECATED gpuCublasErrorCheck. Replace every occurrence with cublas_assert.
#define gpuCublasErrorCheck(ans) { gpuCublasAssert((ans), __FILE__, __LINE__); }
static inline
void
gpuCublasAssert(cublasStatus_t code, const char *file, int line, bool abort=true)
{
	if (code != CUBLAS_STATUS_SUCCESS)
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cublasGetStatusString(code), file, line);
		if (abort)
			exit(code);
	}
}


#define cublas_assert(_code)                                              \
{                                                                         \
	cublasStatus_t __code = _code;                                       \
	if (__code != CUBLAS_STATUS_SUCCESS)                                        \
	{                                                                 \
		error("CUBLAS ERROR: %s\n", cublasGetStatusString(__code));    \
	}                                                                 \
}

#endif /* CUBLAS_UTIL_H */

