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
#define _CONCAT_EXPAND(a, b)  _CONCAT(a, b)
#define CONCAT(a, b)  _CONCAT_EXPAND(a, b)


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


/* Default function argument mechanism.
 * One can actually use it for ANY macro argument, by simply leaving it empty.
 * Example:
 *     #define macro(arg1, arg2, arg3) fun(arg1, DEFAULT_ARG_1(0, arg2), arg3)
 *     macro(arg1, , arg3);
 */
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                               Tuples                                                                   -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Packing / Unpacking
//==========================================================================================================================================


// Pack args in a tuple. 
#define PACK(...)  (__VA_ARGS__)

// Unpack tuple as arg list, do nothing when not a tuple.
#define _UNPACK_ID(...)  __VA_ARGS__
#define _UNPACK_TEST_IF_TUPLE(...)  /* one */, /* two */
// If there is a third argument (i.e. if _UNPACK_TEST_IF_TUPLE was called), args are unpacked with _UNPACK_ID.
// Else they are returned as is (args where not packed - not a tuple).
// No space before 'args' for purely aesthetic reasons, so that no space remains when 'OPT()' expands to empty (i.e. 'args' was not in a tuple).
#define _UNPACK(args, one, ...)  OPT(_UNPACK_ID, ##__VA_ARGS__)args
// Always passed as only 2 arguments, so we pass them again.
#define _UNPACK_EXPAND_ARGS(...)  _UNPACK(__VA_ARGS__)
// When 'tuple' is a tuple '_UNPACK_TEST_IF_TUPLE' is called and it returns two arguments.
// When 'tuple' is not a tuple it is not called, and the two are counted as one argument (no commas).
#define UNPACK(tuple)  _UNPACK_EXPAND_ARGS(tuple, _UNPACK_TEST_IF_TUPLE tuple)


//==========================================================================================================================================
//= Test If Empty
//==========================================================================================================================================


#define __TUPLE_NOT_EMPTY(...)  OPT_NEG(0, ##__VA_ARGS__) OPT(1, ##__VA_ARGS__)
#define _TUPLE_NOT_EMPTY(...)   __TUPLE_NOT_EMPTY(__VA_ARGS__)
#define TUPLE_NOT_EMPTY(t)      _TUPLE_NOT_EMPTY(UNPACK(t))


//==========================================================================================================================================
//= Concat Tuples
//==========================================================================================================================================


#define _CONCAT_TUPLES_0_0(t1, t2)  ()
#define _CONCAT_TUPLES_0_1(t1, t2)  (UNPACK(t2))
#define _CONCAT_TUPLES_1_0(t1, t2)  (UNPACK(t1))
#define _CONCAT_TUPLES_1_1(t1, t2)  (UNPACK(t1), UNPACK(t2))

#define _CONCAT_TUPLES_SELECT(t1_not_empty, t2_not_empty, t1, t2)  _CONCAT_TUPLES_ ## t1_not_empty ## _ ## t2_not_empty(t1, t2)  

#define _CONCAT_TUPLES_EXPAND(t1_not_empty, t2_not_empty, t1, t2)  _CONCAT_TUPLES_SELECT(t1_not_empty, t2_not_empty, t1, t2)
#define CONCAT_TUPLES(t1, t2)  _CONCAT_TUPLES_EXPAND(TUPLE_NOT_EMPTY(t1), TUPLE_NOT_EMPTY(t2), t1, t2)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Foreach Macro Argument                                                           -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//------------------------------------------------------------------------------------------------------------------------------------------
//- Foreach Base
//------------------------------------------------------------------------------------------------------------------------------------------

/* Iterates over 'tpl_arg_vals'.
 * Calls 'fun' foreach arg in 'tpl_arg_vals' as: fun(tpl_prefix, arg, tpl_suffix).
 */

#define _FOREACH_EXPAND_0(i, fun, ...)  fun(__VA_ARGS__)
#define _FOREACH_EXPAND_1(i, fun, ...)  fun(i, __VA_ARGS__)

#define _FOREACH_EXPAND(expand_ver, i, fun, tpl_prefix, a, tpl_suffix)  expand_ver(i, fun, UNPACK(CONCAT_TUPLES(CONCAT_TUPLES(tpl_prefix, a), tpl_suffix)))


// As is.
#define _FOREACH_AS_IS_0(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix)              UNPACK(tpl_src_code)
#define _FOREACH_AS_IS_REC(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix, a, ...)    (UNPACK(tpl_src_code) _FOREACH_EXPAND(expand_ver, i, fun, tpl_prefix, a, tpl_suffix)), expand_ver, fun, tpl_prefix, tpl_suffix, ##__VA_ARGS__

// As semicolon-terminated statements.
#define _FOREACH_AS_STMT_0(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix)            UNPACK(tpl_src_code)
#define _FOREACH_AS_STMT_REC(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix, a, ...)  (UNPACK(tpl_src_code) _FOREACH_EXPAND(expand_ver, i, fun, tpl_prefix, a, tpl_suffix);), expand_ver, fun, tpl_prefix, tpl_suffix, ##__VA_ARGS__

// As a comma-separated list of expressions.
#define _FOREACH_AS_EXPR_0(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix)            UNPACK(tpl_src_code)
#define _FOREACH_AS_EXPR_REC(i, tpl_src_code, expand_ver, fun, tpl_prefix, tpl_suffix, a, ...)  CONCAT_TUPLES(tpl_src_code, (_FOREACH_EXPAND(expand_ver, i, fun, tpl_prefix, a, tpl_suffix))), expand_ver, fun, tpl_prefix, tpl_suffix, ##__VA_ARGS__


#define _FOREACH_BASE(foreach_rec, foreach_0, pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)  RECURSION_FORM_ITER(UNPACK(tpl_arg_vals)) (foreach_rec, foreach_0, (, _FOREACH_EXPAND_ ## pass_iter, fun, tpl_prefix, tpl_suffix, UNPACK(tpl_arg_vals)))


#define FOREACH_AS_IS(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)              _FOREACH_BASE(_FOREACH_AS_IS_REC, _FOREACH_AS_IS_0, pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)

#define FOREACH_AS_STMT_UNGUARDED(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)  _FOREACH_BASE(_FOREACH_AS_STMT_REC, _FOREACH_AS_STMT_0, pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)
#define FOREACH_AS_STMT(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)            do { FOREACH_AS_STMT_UNGUARDED(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix) } while (0)

#define FOREACH_AS_EXPR_UNGUARDED(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)  _FOREACH_BASE(_FOREACH_AS_EXPR_REC, _FOREACH_AS_EXPR_0, pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)
#define FOREACH_AS_EXPR(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix)            ( FOREACH_AS_EXPR_UNGUARDED(pass_iter, fun, tpl_prefix, tpl_arg_vals, tpl_suffix) )


//------------------------------------------------------------------------------------------------------------------------------------------
//- Foreach
//------------------------------------------------------------------------------------------------------------------------------------------


// #define _FOREACH_EXPAND(fun, ...)  fun(__VA_ARGS__)

// #define _FOREACH_0(i, tpl_src_code, fun)            UNPACK(tpl_src_code)
// #define _FOREACH_REC(i, tpl_src_code, fun, a, ...)  (UNPACK(tpl_src_code) _FOREACH_EXPAND(fun, UNPACK(a));), fun, ##__VA_ARGS__

// #define FOREACH_UNGUARDED(fun, ...)  RECURSION_FORM_ITER(__VA_ARGS__)(_FOREACH_REC, _FOREACH_0, (, fun, __VA_ARGS__))
// #define FOREACH(fun, ...)            do { FOREACH_UNGUARDED(fun, ##__VA_ARGS__); } while (0)

#define FOREACH_UNGUARDED(fun, ...)  FOREACH_AS_STMT_UNGUARDED(0, fun, , (__VA_ARGS__), )
#define FOREACH(fun, ...)            FOREACH_AS_STMT(0, fun,  , (__VA_ARGS__), )


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                           Macro Functions                                                              -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Rename Macro Arguments
//==========================================================================================================================================


/* RENAME( list of tupples : (name_initial, name_final, [type]) )
 *     Rename macro arguments to avoid names clashing with macro variables.
 *     Optionally pass the type.
 */

#define __RENAME_PREFIX __eb320f0c2b6a25b48ca861a120eea902__     // MD5 of "macro" (no reason)
#define _RENAME_1(a, b, ...)  DEFAULT_ARG_1(__auto_type, __VA_ARGS__) __MACRO_EXCLUSIVE_PREFIX##b = (a);
// DON'T reuse possible given type for _RENAME_2, the intermediate variables are already the correct type (the given type might be in a 'typeof(var)' format, and the name of 'var' could be shadowed).
#define _RENAME_2(a, b, ...)  __auto_type b = __MACRO_EXCLUSIVE_PREFIX##b;

#define RENAME(...)                                     \
	FOREACH_UNGUARDED(_RENAME_1, ##__VA_ARGS__);    \
	FOREACH_UNGUARDED(_RENAME_2, ##__VA_ARGS__);


//==========================================================================================================================================
//= Unsafe Cast To Type
//==========================================================================================================================================


/* #define cast_to_type_unsafe(x, trg_type)                                       \
({                                                                             \
	__auto_type _cast_to_type_unsafe_x = x;                                \
	*((typeof(typeof(trg_type) *)) ((char *) &_cast_to_type_unsafe_x));    \
}) */


#define cast_to_type_unsafe(x, trg_type)                                            \
({                                                                                  \
	typeof(x) _cast_to_type_unsafe_x;                                           \
	typeof(trg_type) * _cast_to_type_unsafe_ptr;                                \
	_cast_to_type_unsafe_x = x;                                                 \
	_cast_to_type_unsafe_ptr = (typeof(trg_type) *) &_cast_to_type_unsafe_x;    \
	*_cast_to_type_unsafe_ptr;                                                  \
})


//==========================================================================================================================================
//= Return Value Via a Function Argument
//==========================================================================================================================================


#ifdef __cplusplus

	#define assert_non_void_type(ptr, str)                          \
	do {                                                            \
		_Static_assert(                                         \
			!std::is_same<decltype(ptr), void *>::value,    \
			str                                             \
		);                                                      \
	} while (0)

#else

	#define assert_non_void_type(ptr, str)    \
	do {                                      \
		_Static_assert(                   \
			_Generic((ptr),           \
				void * : 0,       \
				default: 1        \
			),                        \
			str                       \
		);                                \
	} while (0)

#endif /* __cplusplus */


/* Return '_val' if '_ptr_out' not NULL.
 *
 * It is very dangerous to attempt to guess the type of the pointer '_ptr_out' from the type of '_val' (e.g. _ptr_out: int * , _val: long).
 * If '_ptr_out' ends up of type void * (e.g. with NULL as argument to a macro), then fix your macro by
 * enforcing the appropriate type for the pointer.
 */
#undef  arg_return
#define arg_return(_ptr_out, _val)                                                                                            \
do {                                                                                                                          \
	RENAME((_ptr_out, ptr_out), (_val, val));                                                                             \
	assert_non_void_type(ptr_out,                                                                                         \
		"Output pointer passed to macro <arg_return()> is of type <void *>, possibly by passing NULL as argument."    \
		" Enforce the appropriate type for the pointer before passing it."                                            \
	);                                                                                                                    \
	if (ptr_out != NULL)                                                                                                  \
		*ptr_out = val;                                                                                               \
} while (0)


/* Return '_val' if '_ptr_out' not NULL, or free it.
 */
#undef  arg_return_or_free
#define arg_return_or_free(_ptr_out, _val)                                                                                            \
do {                                                                                                                                  \
	RENAME((_ptr_out, ptr_out), (_val, val));                                                                                     \
	assert_non_void_type(ptr_out,                                                                                                 \
		"Output pointer passed to macro <arg_return_or_free()> is of type <void *>, possibly by passing NULL as argument."    \
		" Enforce the appropriate type for the pointer before passing it."                                                    \
	);                                                                                                                            \
	if (ptr_out != NULL)                                                                                                          \
		*ptr_out = val;                                                                                                       \
	else                                                                                                                          \
		free(val);                                                                                                            \
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


#undef  SWAP
#define SWAP(_a_ptr, _b_ptr)                         \
do {                                                 \
	RENAME((_a_ptr, a_ptr), (_b_ptr, b_ptr));    \
	__auto_type buf = *a_ptr;                    \
	*a_ptr = *b_ptr;                             \
	*b_ptr = buf;                                \
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


//------------------------------------------------------------------------------------------------------------------------------------------
//- Binary Search - Strictly Defined Boundaries
//------------------------------------------------------------------------------------------------------------------------------------------


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


#define __binary_search(_A, _index_lower_value, _index_upper_value, _target, _boundary_lower_ptr, _boundary_upper_ptr, _cmp_fun, _dist_fun)    \
({                                                                                                                                             \
	RENAME((_A, A), (_index_lower_value, index_lower_value), (_index_upper_value, index_upper_value), (_target, target),                   \
			(_boundary_lower_ptr, __boundary_lower_ptr, long *), (_boundary_upper_ptr, __boundary_upper_ptr, long *));             \
	long s, e, m, ret;                                                                                                                     \
	long * boundary_lower_ptr = __boundary_lower_ptr;                                                                                      \
	long * boundary_upper_ptr = __boundary_upper_ptr;                                                                                      \
                                                                                                                                               \
	s = (index_lower_value);                                                                                                               \
	e = (index_upper_value);                                                                                                               \
	if (_cmp_fun(target, A, s) < 0)                                                                                                        \
	{                                                                                                                                      \
		ret = s;                                                                                                                       \
		e = s;                                                                                                                         \
		s = -1;                                                                                                                        \
	}                                                                                                                                      \
	else if (_cmp_fun(target, A, e) > 0)                                                                                                   \
	{                                                                                                                                      \
		ret = e;                                                                                                                       \
		s = e;                                                                                                                         \
		e = -1;                                                                                                                        \
	}                                                                                                                                      \
	else                                                                                                                                   \
	{                                                                                                                                      \
		while (1)                                                                                                                      \
		{                                                                                                                              \
			m = (s + e) / 2;                                                                                                       \
			if (m == s || m == e)                                                                                                  \
				break;                                                                                                         \
			if (_cmp_fun(target, A, m) > 0)                                                                                        \
				s = m;                                                                                                         \
			else                                                                                                                   \
				e = m;                                                                                                         \
		}                                                                                                                              \
		if (_cmp_fun(target, A, s) == 0)                                                                                               \
			ret = e = s;                                                                                                           \
		else if (_cmp_fun(target, A, e) == 0)                                                                                          \
			ret = s = e;                                                                                                           \
		else                                                                                                                           \
			ret = (_dist_fun(target, A, s) < _dist_fun(target, A, e)) ? s : e;                                                     \
	}                                                                                                                                      \
	arg_return(boundary_lower_ptr, s);                                                                                                     \
	arg_return(boundary_upper_ptr, e);                                                                                                     \
	ret;                                                                                                                                   \
})


#undef  binary_search
#define binary_search(A, index_lower_value, index_upper_value, target, boundary_lower_ptr, boundary_upper_ptr, ... /* compare_function, distance_function */)    \
({                                                                                                                                                               \
	__binary_search(A, index_lower_value, index_upper_value, target, boundary_lower_ptr, boundary_upper_ptr,                                                 \
			DEFAULT_ARG_1(_binary_search_default_cmp, __VA_ARGS__),                                                                                  \
			DEFAULT_ARG_2(_binary_search_default_dist, __VA_ARGS__));                                                                                \
})


//------------------------------------------------------------------------------------------------------------------------------------------
//- Binary Search - Simple
//------------------------------------------------------------------------------------------------------------------------------------------


/* Index boundaries are inclusive: [index_lower_value, index_upper_value].
 *
 * If no equal value found, returns the lower position.
 */ 


#define __binary_search_simple(_A, _index_lower_value, _index_upper_value, _target, _cmp_fun)                                    \
({                                                                                                                               \
	RENAME((_A, A), (_index_lower_value, index_lower_value), (_index_upper_value, index_upper_value), (_target, target));    \
	long s, e, m, ret;                                                                                                       \
                                                                                                                                 \
	s = (index_lower_value);                                                                                                 \
	e = (index_upper_value);                                                                                                 \
	if (_cmp_fun(target, A, s) < 0)                                                                                          \
		ret = s;                                                                                                         \
	else if (_cmp_fun(target, A, e) > 0)                                                                                     \
		ret = e;                                                                                                         \
	else                                                                                                                     \
	{                                                                                                                        \
		while (1)                                                                                                        \
		{                                                                                                                \
			m = (s + e) / 2;                                                                                         \
			if (m == s || m == e)                                                                                    \
				break;                                                                                           \
			if (_cmp_fun(target, A, m) > 0)                                                                          \
				s = m;                                                                                           \
			else                                                                                                     \
				e = m;                                                                                           \
		}                                                                                                                \
		if (_cmp_fun(target, A, s) == 0)                                                                                 \
			ret = s;                                                                                                 \
		else if (_cmp_fun(target, A, e) == 0)                                                                            \
			ret = e;                                                                                                 \
		else                                                                                                             \
			ret = s;                                                                                                 \
	}                                                                                                                        \
	ret;                                                                                                                     \
})


#undef  binary_search_simple
#define binary_search_simple(A, index_lower_value, index_upper_value, target, ... /* compare_function */)                                   \
({                                                                                                                                          \
	__binary_search_simple(A, index_lower_value, index_upper_value, target, DEFAULT_ARG_1(_binary_search_default_cmp, __VA_ARGS__));    \
})


#endif /* MACROLIB_H */

