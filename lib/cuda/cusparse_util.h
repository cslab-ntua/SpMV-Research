#ifndef CUSPARSE_UTIL_H
#define CUSPARSE_UTIL_H

#include "debug.h"

// https://stackoverflow.com/a/14038590
// DEPRECATED gpuCusparseErrorCheck. Replace every occurrence with cusparse_assert.
#define gpuCusparseErrorCheck(ans) { gpuCusparseAssert((ans), __FILE__, __LINE__); }
static inline
void
gpuCusparseAssert(cusparseStatus_t code, const char *file, int line, bool abort=true)
{
	if (code != CUSPARSE_STATUS_SUCCESS)
	{
		fprintf(stderr,"ERROR @ %s %s %d\n", cusparseGetErrorString(code), file, line);
		if (abort)
			exit(code);
	}
}


#define cusparse_assert(_code)                                            \
{                                                                         \
	cusparseStatus_t __code = _code;                                       \
	if (__code != CUSPARSE_STATUS_SUCCESS)                                        \
	{                                                                 \
		error("CUSPARSE ERROR: %s\n", cusparseGetErrorString(__code));    \
	}                                                                 \
}

#endif /* CUSPARSE_UTIL_H */

