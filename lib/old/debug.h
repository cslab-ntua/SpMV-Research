#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>            // va_list, va_start(), va_arg(), va_end(), va_copy()
#include <unistd.h>            // getpid()
#include <errno.h>             // errno
#include <sys/syscall.h>       // syscall()
#include <fcntl.h>

/*
 * 'backtrace()' needs linking with flag '-rdynamic' to show function names (GNU linker).
 */
#ifdef DEBUG_BACKTRACE
	#include <execinfo.h>   // backtrace(), backtrace_symbols()
#endif

#include "macros/cpp_defines.h"


/*
 * pid_t gettid(void);
 *     gettid() returns the caller's thread ID (TID).
 *     In a single-threaded process, the thread ID is equal to the process ID (PID, as returned by getpid(2)).
 *     In a multithreaded process, all threads have the same PID, but each one has a unique TID.
 *
 *     Glibc does not provide a wrapper for this system call; call it using syscall(2).
 *     The thread ID returned by this call is not the same thing as a POSIX thread ID (i.e., the opaque value returned by pthread_self(3)).
 *
 *     In a new thread group created by a clone(2) call that does not specify the CLONE_THREAD flag
 *     (or, equivalently, a new process created by fork(2)), the new process is a thread group leader, and
 *     its thread group ID (the value returned by getpid(2)) is the same as its thread ID (the value returned by gettid()).
 *
 * cold attribute
 *     The cold attribute on functions is used to inform the compiler that the function is unlikely to be executed.
 *     
 *     The function is optimized for size rather than speed and on many targets it is placed into a special subsection of
 *     the text section so all cold functions appear close together, improving code locality of non-cold parts of program.
 *     
 *     The paths leading to calls of cold functions within code are marked as unlikely by the branch prediction mechanism.
 *     It is thus useful to mark functions used to handle unlikely conditions, such as perror, as cold to improve
 *     optimization of hot functions that do call marked functions in rare occasions.
 * 
 * We declare the function 'static' (scope is the file it is defined in) to be able to
 * define it in multiple files.
 */


#ifdef DEBUG_BACKTRACE
	static __attribute__((cold)) __attribute__((unused))
	int error_backtrace_print(char *buf, int size)
	{
		int bt_buf_size = 500;
		void *bt_buf[bt_buf_size];
		char **strings;
		int i, ptr_num, n;
		
		ptr_num = backtrace(bt_buf, bt_buf_size);
		
		strings = backtrace_symbols(bt_buf, ptr_num);
		if (strings == NULL) {
			perror("backtrace_symbols");
			exit(EXIT_FAILURE);
		}
		
		n = snprintf(buf, size, "Backtrace:\n");
		for (i=2;i<ptr_num;i++)     // We start from [2] to exclude error_backtrace_print() and display_error().
			n += snprintf(buf + n, size - n, "%s\n", strings[i]);
		n += snprintf(buf + n, size - n, "end\n");
		
		free(strings);
		return n;
	}
#endif /* DEBUG_BACKTRACE */


static __attribute__((cold)) __attribute__((unused))
void display_error(int do_exit, int exit_status, const char *file_name, const char *function_name, int line, const char *format, ...)
{
	va_list ap;
	int buf_size = 10*1024 - 1;     // -1 so that +1 will be a power of 2
	char buf[buf_size + 1];         // to always be a valid string (unused last element, always '\0')
	int errno_buf = errno;
	int n;
	
	va_start(ap, format);
	
	buf[buf_size] = '\0';
	
	n = snprintf(buf, buf_size, "\n\tERROR\n[tid %ld] %s\n => %s(): line [%d]\n", syscall(SYS_gettid), file_name, function_name, line);
	
	#ifdef DEBUG_BACKTRACE
		n += error_backtrace_print(buf + n,  buf_size - n);
	#endif
	
	n += snprintf(buf + n,  buf_size - n,  "      >  ");
	n += vsnprintf(buf + n,  buf_size - n,  format,  ap);
	
	if (errno_buf != 0)
	{
		snprintf(buf + n,  buf_size - n,  "\n      >  errno [%d]",  errno_buf);
		errno = errno_buf;
		buf[buf_size-1] = '\0';
		perror(buf);
	}
	else
	{
		buf[buf_size-1] = '\0';
		fprintf(stderr, "%s\n", buf);
	}
	fflush(stderr);
	
	va_end(ap);
	
	if (do_exit)
		exit(exit_status);
}

#define __display_error(do_exit, exit_status, format, ...) display_error(do_exit, exit_status, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)


#define error_fatal(exit_status, format, ...) __display_error(1, exit_status, format, ##__VA_ARGS__)

#define warning(format, ...) __display_error(0, 0, format, ##__VA_ARGS__)

#define error(format, ...) __display_error(1, EXIT_FAILURE, format, ##__VA_ARGS__)


// POSIX thread functions do not return error numbers in errno, but in the actual return value of the function call instead.
#define error_fatal_pthread(error_code, exit_status, format, ...) do { errno = error_code; error_fatal(exit_status, format, ##__VA_ARGS__); } while (0)
#define warning_pthread(error_code, format, ...) do { errno = error_code; warning(format, ##__VA_ARGS__); } while (0)
#define error_pthread(error_code, format, ...) do { errno = error_code; error(format, ##__VA_ARGS__); } while (0)


#ifdef DEBUG
	#define INTRO(format, ...)                                                                      \
	do {                                                                                            \
		printf("%ld: %s IN " format "\n", syscall(SYS_gettid), __FUNCTION__, ##__VA_ARGS__);    \
		fflush(stdout);                                                                         \
	} while (0)

	#define OUTRO(format, ...)                                                                      \
	do {                                                                                            \
		printf("%ld: %s OUT " format "\n", syscall(SYS_gettid), __FUNCTION__, ##__VA_ARGS__);   \
		fflush(stdout);                                                                         \
	} while (0)
#else
	#define INTRO(format, ...) /* empty */
	#define OUTRO(format, ...) /* empty */
#endif /* DEBUG */


#endif /* DEBUG_H */

