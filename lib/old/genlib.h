#ifndef GENLIB_H
#define GENLIB_H

#ifdef __cplusplus
	#error "genlib.h is a strictly C library, can't compile as C++ source"
#endif

#include <assert.h>
#include <complex.h>
#include <tgmath.h>
#include <string.h>
#include <limits.h>   // for CHAR_BIT

#include "debug.h"
#include "macros/macrolib.h"


/* 
 * Complex numbers:
 *     The C programming language, as of C99, supports complex number math with the three built-in types double _Complex, float _Complex, and long double _Complex (see _Complex).
 *     When the header <complex.h> is included, the three complex number types are also accessible as double complex, float complex, long double complex.
 */


#define GENLIB_is_ws(c)  (c == ' ' || c == '\n' || c == '\r' || c == '\t')


#define GENLIB_rule_expand_storage_classes(type, ...)    \
	type: __VA_ARGS__,                               \
	const type: __VA_ARGS__,                         \
	volatile type: __VA_ARGS__,                      \
	volatile const type: __VA_ARGS__


#define GENLIB_rule_expand_storage_classes_and_sign(type, code...)    \
	GENLIB_rule_expand_storage_classes(type, code),               \
	GENLIB_rule_expand_storage_classes(unsigned type, code)


//==========================================================================================================================================
//= Type Checks
//==========================================================================================================================================


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                            Type Checking                                                               -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define gen_type_equals(var, T)    \
(                                  \
	_Generic((var),            \
		typeof(T): 1,      \
		default: 0         \
	)                          \
)


#define gen_type_is_basic_int(var)                                            \
(                                                                             \
	_Generic((var),                                                       \
		GENLIB_rule_expand_storage_classes_and_sign(char, 1),         \
		GENLIB_rule_expand_storage_classes(signed char, 1),           \
		GENLIB_rule_expand_storage_classes_and_sign(short, 1),        \
		GENLIB_rule_expand_storage_classes_and_sign(int, 1),          \
		GENLIB_rule_expand_storage_classes_and_sign(long, 1),         \
		GENLIB_rule_expand_storage_classes_and_sign(long long, 1),    \
		default: 0                                                    \
	)                                                                     \
)


#define gen_type_is_basic_int_ptr(var)                                          \
(                                                                               \
	_Generic((var),                                                         \
		GENLIB_rule_expand_storage_classes_and_sign(char *, 1),         \
		GENLIB_rule_expand_storage_classes(signed char *, 1),           \
		GENLIB_rule_expand_storage_classes_and_sign(short *, 1),        \
		GENLIB_rule_expand_storage_classes_and_sign(int *, 1),          \
		GENLIB_rule_expand_storage_classes_and_sign(long *, 1),         \
		GENLIB_rule_expand_storage_classes_and_sign(long long *, 1),    \
		default: 0                                                      \
	)                                                                       \
)


#define gen_type_is_basic_float(var)                                           \
(                                                                              \
	_Generic((var),                                                        \
		GENLIB_rule_expand_storage_classes(float, 1),                  \
		GENLIB_rule_expand_storage_classes(double, 1),                 \
		GENLIB_rule_expand_storage_classes(long double, 1),            \
		GENLIB_rule_expand_storage_classes(complex float, 1),          \
		GENLIB_rule_expand_storage_classes(complex double, 1),         \
		GENLIB_rule_expand_storage_classes(complex long double, 1),    \
		default: 0                                                     \
	)                                                                      \
)


#define gen_type_is_basic_float_ptr(var)                                         \
(                                                                                \
	_Generic((var),                                                          \
		GENLIB_rule_expand_storage_classes(float *, 1),                  \
		GENLIB_rule_expand_storage_classes(double *, 1),                 \
		GENLIB_rule_expand_storage_classes(long double *, 1),            \
		GENLIB_rule_expand_storage_classes(complex float *, 1),          \
		GENLIB_rule_expand_storage_classes(complex double *, 1),         \
		GENLIB_rule_expand_storage_classes(complex long double *, 1),    \
		default: 0                                                       \
	)                                                                        \
)


#define gen_type_is_basic(var)                                        \
(                                                                     \
	gen_type_is_basic_int(var) || gen_type_is_basic_float(var)    \
)

#define gen_type_is_basic_ptr(var)                                            \
(                                                                             \
	gen_type_is_basic_int_ptr(var) || gen_type_is_basic_float_ptr(var)    \
)

#define gen_type_is_basic_or_void_ptr(var)    \
(                                             \
	_Generic((var),                       \
		void * : 1,                   \
		default: 0                    \
	) || gen_type_is_basic_ptr(var)       \
)


//==========================================================================================================================================
//= Asserts
//==========================================================================================================================================


#define gen_assert_type_equals(var, type)                              \
do {                                                                   \
	_Static_assert(                                                \
		gen_type_equals(var, type),                            \
		"Type of <" STRING(var) "> should be " STRING(type)    \
	);                                                             \
} while (0)


#define gen_assert_type_is_basic_int(var, ... /* error_message */)                                              \
do {                                                                                                            \
	_Static_assert(                                                                                         \
		gen_type_is_basic_int(var),                                                                     \
		"Type of <" STRING(var) "> is not a basic integer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                      \
} while (0)


#define gen_assert_type_is_basic_int_ptr(var, ... /* error_message */)                                                  \
do {                                                                                                                    \
	_Static_assert(                                                                                                 \
		gen_type_is_basic_int_ptr(var),                                                                         \
		"Type of <" STRING(var) "> is not a basic integer pointer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                              \
} while (0)


#define gen_assert_type_is_basic_float(var, ... /* error_message */)                                                   \
do {                                                                                                                   \
	_Static_assert(                                                                                                \
		gen_type_is_basic_float(var),                                                                          \
		"Type of <" STRING(var) "> is not a basic floating-point type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                             \
} while (0)


#define gen_assert_type_is_basic_float_ptr(var, ... /* error_message */)                                                       \
do {                                                                                                                           \
	_Static_assert(                                                                                                        \
		gen_type_is_basic_float_ptr(var),                                                                              \
		"Type of <" STRING(var) "> is not a basic floating-point pointer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                                     \
} while (0)


#define gen_assert_type_is_basic(var, ... /* error_message */)                                          \
do {                                                                                                    \
	_Static_assert(                                                                                 \
		gen_type_is_basic(var),                                                                 \
		"Type of <" STRING(var) "> is not a basic type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                              \
} while (0)


#define gen_assert_type_is_basic_ptr(var, ... /* error_message */)                                              \
do {                                                                                                            \
	_Static_assert(                                                                                         \
		gen_type_is_basic_ptr(var),                                                                     \
		"Type of <" STRING(var) "> is not a basic pointer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                      \
} while (0)


#define gen_assert_type_is_basic_ptr_or_void_ptr(var, ... /* error_message */)                                          \
do {                                                                                                                    \
	_Static_assert(                                                                                                 \
		gen_type_is_basic_or_void_ptr(var),                                                                     \
		"Type of <" STRING(var) "> is not a basic or void pointer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                              \
} while (0)


#define gen_assert_type_is_basic_or_basic_ptr(var, ... /* error_message */)                                                     \
do {                                                                                                                            \
	_Static_assert(                                                                                                         \
		gen_type_is_basic(var) || gen_type_is_basic_ptr(var),                                                           \
		"Type of <" STRING(var) "> is not a basic type or a basic pointer type" OPT((__VA_ARGS__), ": ") __VA_ARGS__    \
	);                                                                                                                      \
} while (0)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Conversions                                                                -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Convert Integer to Unsigned
//==========================================================================================================================================


#define gen_cast_integer_to_unsigned(var)                                                              \
({                                                                                                     \
	/* fail for non-simple array types */                                                          \
	gen_assert_type_is_basic_int((var), "gen_cast_integer_to_unsigned");                           \
	_Generic((var),                                                                                \
		GENLIB_rule_expand_storage_classes_and_sign(char, (unsigned char) (var)),              \
		GENLIB_rule_expand_storage_classes(signed char, (unsigned char) (var)),                \
		GENLIB_rule_expand_storage_classes_and_sign(short, (unsigned short) (var)),            \
		GENLIB_rule_expand_storage_classes_and_sign(int, (unsigned int) (var)),                \
		GENLIB_rule_expand_storage_classes_and_sign(long, (unsigned long) (var)),              \
		GENLIB_rule_expand_storage_classes_and_sign(long long, (unsigned long long) (var)),    \
		default: (var)                                                                         \
	);                                                                                             \
})


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                     String / Printing Utilities                                                        -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Basic Type Name to String
//==========================================================================================================================================


#define GENLIB_rule_type_string_storage_classes(type)    \
	type: STRING(type),                              \
	const type: "const " STRING(type),               \
	volatile type: "volatile " STRING(type),         \
	volatile const type: "volatile const " STRING(type)

#define GENLIB_rule_type_string_sign(type)                \
	GENLIB_rule_type_string_storage_classes(type),    \
	GENLIB_rule_type_string_storage_classes(unsigned type)

#define GENLIB_rule_pointer_string_storage_classes(type)    \
	GENLIB_rule_type_string_storage_classes(type),      \
	GENLIB_rule_type_string_storage_classes(type *)

#define GENLIB_rule_pointer_string_storage_classes_and_sign(type)    \
	GENLIB_rule_type_string_sign(type),                          \
	GENLIB_rule_type_string_sign(type *)


#define gen_basic_type_name_to_string(var)                                          \
(                                                                                   \
	_Generic((var),                                                             \
		void *: "void *",                                                   \
		GENLIB_rule_pointer_string_storage_classes_and_sign(char),          \
		GENLIB_rule_pointer_string_storage_classes(signed char),            \
		GENLIB_rule_pointer_string_storage_classes_and_sign(short),         \
		GENLIB_rule_pointer_string_storage_classes_and_sign(int),           \
		GENLIB_rule_pointer_string_storage_classes_and_sign(long),          \
		GENLIB_rule_pointer_string_storage_classes_and_sign(long long),     \
		GENLIB_rule_pointer_string_storage_classes(float),                  \
		GENLIB_rule_pointer_string_storage_classes(double),                 \
		GENLIB_rule_pointer_string_storage_classes(long double),            \
		GENLIB_rule_pointer_string_storage_classes(complex float),          \
		GENLIB_rule_pointer_string_storage_classes(complex double),         \
		GENLIB_rule_pointer_string_storage_classes(complex long double),    \
		default: ""                                                         \
	)                                                                           \
)


//==========================================================================================================================================
//= Number to String
//==========================================================================================================================================


#define gen_numtostr(str, N, val)                                                                                                                                    \
({                                                                                                                                                                   \
	int _len = 0;                                                                                                                                                \
	char * _str = (str);                                                                                                                                         \
	__auto_type _val = (val);                                                                                                                                    \
	gen_assert_type_is_basic(_val, "gen_numtostr");                                                                                                              \
	_len = _Generic(_val,                                                                                                                                        \
		GENLIB_rule_expand_storage_classes(char,                snprintf(_str, N, "%d"     , (char               ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(unsigned char,       snprintf(_str, N, "%u"     , (char               ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(signed char,         snprintf(_str, N, "%d"     , (signed char        ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(short,               snprintf(_str, N, "%d"     , (short              ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(unsigned short,      snprintf(_str, N, "%u"     , (short              ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(int,                 snprintf(_str, N, "%d"     , (int                ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(unsigned int,        snprintf(_str, N, "%u"     , (int                ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(long,                snprintf(_str, N, "%ld"    , (long               ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(unsigned long,       snprintf(_str, N, "%lu"    , (long               ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(long long,           snprintf(_str, N, "%lld"   , (long long          ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(unsigned long long,  snprintf(_str, N, "%llu"   , (long long          ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(float,               snprintf(_str, N, "%g"     , (float              ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(double,              snprintf(_str, N, "%lg"    , (double             ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(long double,         snprintf(_str, N, "%Lg"    , (long double        ) _val)),                                   \
		GENLIB_rule_expand_storage_classes(complex float,       snprintf(_str, N, "%g %g"  , crealf(_val), cimagf(_val))),                                   \
		GENLIB_rule_expand_storage_classes(complex double,      snprintf(_str, N, "%lg %lg", creal((complex double) _val), cimag((complex double)_val))),    \
		GENLIB_rule_expand_storage_classes(complex long double, snprintf(_str, N, "%Lg %Lg", creall(_val), cimagl(_val)))                                    \
	);                                                                                                                                                           \
	_len;                                                                                                                                                        \
})


//==========================================================================================================================================
//= String to Number
//==========================================================================================================================================


static inline
int
GENLIB_find_ws(char * str, int len)
{
	int i = 0;
	while (i < len)
	{
		if (GENLIB_is_ws(str[i]))
			break;
		i++;
	}
	return i;
}


static inline
int
GENLIB_find_non_ws(char * str, int len)
{
	int i = 0;
	while (i < len)
	{
		if (!GENLIB_is_ws(str[i]))
			break;
		i++;
	}
	return i;
}


static inline
int
GENLIB_find_next_token(char * str, int len)
{
	int i = 0;
	i += GENLIB_find_ws(str, len - i);
	i += GENLIB_find_non_ws(str, len - i);
	return i;
}


static inline
void
GENLIB_cpy(char * restrict src, char * restrict dst, int N)
{
	int i = 0;
	for (i=0;i<N;i++)
		dst[i] = src[i];
}


/* In tgmath.h macros are defined with names like their 'double' counterparts of math.h or complex.h.
 * So we need to explicitly cast to 'double' or 'complex double' to avoid warnings when the type is different (e.g. float).
 *
 * We define the concrete inline functions because otherwise the compilation is unacceptably slow.
 */

#define GENLIB_safe_strtox(strtox, _str, _N, _len_ptr, _base...)                                                                         \
({                                                                                                                                       \
	RENAME((_str, str, char *), (_N, N, long), (_len_ptr, len_ptr), (DEFAULT_ARG_1(10, _base), base, , __attribute__((unused))));    \
	char * endptr;                                                                                                                   \
	int len;                                                                                                                         \
	long i = 0;                                                                                                                      \
	i += GENLIB_find_non_ws(str + i, N - i);                                                                                         \
	len = GENLIB_find_ws(str + i, N - i);                                                                                            \
	char buf[len + 1];                                                                                                               \
	strncpy(buf, str + i, len);                                                                                                      \
	buf[len] = '\0';                                                                                                                 \
	errno = 0;                                                                                                                       \
	__auto_type ret = OPT_TERNARY((_base),                                                                                           \
			strtox(buf, &endptr, base),                                                                                      \
			strtox(buf, &endptr));                                                                                           \
	if (errno != 0)                                                                                                                  \
		error(STRING(strtox));                                                                                                   \
	*len_ptr = i + len;                                                                                                              \
	ret;                                                                                                                             \
})

static inline
long
GENLIB_safe_strtol(char * str, long N, long * len_ptr, long base)
{
	return GENLIB_safe_strtox(strtol, str, N, len_ptr, base);
}

static inline
long long
GENLIB_safe_strtoll(char * str, long N, long * len_ptr, long base)
{
	return GENLIB_safe_strtox(strtoll, str, N, len_ptr, base);
}

static inline
float
GENLIB_safe_strtof(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox(strtof, str, N, len_ptr);
}

static inline
double
GENLIB_safe_strtod(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox(strtod, str, N, len_ptr);
}

static inline
long double
GENLIB_safe_strtold(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox(strtold, str, N, len_ptr);
}

#define GENLIB_safe_strtox_complex(safe_strtox, _str, _N, _len_ptr)         \
({                                                                          \
	RENAME((_str, str, char *), (_N, N, long), (_len_ptr, len_ptr));    \
	long len;                                                           \
	__auto_type real = safe_strtox(str, N, &len);                       \
	*len_ptr = len;                                                     \
	__auto_type imag = safe_strtox(str + len, N - len, &len);           \
	*len_ptr += len;                                                    \
	real + imag * I;                                                    \
})

static inline
complex float
GENLIB_safe_strtof_complex(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox_complex(GENLIB_safe_strtof, str, N, len_ptr);
}

static inline
complex double
GENLIB_safe_strtod_complex(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox_complex(GENLIB_safe_strtod, str, N, len_ptr);
}

static inline
complex long double
GENLIB_safe_strtold_complex(char * str, long N, long * len_ptr)
{
	return GENLIB_safe_strtox_complex(GENLIB_safe_strtold, str, N, len_ptr);
}


#define gen_strtonum(_str, _N, _var_ptr, _base...)                                                                       \
({                                                                                                                       \
	RENAME((_str, __str), (_N, N, long), (_var_ptr, var_ptr), (DEFAULT_ARG_1(10, _base), base));                     \
	char * str = __str;                                                                                              \
	long len;                                                                                                        \
	gen_assert_type_is_basic(*var_ptr, "gen_strtonum");                                                              \
	(*var_ptr) = _Generic((var_ptr),                                                                                 \
		GENLIB_rule_expand_storage_classes_and_sign(char *,       GENLIB_safe_strtol( str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes(signed char *,         GENLIB_safe_strtol( str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes_and_sign(short *,      GENLIB_safe_strtol( str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes_and_sign(int *,        GENLIB_safe_strtol( str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes_and_sign(long *,       GENLIB_safe_strtol( str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes_and_sign(long long *,  GENLIB_safe_strtoll(str, N, &len, base)),      \
		GENLIB_rule_expand_storage_classes(float *,               GENLIB_safe_strtof( str, N, &len)),            \
		GENLIB_rule_expand_storage_classes(double *,              GENLIB_safe_strtod( str, N, &len)),            \
		GENLIB_rule_expand_storage_classes(long double *,         GENLIB_safe_strtold(str, N, &len)),            \
		GENLIB_rule_expand_storage_classes(complex float *,       GENLIB_safe_strtof_complex( str, N, &len)),    \
		GENLIB_rule_expand_storage_classes(complex double *,      GENLIB_safe_strtod_complex( str, N, &len)),    \
		GENLIB_rule_expand_storage_classes(complex long double *, GENLIB_safe_strtold_complex(str, N, &len))     \
	);                                                                                                               \
	len;                                                                                                             \
})


//==========================================================================================================================================
//= Generic Printing of Basic Types
//==========================================================================================================================================


#define GENLIB_print(buf, N, val)                                                                                                                                                                                    \
do {                                                                                                                                                                                                                 \
	__auto_type _val = (val);                                                                                                                                                                                    \
	void * _ptr = &_val;                                                                                                                                                                                         \
	_i += _Generic(_val,                                                                                                                                                                                         \
		GENLIB_rule_expand_storage_classes(char *,              snprintf(buf + _i, N - _i, "%s"      , *((char **             ) _ptr))),                                                                     \
		GENLIB_rule_expand_storage_classes(char,                snprintf(buf + _i, N - _i, "%c"      , *((char *               ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(unsigned char,       snprintf(buf + _i, N - _i, "%c"      , *((char *               ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(signed char,         snprintf(buf + _i, N - _i, "%c"      , *((signed char *        ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(short,               snprintf(buf + _i, N - _i, "%d"      , *((short *              ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(unsigned short,      snprintf(buf + _i, N - _i, "%u"      , *((short *              ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(int,                 snprintf(buf + _i, N - _i, "%d"      , *((int *                ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(unsigned int,        snprintf(buf + _i, N - _i, "%u"      , *((int *                ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(long,                snprintf(buf + _i, N - _i, "%ld"     , *((long *               ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(unsigned long,       snprintf(buf + _i, N - _i, "%lu"     , *((long *               ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(long long,           snprintf(buf + _i, N - _i, "%lld"    , *((long long *          ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(unsigned long long,  snprintf(buf + _i, N - _i, "%llu"    , *((long long *          ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(float,               snprintf(buf + _i, N - _i, "%g"      , *((float *              ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(double,              snprintf(buf + _i, N - _i, "%lg"     , *((double *             ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(long double,         snprintf(buf + _i, N - _i, "%Lg"     , *((long double *        ) _ptr))),                                                                    \
		GENLIB_rule_expand_storage_classes(complex float,       snprintf(buf + _i, N - _i, "%g %gI"  , crealf(*((complex float *) _ptr)), cimagf(*((complex float *) _ptr)))),                               \
		GENLIB_rule_expand_storage_classes(complex double,      snprintf(buf + _i, N - _i, "%lg %lgI", creal(*((complex double *) _ptr)), cimag(*((complex double *) _ptr)))),                               \
		GENLIB_rule_expand_storage_classes(complex long double, snprintf(buf + _i, N - _i, "%Lg %LgI", creall(*((complex long double *) _ptr)), cimagl(*((complex long double *) _ptr))))    \
	);                                                                                                                                                                                                           \
} while (0)


#define gen_print(buf, n, ...)                                                    \
do {                                                                              \
	long _i = 0;                                                              \
	FOREACH_AS_STMT_UNGUARDED(0, GENLIB_print, (buf, n), (__VA_ARGS__), );    \
	printf("%s", buf);                                                        \
} while (0)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                   Type Conversion / Type Casting                                                       -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Basic Type to Double Converter Functions
//==========================================================================================================================================


__attribute__((hot)) static inline double gen_c2d  (void * A, long i) { return (double)      (((char *)                A)[i]); }
__attribute__((hot)) static inline double gen_sc2d (void * A, long i) { return (double)      (((signed char *)         A)[i]); }
__attribute__((hot)) static inline double gen_uc2d (void * A, long i) { return (double)      (((unsigned char *)       A)[i]); }
__attribute__((hot)) static inline double gen_s2d  (void * A, long i) { return (double)      (((short *)               A)[i]); }
__attribute__((hot)) static inline double gen_us2d (void * A, long i) { return (double)      (((unsigned short *)      A)[i]); }
__attribute__((hot)) static inline double gen_i2d  (void * A, long i) { return (double)      (((int *)                 A)[i]); }
__attribute__((hot)) static inline double gen_ui2d (void * A, long i) { return (double)      (((unsigned int *)        A)[i]); }
__attribute__((hot)) static inline double gen_l2d  (void * A, long i) { return (double)      (((long *)                A)[i]); }
__attribute__((hot)) static inline double gen_ul2d (void * A, long i) { return (double)      (((unsigned long *)       A)[i]); }
__attribute__((hot)) static inline double gen_ll2d (void * A, long i) { return (double)      (((long long *)           A)[i]); }
__attribute__((hot)) static inline double gen_ull2d(void * A, long i) { return (double)      (((unsigned long long *)  A)[i]); }
__attribute__((hot)) static inline double gen_f2d  (void * A, long i) { return (double)      (((float *)               A)[i]); }
__attribute__((hot)) static inline double gen_d2d  (void * A, long i) { return (double)      (((double *)              A)[i]); }
__attribute__((hot)) static inline double gen_ld2d (void * A, long i) { return (double)      (((long double *)         A)[i]); }
__attribute__((hot)) static inline double gen_cf2d (void * A, long i) { return (double) cabsf(((complex float *)       A)[i]); }
__attribute__((hot)) static inline double gen_cd2d (void * A, long i) { return (double) cabs (((complex double *)      A)[i]); }
__attribute__((hot)) static inline double gen_cld2d(void * A, long i) { return (double) cabsl(((complex long double *) A)[i]); }

#define gen_functor_convert_basic_type_to_double(var_ptr)                                \
({                                                                                       \
	/* fail for non-simple array types */                                            \
	gen_assert_type_is_basic_ptr_or_void_ptr((var_ptr),                              \
		"for non-simple array types a <get-and-convert-to-double> "              \
		"function must be provided, of type:  double ()(void * a, int i)"        \
	);                                                                               \
	_Generic((var_ptr),                                                              \
		GENLIB_rule_expand_storage_classes(char *,                gen_c2d  ),    \
		GENLIB_rule_expand_storage_classes(signed char *,         gen_sc2d ),    \
		GENLIB_rule_expand_storage_classes(unsigned char *,       gen_uc2d ),    \
		GENLIB_rule_expand_storage_classes(short *,               gen_s2d  ),    \
		GENLIB_rule_expand_storage_classes(unsigned short *,      gen_us2d ),    \
		GENLIB_rule_expand_storage_classes(int *,                 gen_i2d  ),    \
		GENLIB_rule_expand_storage_classes(unsigned int *,        gen_ui2d ),    \
		GENLIB_rule_expand_storage_classes(long *,                gen_l2d  ),    \
		GENLIB_rule_expand_storage_classes(unsigned long *,       gen_ul2d ),    \
		GENLIB_rule_expand_storage_classes(long long *,           gen_ll2d ),    \
		GENLIB_rule_expand_storage_classes(unsigned long long *,  gen_ull2d),    \
		GENLIB_rule_expand_storage_classes(float *,               gen_f2d  ),    \
		GENLIB_rule_expand_storage_classes(double *,              gen_d2d  ),    \
		GENLIB_rule_expand_storage_classes(long double *,         gen_ld2d ),    \
		GENLIB_rule_expand_storage_classes(complex float *,       gen_cf2d ),    \
		GENLIB_rule_expand_storage_classes(complex double *,      gen_cd2d ),    \
		GENLIB_rule_expand_storage_classes(complex long double *, gen_cld2d),    \
		default: NULL                                                            \
	);                                                                               \
})


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Generic Functions                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Rotate Generic Integer
//==========================================================================================================================================


#define gen_rotl(_v, _n)                                    \
({                                                          \
	RENAME((_v, __v), (_n, __n));                       \
	typeof(gen_cast_integer_to_unsigned(__v)) T;        \
	typeof(T) v = __v;                                  \
	typeof(T) n = __n;                                  \
	const typeof(T) mask = sizeof(v) * CHAR_BIT - 1;    \
	n &= mask;                                          \
	v = (v << n) | (v >> ((-n) & mask));                \
	v;                                                  \
})

#define gen_rotr(_v, _n)                                    \
({                                                          \
	RENAME((_v, __v), (_n, __n));                       \
	typeof(gen_cast_integer_to_unsigned(__v)) T;        \
	typeof(T) v = __v;                                  \
	typeof(T) n = __n;                                  \
	const typeof(T) mask = sizeof(v) * CHAR_BIT - 1;    \
	n &= mask;                                          \
	v = (v >> n) | (v << ((-n) & mask));                \
	v;                                                  \
})


#endif /* GENLIB_H */

