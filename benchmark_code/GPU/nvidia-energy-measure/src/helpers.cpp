///
/// \author Anastasiadis Petros (panastas@cslab.ece.ntua.gr)
/// Many thanks to Kajal Varma for providing the base code for this project (github.com/kajalv/nvml-power)
/// \brief Some CUDA function calls with added error-checking
///

#include "nvem.hpp"
/// For print functions
#include <stdarg.h>

double csecond(void) {
  struct timespec tms;

  if (clock_gettime(CLOCK_REALTIME, &tms)) {
    return (0.0);
  }
  /// seconds, multiplied with 1 million
  int64_t micros = tms.tv_sec * 1000000;
  /// Add full microseconds
  micros += tms.tv_nsec / 1000;
  /// round up if necessary
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  return ((double)micros / 1000000.0);
}


#if !defined(PRINTFLIKE)
#if defined(__GNUC__)
#define PRINTFLIKE(n,m) __attribute__((format(printf,n,m)))
#else
#define PRINTFLIKE(n,m) /* If only */
#endif /* __GNUC__ */
#endif /* PRINTFLIKE */

void _printf(const char *fmt, va_list ap)
{
    if (fmt) vfprintf(stderr, fmt, ap);
    //putc('\n', stderr);
}

void warning(const char *fmt, ...) {
	fprintf(stderr, "WARNING -> ");
	va_list ap;
	va_start(ap, fmt);
	_printf(fmt, ap);
	va_end(ap);
}

void error(const char *fmt, ...) {
	fprintf(stderr, "ERROR ->");
	va_list ap;
	va_start(ap, fmt);
	_printf(fmt, ap);
	va_end(ap);
	exit(1);
}

void massert(bool condi, const char *fmt, ...) {
	if (!condi) {
		va_list ap;
		va_start(ap, fmt);
		_printf(fmt, ap);
		va_end(ap);
		exit(1);
  	}
}
