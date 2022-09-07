#ifndef CPP_DEFINES_H
#define CPP_DEFINES_H

/*
 * Replace C idioms with equivalent of C++.
 *
 * __cplusplus
 *     This macro is defined when the C++ compiler is in use.
 *     You can use __cplusplus to test whether a header is compiled by a C compiler or a C++ compiler.
 *     This macro is similar to __STDC_VERSION__, in that it expands to a version number. 
 */

#ifdef __cplusplus

	#include <type_traits>

	#define __auto_type  auto
	// #define typeof(t)  std::decay<decltype(t)>::type
	#define typeof(t)  decltype(t)

	#define static_cast(type, expression)  static_cast<type>(expression)

	// 'restrict' not a cpp keyword.
	#define restrict 

#else

	#define static_cast(type, expression)  ((type) expression)

#endif /* __cplusplus */



/* #undef alloc

#define alloc(ptr, n)                                                      \
do {                                                                       \
	ptr = static_cast(typeof(ptr), malloc((n) * sizeof(*(ptr))));      \
} while(0) */



#endif /* CPP_DEFINES_H */

