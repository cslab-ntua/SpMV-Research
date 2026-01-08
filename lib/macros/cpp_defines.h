#ifndef CPP_DEFINES_H
#define CPP_DEFINES_H

/*
 * Replace C idioms with equivalent of C++.
 *
 * __cplusplus
 *
 *     This macro is defined when the C++ compiler is in use.
 *     You can use __cplusplus to test whether a header is compiled by a C compiler or a C++ compiler.
 *     This macro is similar to __STDC_VERSION__, in that it expands to a version number.
 *
 *     Depending on the language standard selected, the value of the macro is
 *     199711L for the 1998 C++ standard,
 *     201103L for the 2011 C++ standard,
 *     201402L for the 2014 C++ standard,
 *     201703L for the 2017 C++ standard,
 *     202002L for the 2020 C++ standard,
 *     202302L for the 2023 C++ standard,
 *     or an unspecified value strictly larger than 202302L for the experimental languages enabled by -std=c++26 and -std=gnu++26.
 */

#ifdef __cplusplus

	#include <type_traits>
	#include <typeinfo>
	#include <cstring>

	#define __auto_type  auto

	// This is more correct (decays to pass by value), but needs C++14 (decay is C++11 but decay::type is C++14 ...).
	#if __cplusplus >= 201400L
		// #define typeof(t)  std::decay<decltype(t)>::type
		#define typeof(t)  decltype(({                                                                            \
					typename std::decay<decltype(t)>::type * __typeof_tmp;                            \
					__typeof_tmp = NULL;   /* This way it doesn't give 'uninitialized' warning. */    \
					*__typeof_tmp;                                                                    \
					}))

	#else
		#define typeof(t)  decltype(t)
	#endif

	#define static_cast(type, expression)  static_cast<type>(expression)

	#define _Static_assert  static_assert

	// 'restrict' not a cpp keyword.
	#define restrict 

	// C++ in it's infinite wisdom considers NULL of type long int.
	// Include headers that define NULL and pray for the best.
	#include <stddef.h>
	// #include <iostream>
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

