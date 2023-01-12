#ifndef MACROLIB_H
#define MACROLIB_H

#include "macros/cpp_defines.h"
#include "macros/count_macro_arguments.h"
#include "macros/recursion.h"
#include "debug.h"


// Intermediate step to expand arguments before calling "fun" with them (e.g. for concatenation ##).
#define EXPAND_ARGS_AND_CALL(fun, ...)  fun(__VA_ARGS__)


#define _ID(...)  __VA_ARGS__
#define _SINK(...)


#define _STRING(...)  #__VA_ARGS__
#define STRING(...)  _STRING(__VA_ARGS__)


#define _CONCAT(a, b)  a ## b
#define CONCAT(a, b)  _CONCAT(a, b)


// This is usefull for the _Pragma operator, which expects a single string literal, when we want to parameterize the string.
// We pass the argument unstringized, like in a #pragma directive.
#define PRAGMA(...)  _Pragma(STRING(__VA_ARGS__))


// __VA_OPT__ replacement (almost, can't use the comma ',' with it).
#define _OPT_ID(...)  __VA_ARGS__
#define _OPT_SINK(...)
#define _OPT_EXPAND(optional, fun, ...)  fun(optional)
#define OPT(optional, ...)      _OPT_EXPAND(optional, _OPT_ID  , ##__VA_ARGS__ (_OPT_SINK))
#define OPT_NEG(optional, ...)  _OPT_EXPAND(optional, _OPT_SINK, ##__VA_ARGS__ ()_OPT_ID)


//==========================================================================================================================================
//= Default Argument Values
//==========================================================================================================================================


// Default function argument mechanism.
#define _DEF_ARG_1(def, arg)  arg
#define _DEF_ARG_0(def, arg)  def
#define _DEF_ARG(num, def, arg)  _DEF_ARG_ ## num (def, arg)
#define _DEF_ARG_EXPAND(num, def, arg)  _DEF_ARG(num, def, arg)
#define _DEFAULT_ARG_TEST_EMPTY(def, arg)  _DEF_ARG_EXPAND(COUNT_ARGS(arg), def, arg)

#define _DEFAULT_ARG_5(def, a1, a2, a3, a4, a5, ...)  _DEFAULT_ARG_TEST_EMPTY(def, a5)
#define DEFAULT_ARG_5(def, ...)  _DEFAULT_ARG_5(def, __VA_ARGS__, def, def, def, def)

#define _DEFAULT_ARG_4(def, a1, a2, a3, a4, ...)  _DEFAULT_ARG_TEST_EMPTY(def, a4)
#define DEFAULT_ARG_4(def, ...)  _DEFAULT_ARG_4(def, __VA_ARGS__, def, def, def)

#define _DEFAULT_ARG_3(def, a1, a2, a3, ...)  _DEFAULT_ARG_TEST_EMPTY(def, a3)
#define DEFAULT_ARG_3(def, ...)  _DEFAULT_ARG_3(def, __VA_ARGS__, def, def)

#define _DEFAULT_ARG_2(def, a1, a2, ...)  _DEFAULT_ARG_TEST_EMPTY(def, a2)
#define DEFAULT_ARG_2(def, ...)  _DEFAULT_ARG_2(def, __VA_ARGS__, def)

#define _DEFAULT_ARG_1(def, a1, ...)  _DEFAULT_ARG_TEST_EMPTY(def, a1)
#define DEFAULT_ARG_1(def, ...)  _DEFAULT_ARG_1(def, __VA_ARGS__, def)


//==========================================================================================================================================
//= Tupples Packing / Unpacking
//==========================================================================================================================================


// Pack args in a tuple. 
#define PACK(...)  (__VA_ARGS__)

// Unpack tuple as arg list, do nothing when not a tuple.
#define _UNPACK_ID(...)  __VA_ARGS__
#define _UNPACK_TEST_IF_TUPLE(...)  /* one */, /* two */
// If there is a third argument (i.e. if _UNPACK_TEST_IF_TUPLE was called), args are unpacked with _UNPACK_ID.
// Else they are returned as is (args where not packed - not a tuple).
#define _UNPACK(args, one, ...)  OPT(_UNPACK_ID, ##__VA_ARGS__) args
// Always passed as only 2 arguments, so we pass them again.
#define _UNPACK_EXPAND_ARGS(...)  _UNPACK(__VA_ARGS__)
// When 'tuple' is a tuple '_UNPACK_TEST_IF_TUPLE' is called and it returns two arguments.
// When 'tuple' is not a tuple it is not called, and the two are counted as one argument (no commas).
#define UNPACK(tuple)  _UNPACK_EXPAND_ARGS(tuple, _UNPACK_TEST_IF_TUPLE tuple)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Macro Functions                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Foreach Macro Argument
//==========================================================================================================================================


#define _FOREACH_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _FOREACH_0(i, resulting_code, fun)            UNPACK(resulting_code)
#define _FOREACH_REC(i, resulting_code, fun, a, ...)  (UNPACK(resulting_code) _FOREACH_EXPAND(fun, UNPACK(a));), fun, ##__VA_ARGS__

#define FOREACH_UNGUARDED(fun, ...)  RECURSION_FORM_ITER(__VA_ARGS__)(_FOREACH_REC, _FOREACH_0, (, fun, __VA_ARGS__))
#define FOREACH(fun, ...)            do { FOREACH_UNGUARDED(fun, ##__VA_ARGS__); } while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Foreach Macro Argument With Iterator
//------------------------------------------------------------------------------------------------------------------------------------------


#define _FOREACH_ITER_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _FOREACH_ITER_0(i, resulting_code, fun)            UNPACK(resulting_code)
#define _FOREACH_ITER_REC(i, resulting_code, fun, a, ...)  (UNPACK(resulting_code) _FOREACH_ITER_EXPAND(fun, i, UNPACK(a));), fun, ##__VA_ARGS__

#define FOREACH_ITER_UNGUARDED(fun, ...)  RECURSION_FORM_ITER(__VA_ARGS__)(_FOREACH_ITER_REC, _FOREACH_ITER_0, (, fun, __VA_ARGS__))
#define FOREACH_ITER(fun, ...)            do { FOREACH_ITER_UNGUARDED(fun, ##__VA_ARGS__); } while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Foreach Macro Argument In Tupple With Extra Static Prefix/Suffix Arguments
//------------------------------------------------------------------------------------------------------------------------------------------


#define _FOREACH_PACKED_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _FOREACH_PACKED_0_0_0(i, resulting_code, fun)                            UNPACK(resulting_code)
#define _FOREACH_PACKED_0_0_REC(i, resulting_code, fun, a, ...)                  (UNPACK(resulting_code) _FOREACH_PACKED_EXPAND(fun, UNPACK(a));), fun, ##__VA_ARGS__
#define _FOREACH_PACKED_UNGUARDED_0_0(fun, prefix, arg, suffix)               RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_0_0_REC, _FOREACH_PACKED_0_0_0, (, fun, UNPACK(arg)))

#define _FOREACH_PACKED_0_1_0(i, resulting_code, fun, suffix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_0_1_REC(i, resulting_code, fun, suffix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_EXPAND(fun, UNPACK(a), UNPACK(suffix));), fun, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_UNGUARDED_0_1(fun, prefix, arg, suffix)               RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_0_1_REC, _FOREACH_PACKED_0_1_0, (, fun, suffix, UNPACK(arg)))

#define _FOREACH_PACKED_1_0_0(i, resulting_code, fun, prefix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_1_0_REC(i, resulting_code, fun, prefix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_EXPAND(fun, UNPACK(prefix), UNPACK(a));), fun, prefix, ##__VA_ARGS__
#define _FOREACH_PACKED_UNGUARDED_1_0(fun, prefix, arg, suffix)               RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_1_0_REC, _FOREACH_PACKED_1_0_0, (, fun, prefix, UNPACK(arg)))

#define _FOREACH_PACKED_1_1_0(i, resulting_code, fun, prefix, suffix)            UNPACK(resulting_code)
#define _FOREACH_PACKED_1_1_REC(i, resulting_code, fun, prefix, suffix, a, ...)  (UNPACK(resulting_code) _FOREACH_PACKED_EXPAND(fun, UNPACK(prefix), UNPACK(a), UNPACK(suffix));), fun, prefix, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_UNGUARDED_1_1(fun, prefix, arg, suffix)               RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_1_1_REC, _FOREACH_PACKED_1_1_0, (, fun, prefix, suffix, UNPACK(arg)))

#define __FOREACH_PACKED_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)  _FOREACH_PACKED_UNGUARDED_ ## prefix_num ## _ ## suffix_num(fun, prefix, arg, suffix)  
#define _FOREACH_PACKED_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)   __FOREACH_PACKED_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)

#define FOREACH_PACKED_UNGUARDED(fun, prefix, arg, suffix)  _FOREACH_PACKED_UNGUARDED(COUNT_ARGS(prefix), COUNT_ARGS(suffix), fun, prefix, arg, suffix)
#define FOREACH_PACKED(fun, prefix, arg, suffix)            do { FOREACH_PACKED_UNGUARDED(fun, prefix, arg, suffix); } while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Foreach Macro Argument In Tupple With Extra Static Prefix/Suffix Arguments And Iterator
//------------------------------------------------------------------------------------------------------------------------------------------


#define _FOREACH_PACKED_ITER_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _FOREACH_PACKED_ITER_0_0_0(i, resulting_code, fun)                            UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_0_0_REC(i, resulting_code, fun, a, ...)                  (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPAND(fun, i, UNPACK(a));), fun, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_UNGUARDED_0_0(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_0_0_REC, _FOREACH_PACKED_ITER_0_0_0, (, fun, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_0_1_0(i, resulting_code, fun, suffix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_0_1_REC(i, resulting_code, fun, suffix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPAND(fun, i, UNPACK(a), UNPACK(suffix));), fun, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_UNGUARDED_0_1(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_0_1_REC, _FOREACH_PACKED_ITER_0_1_0, (, fun, suffix, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_1_0_0(i, resulting_code, fun, prefix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_1_0_REC(i, resulting_code, fun, prefix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPAND(fun, i, UNPACK(prefix), UNPACK(a));), fun, prefix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_UNGUARDED_1_0(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_1_0_REC, _FOREACH_PACKED_ITER_1_0_0, (, fun, prefix, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_1_1_0(i, resulting_code, fun, prefix, suffix)            UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_1_1_REC(i, resulting_code, fun, prefix, suffix, a, ...)  (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPAND(fun, i, UNPACK(prefix), UNPACK(a), UNPACK(suffix));), fun, prefix, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_UNGUARDED_1_1(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_1_1_REC, _FOREACH_PACKED_ITER_1_1_0, (, fun, prefix, suffix, UNPACK(arg)))

#define __FOREACH_PACKED_ITER_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)  _FOREACH_PACKED_ITER_UNGUARDED_ ## prefix_num ## _ ## suffix_num(fun, prefix, arg, suffix)  
#define _FOREACH_PACKED_ITER_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)   __FOREACH_PACKED_ITER_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)

#define FOREACH_PACKED_ITER_UNGUARDED(fun, prefix, arg, suffix)  _FOREACH_PACKED_ITER_UNGUARDED(COUNT_ARGS(prefix), COUNT_ARGS(suffix), fun, prefix, arg, suffix)
#define FOREACH_PACKED_ITER(fun, prefix, arg, suffix)            do { FOREACH_PACKED_ITER_UNGUARDED(fun, prefix, arg, suffix); } while (0)


//------------------------------------------------------------------------------------------------------------------------------------------
//- Expression Foreach Macro Argument In Tupple With Extra Static Prefix/Suffix Arguments And Iterator
//------------------------------------------------------------------------------------------------------------------------------------------


#define _FOREACH_PACKED_ITER_EXPRESSION_EXPAND(fun, ...)  fun(__VA_ARGS__)

#define _FOREACH_PACKED_ITER_EXPRESSION_0_0_0(i, resulting_code, fun)                            UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_EXPRESSION_0_0_REC(i, resulting_code, fun, a, ...)                  (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPRESSION_EXPAND(fun, i, UNPACK(a))), fun, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED_0_0(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_EXPRESSION_0_0_REC, _FOREACH_PACKED_ITER_EXPRESSION_0_0_0, (, fun, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_EXPRESSION_0_1_0(i, resulting_code, fun, suffix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_EXPRESSION_0_1_REC(i, resulting_code, fun, suffix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPRESSION_EXPAND(fun, i, UNPACK(a), UNPACK(suffix))), fun, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED_0_1(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_EXPRESSION_0_1_REC, _FOREACH_PACKED_ITER_EXPRESSION_0_1_0, (, fun, suffix, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_EXPRESSION_1_0_0(i, resulting_code, fun, prefix)                    UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_EXPRESSION_1_0_REC(i, resulting_code, fun, prefix, a, ...)          (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPRESSION_EXPAND(fun, i, UNPACK(prefix), UNPACK(a))), fun, prefix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED_1_0(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_EXPRESSION_1_0_REC, _FOREACH_PACKED_ITER_EXPRESSION_1_0_0, (, fun, prefix, UNPACK(arg)))

#define _FOREACH_PACKED_ITER_EXPRESSION_1_1_0(i, resulting_code, fun, prefix, suffix)            UNPACK(resulting_code)
#define _FOREACH_PACKED_ITER_EXPRESSION_1_1_REC(i, resulting_code, fun, prefix, suffix, a, ...)  (UNPACK(resulting_code) _FOREACH_PACKED_ITER_EXPRESSION_EXPAND(fun, i, UNPACK(prefix), UNPACK(a), UNPACK(suffix))), fun, prefix, suffix, ##__VA_ARGS__
#define _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED_1_1(fun, prefix, arg, suffix)                  RECURSION_FORM_ITER(UNPACK(arg))(_FOREACH_PACKED_ITER_EXPRESSION_1_1_REC, _FOREACH_PACKED_ITER_EXPRESSION_1_1_0, (, fun, prefix, suffix, UNPACK(arg)))

#define __FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)  _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED_ ## prefix_num ## _ ## suffix_num(fun, prefix, arg, suffix)  
#define _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)   __FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(prefix_num, suffix_num, fun, prefix, arg, suffix)

#define FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(fun, prefix, arg, suffix)  _FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(COUNT_ARGS(prefix), COUNT_ARGS(suffix), fun, prefix, arg, suffix)
#define FOREACH_PACKED_ITER_EXPRESSION(fun, prefix, arg, suffix)            ({ FOREACH_PACKED_ITER_EXPRESSION_UNGUARDED(fun, prefix, arg, suffix) })


//==========================================================================================================================================
//= Rename Macro Arguments
//==========================================================================================================================================


/* RENAME( list of tupples : (name_initial, name_final, [type]) )
 *     Rename macro arguments to avoid names clashing with macro variables.
 *     Optionally pass the type.
 */

#define __RENAME_PREFIX __eb320f0c2b6a25b48ca861a120eea902__     // MD5 of "macro" (no reason)
#define RENAME_1(a, b, ...)  DEFAULT_ARG_1(__auto_type, ##__VA_ARGS__) __MACRO_EXCLUSIVE_PREFIX##b = (a);
// DON'T reuse possible given type for RENAME_2, the intermediate variables are already the correct type (the given type might be in a 'typeof(var)' format, and the name of 'var' could be shadowed).
#define RENAME_2(a, b, ...)  __auto_type b = __MACRO_EXCLUSIVE_PREFIX##b;

#define RENAME(...)                                    \
	FOREACH_UNGUARDED(RENAME_1, ##__VA_ARGS__);    \
	FOREACH_UNGUARDED(RENAME_2, ##__VA_ARGS__);


//==========================================================================================================================================
//= Return Value Via a Function Argument
//==========================================================================================================================================


/* THIS IS VERY DANGEROUS! (e.g. int * _ptr, long _val)
 *
 * We can't just dereference, because if 'ptr' is NULL (not just equal, but actually the token NULL,
 * like when NULL is passed as an argument of a macro) we will be dereferencing a void *.
 * Therefor, we need to explicitly copy the memory region.
 */
/* #undef  arg_return
#define arg_return(_ptr, _val)                                   \
do {                                                             \
	RENAME((_ptr, ptr), (_val, val));                        \
	unsigned char * mem = (unsigned char *) ptr;             \
	const long N = sizeof(val);                              \
	long i;                                                  \
	if (mem != NULL)                                         \
		for (i=0;i<N;i++)                                \
			mem[i] = ((unsigned char *) &val)[i];    \
} while (0) */

/* It is very dangerous to attempt to guess the type of the pointer '_ptr' from the type of '_val' (e.g. _ptr: int * , _val: long).
 * If '_ptr' ends up of type void * (e.g. with NULL as argument to a macro), then fix your macro by
 * enforcing the appropriate type for the pointer.
 */
#undef  arg_return
#define arg_return(_ptr, _val)                                                                                         \
do {                                                                                                                   \
	RENAME((_ptr, ptr), (_val, val));                                                                              \
	_Static_assert(                                                                                                \
		_Generic((ptr),                                                                                        \
			void * : 0,                                                                                    \
			default: 1                                                                                     \
		),                                                                                                     \
		"Pointer passed to macro <arg_return()> is of type <void *>, possibly by passing NULL as argument."    \
		" Enforce the appropriate type for the pointer before passing it."                                     \
	);                                                                                                             \
	if (ptr != NULL)                                                                                               \
		*ptr = (val);                                                                                          \
} while (0)


#undef  arg_return_or_free
#define arg_return_or_free(ptr, val)    \
do {                                    \
	if (ptr != NULL)                \
		*ptr = (val);           \
	else                            \
		free(ptr);              \
} while (0)


//==========================================================================================================================================
//= malloc
//==========================================================================================================================================


#undef  safe_aligned_alloc
#define safe_aligned_alloc(_alignment, _size)                                                                 \
({                                                                                                            \
	RENAME((_alignment, alignment), (_size, size));                                                       \
	void * ret = aligned_alloc((alignment), (alignment) * (((size) + (alignment) - 1) / (alignment)));    \
	if (ret == NULL)                                                                                      \
		error("aligned_alloc");                                                                       \
	ret;                                                                                                  \
})


//==========================================================================================================================================
//= Swap Values
//==========================================================================================================================================


// We can't raname a, b because we need to store to them.
#undef  SWAP
#define SWAP(a, b)                     \
do {                                   \
	__auto_type __SWAP_buf = a;    \
	a = b;                         \
	b = __SWAP_buf;                \
} while (0)


//==========================================================================================================================================
//= Absolute Value
//==========================================================================================================================================


#undef  ABS
#define ABS(_v)                \
({                             \
	RENAME((_v, v));       \
	(v >= 0) ? v : - v;    \
})


//==========================================================================================================================================
//= Binary Search
//==========================================================================================================================================


#define _binary_search_default_cmp(target, A, i)           \
({                                                         \
	(target > A[i]) ? 1 : (target < A[i]) ? -1 : 0;    \
})

#define _binary_search_default_dist(target, A, i)    \
({                                                   \
	(ABS(target - A[i]));                        \
})

#define _binary_search_base(_A, _index_lower_value, _index_upper_value, _target, _boundary_lower_ptr, _boundary_upper_ptr, _cmp_fun, _dist_fun)    \
({                                                                                                                                                 \
	RENAME((_A, A), (_index_lower_value, index_lower_value), (_index_upper_value, index_upper_value), (_target, target),                       \
			(_boundary_lower_ptr, boundary_lower_ptr, long *), (_boundary_upper_ptr, boundary_upper_ptr, long *));                     \
	long s, e, m, ret;                                                                                                                         \
	s = (index_lower_value);                                                                                                                   \
	e = (index_upper_value);                                                                                                                   \
	if (_cmp_fun(target, A, s) < 0)                                                                                                            \
	{                                                                                                                                          \
		ret = s;                                                                                                                           \
		e = s;                                                                                                                             \
		s = -1;                                                                                                                            \
	}                                                                                                                                          \
	else if (_cmp_fun(target, A, e) > 0)                                                                                                       \
	{                                                                                                                                          \
		ret = e;                                                                                                                           \
		s = e;                                                                                                                             \
		e = -1;                                                                                                                            \
	}                                                                                                                                          \
	else                                                                                                                                       \
	{                                                                                                                                          \
		while (1)                                                                                                                          \
		{                                                                                                                                  \
			m = (s + e) / 2;                                                                                                           \
			if (m == s || m == e)                                                                                                      \
				break;                                                                                                             \
			if (_cmp_fun(target, A, m) > 0)                                                                                            \
				s = m;                                                                                                             \
			else                                                                                                                       \
				e = m;                                                                                                             \
		}                                                                                                                                  \
		if (_cmp_fun(target, A, s) == 0)                                                                                                   \
			ret = e = s;                                                                                                               \
		else if (_cmp_fun(target, A, e) == 0)                                                                                              \
			ret = s = e;                                                                                                               \
		else                                                                                                                               \
			ret = (_dist_fun(target, A, s) < _dist_fun(target, A, e)) ? s : e;                                                         \
	}                                                                                                                                          \
	if (boundary_lower_ptr != NULL)                                                                                                            \
		*boundary_lower_ptr = s;                                                                                                           \
	if (boundary_upper_ptr != NULL)                                                                                                            \
		*boundary_upper_ptr = e;                                                                                                           \
	ret;                                                                                                                                       \
})


/* Index boundaries are inclusive: [index_lower_value, index_upper_value].
 *
 * The boundaries returned are the closest possible from up and down,
 * i.e. only one of the following is true:
 * 
 *     A[boundary_lower_ptr]     ==  target  ==  A[boundary_upper_ptr]
 * or
 *     A[boundary_lower_ptr]      <  target  <   A[boundary_upper_ptr]
 * or
 *     boundary_lower_ptr == -1   ,  target  <   A[boundary_upper_ptr]       -> smaller than everything
 * or
 *     A[boundary_lower_ptr]      <  target  ,   boundary_upper_ptr == -1    -> larger than everything
 */ 

#undef  binary_search
#define binary_search(A, index_lower_value, index_upper_value, target, boundary_lower_ptr, boundary_upper_ptr, ... /* compare_function, distance_function */)    \
({                                                                                                                                                               \
	_binary_search_base(A, index_lower_value, index_upper_value, target, boundary_lower_ptr, boundary_upper_ptr,                                             \
			/* either given both, or use the default for both */                                                                                     \
			DEFAULT_ARG_2(_binary_search_default_cmp, ##__VA_ARGS__),                                                                                \
			DEFAULT_ARG_2(_binary_search_default_dist, ##__VA_ARGS__));                                                                              \
})


#endif /* MACROLIB_H */

