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
	#include <typeinfo>

	#define __auto_type  auto

	// This is more correct (decays to pass by value), but needs C++11.
	// #define typeof(t)  std::decay<decltype(t)>::type
	#define typeof(t)  decltype(t)

	#define static_cast(type, expression)  static_cast<type>(expression)

	#define _Static_assert  static_assert

	// 'restrict' not a cpp keyword.
	#define restrict 

	// C++ in it's infinite wisdom considers NULL of type long int.
	// Include headers that define NULL and pray for the best.
	#include <stddef.h>
	#include <iostream>
	#include <unistd.h>
	// #undef NULL
	// #define NULL  ((void *) 0)

#else

	#define static_cast(type, expression)  ((type) expression)

#endif /* __cplusplus */



/* #undef alloc

#define alloc(ptr, n)                                                      \
do {                                                                       \
	ptr = static_cast(typeof(ptr), malloc((n) * sizeof(*(ptr))));      \
} while(0) */



#endif /* CPP_DEFINES_H */

