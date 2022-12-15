#ifndef GENLIB_H
#define GENLIB_H

#include <assert.h>
#include <complex.h>
#ifdef __cplusplus
	#define complex  _Complex
#endif
#include <tgmath.h>
#include <string.h>

#include "macros/cpp_defines.h"
#include "debug.h"
#include "macros/macrolib.h"


#define GENLIB_is_ws(c)  (c == ' ' || c == '\n' || c == '\r' || c == '\t')

/* 
 * Complex numbers:
 *     The C programming language, as of C99, supports complex number math with the three built-in types double _Complex, float _Complex, and long double _Complex (see _Complex).
 *     When the header <complex.h> is included, the three complex number types are also accessible as double complex, float complex, long double complex.
 */


#define GENLIB_rule_expand_storage_classes(type, ...)    \
	type: __VA_ARGS__,                               \
	const type: __VA_ARGS__,                         \
	volatile type: __VA_ARGS__,                      \
	volatile const type: __VA_ARGS__


#define GENLIB_rule_expand_sign(type, code...)             \
	GENLIB_rule_expand_storage_classes(type, code),    \
	GENLIB_rule_expand_storage_classes(unsigned type, code)


//==========================================================================================================================================
//= Asserts
//==========================================================================================================================================


#ifdef __cplusplus

#else

#define gen_type_equals(var, T)    \
(                                  \
	_Generic((var),            \
		typeof(T): 1,      \
		default: 0         \
	)                          \
)


#define gen_type_is_basic(var)                                                 \
(                                                                              \
	_Generic((var),                                                        \
		GENLIB_rule_expand_sign(char, 1),                              \
		GENLIB_rule_expand_storage_classes(signed char, 1),            \
		GENLIB_rule_expand_sign(short, 1),                             \
		GENLIB_rule_expand_sign(int, 1),                               \
		GENLIB_rule_expand_sign(long, 1),                              \
		GENLIB_rule_expand_sign(long long, 1),                         \
		GENLIB_rule_expand_storage_classes(float, 1),                  \
		GENLIB_rule_expand_storage_classes(double, 1),                 \
		GENLIB_rule_expand_storage_classes(long double, 1),            \
		GENLIB_rule_expand_storage_classes(complex float, 1),          \
		GENLIB_rule_expand_storage_classes(complex double, 1),         \
		GENLIB_rule_expand_storage_classes(complex long double, 1),    \
		default: 0                                                     \
	)                                                                      \
)


#define gen_ptr_type_is_basic(var)        \
(                                         \
	_Generic((var),                   \
		void * : 1,               \
		default: 0                \
	) || gen_type_is_basic(*(var))    \
)


#define gen_assert_basic_type(var, ... /* error_message */)                                             \
do {                                                                                                    \
	_Static_assert(                                                                                 \
		gen_type_is_basic(var),                                                                 \
		"Type of <" STRING(var) "> is not a basic type" OPT(": ", ##__VA_ARGS__) __VA_ARGS__    \
	);                                                                                              \
} while (0)


#define gen_assert_ptr_basic_type(var, ... /* error_message */)                                                    \
do {                                                                                                               \
	_Static_assert(                                                                                            \
		gen_ptr_type_is_basic(var),                                                                        \
		"Type referenced by <" STRING(var) "> is not a basic type" OPT(": ", ##__VA_ARGS__) __VA_ARGS__    \
	);                                                                                                         \
} while (0)


#define gen_assert_type(var, type, var_name)                                \
do {                                                                        \
	_Static_assert(                                                     \
		gen_type_equals(var, type),                                 \
		"Type of <" STRING(var_name) "> should be " STRING(type)    \
	);                                                                  \
} while (0)

#endif /* __cplusplus */


//==========================================================================================================================================
//= Basic Type to String
//==========================================================================================================================================


#ifdef __cplusplus

	#include <typeinfo>

	#define gen_basic_type_name_to_string(var)  (typeid(typeof(var)).name())

#else

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

#define GENLIB_rule_pointer_string_sign(type)    \
	GENLIB_rule_type_string_sign(type),      \
	GENLIB_rule_type_string_sign(type *)


#define gen_basic_type_name_to_string(var)                                          \
(                                                                                   \
	_Generic((var),                                                             \
		GENLIB_rule_pointer_string_sign(char),                              \
		GENLIB_rule_pointer_string_storage_classes(signed char),            \
		GENLIB_rule_pointer_string_sign(short),                             \
		GENLIB_rule_pointer_string_sign(int),                               \
		GENLIB_rule_pointer_string_sign(long),                              \
		GENLIB_rule_pointer_string_sign(long long),                         \
		GENLIB_rule_pointer_string_storage_classes(float),                  \
		GENLIB_rule_pointer_string_storage_classes(double),                 \
		GENLIB_rule_pointer_string_storage_classes(long double),            \
		GENLIB_rule_pointer_string_storage_classes(complex float),          \
		GENLIB_rule_pointer_string_storage_classes(complex double),         \
		GENLIB_rule_pointer_string_storage_classes(complex long double),    \
		default: ""                                                         \
	)                                                                           \
)

#endif /* __cplusplus */


//==========================================================================================================================================
//= Number to String
//==========================================================================================================================================


#ifdef __cplusplus

	#include <iostream>
	#include <sstream>

	/* #define gen_numtostr(_str, N, val)              \
	({                                              \
		std::stringstream _ss;                  \
		std::string _s;                         \
		_ss << val;                             \
		_ss >> _s;                              \
		snprintf(_str, N, "%s", _s.c_str());    \
		_s.length();                            \
	}) */

	#define gen_numtostr(_str, N, val)               \
	({                                               \
		std::string _s = std::to_string(val);    \
		snprintf(_str, N, "%s", _s.c_str());     \
		_s.length();                             \
	})

#else

#define gen_numtostr(str, N, val)                                                                                                                                    \
({                                                                                                                                                                   \
	int _len = 0;                                                                                                                                                \
	char * _str = (str);                                                                                                                                         \
	__auto_type _val = (val);                                                                                                                                    \
	gen_assert_basic_type(_val, "gen_numtostr");                                                                                                                 \
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

#endif /* __cplusplus */


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


#ifdef __cplusplus

	#include <iostream>
	#include <sstream>
	// #include <charconv>

	#define gen_strtonum(__str, N, var_ptr, base...)                 \
	({                                                               \
		__auto_type _N = N;                                      \
		__auto_type _var_ptr = var_ptr;                          \
		int _ws_len, _len, _total_len;                           \
		char * _str = (__str);                                   \
		char * _ptr = _str;                                      \
		auto _num = *_var_ptr;                                   \
		_ptr += GENLIB_find_non_ws(_ptr, _N - (_ptr - _str));    \
		_len = GENLIB_find_ws(_ptr, _N - (_ptr - _str));         \
		_ws_len = _ptr - _str;                                   \
		_total_len = _len + _ws_len;                             \
		std::stringstream _ss(_ptr);                             \
		if (_len > 0)                                            \
		{                                                        \
			if (_ptr[0] == '0')                              \
			{                                                \
				if ((_len > 1) && (_ptr[1] == 'x'))      \
				{                                        \
					_ss.seekp(_ws_len + 2);          \
					_ss >> std::hex >> _num;         \
				}                                        \
				else                                     \
					_ss >> std::oct >> _num;         \
			}                                                \
			else                                             \
				_ss >> _num;                             \
			*_var_ptr = _num;                                \
		}                                                        \
		_total_len;                                              \
	})

	/* #define gen_strtonum(__str, N, var_ptr, base...)    \
	({                                                  \
		auto _str = __str;                          \
		auto _var_ptr = var_ptr;                    \
		auto _num = *_var_ptr;                      \
		std::stringstream _ss(_str);                \
		std::string _rem;                           \
		_ss >> _num;                                \
		getline(_ss, _rem, '\0');                   \
		*_var_ptr = _num;                           \
		(N-1) - _rem.length();                      \
	}) */

	/* #define gen_strtonum(_str, N, var_ptr, base...)        \
	({                                                     \
		auto _str = __str;                             \
		auto _var_ptr = var_ptr;                       \
		auto _num = *_var_ptr;                         \
		std::from_chars_result _ret;                   \
		_ret = std::from_chars(_str, _str+N, _num);    \
		*_var_ptr = _num;                              \
		_ret.ptr - _str;                               \
	}) */

#else

// In tgmath.h macros are defined with names like their 'double' counterparts of math.h or complex.h.
// So we need to explicitly cast to 'double' or 'complex double' to avoid warnings when the type is different (e.g. float).

#define GENLIB_safe_strtox(strtox, str, len, endptr, ... /* base */)    \
({                                                                      \
	char _buf[len + 1];                                             \
	/* GENLIB_cpy((str), _buf, len); */                             \
	strncpy(_buf, (str), len);                                      \
	_buf[len] = '\0';                                               \
	errno = 0;                                                      \
	__auto_type _ret = strtox(_buf, &endptr, ##__VA_ARGS__);        \
	if (errno != 0)                                                 \
		error(STRING(strtox));                                  \
	endptr = str + (endptr - _buf);                                 \
	_ret;                                                           \
})


#define gen_strtonum(str, N, var_ptr, base...)                                                                                                           \
({                                                                                                                                                       \
	__auto_type _N = N;                                                                                                                              \
	__auto_type _var_ptr = var_ptr;                                                                                                                  \
	int _len;                                                                                                                                        \
	char * _str = (str);                                                                                                                             \
	char * _ptr = _str;                                                                                                                              \
	char * _endptr;                                                                                                                                  \
	gen_assert_basic_type(*_var_ptr, "gen_strtonum");                                                                                                \
	_ptr += GENLIB_find_non_ws(_ptr, _N - (_ptr - _str));                                                                                            \
	_len = GENLIB_find_ws(_ptr, _N - (_ptr - _str));                                                                                                 \
	(*_var_ptr) = _Generic((_var_ptr),                                                                                                               \
		GENLIB_rule_expand_sign(char *,                          GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_storage_classes(signed char *,        GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_sign(short *,                         GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_sign(int *,                           GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_sign(long *,                          GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_sign(long long *,                     GENLIB_safe_strtox(strtoll, _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base))),    \
		GENLIB_rule_expand_storage_classes(float *,              GENLIB_safe_strtox(strtof , _ptr, _len, _endptr)),                              \
		GENLIB_rule_expand_storage_classes(double *,             GENLIB_safe_strtox(strtod , _ptr, _len, _endptr)),                              \
		GENLIB_rule_expand_storage_classes(long double *,        GENLIB_safe_strtox(strtold, _ptr, _len, _endptr)),                              \
		GENLIB_rule_expand_storage_classes(complex float *, ({                                                                                   \
			float _real, _imag;                                                                                                              \
			_real = GENLIB_safe_strtox(strtof, _ptr, _len, _endptr);                                                                         \
			_ptr = _endptr;                                                                                                                  \
			_ptr += GENLIB_find_non_ws(_ptr, _N - (_ptr - _str));                                                                            \
			_len = GENLIB_find_ws(_ptr, _N - (_ptr - _str));                                                                                 \
			_imag = GENLIB_safe_strtox(strtof, _ptr, _len, _endptr);                                                                         \
			_real + _imag * I;                                                                                                               \
			})),                                                                                                                             \
		GENLIB_rule_expand_storage_classes(complex double *, ({                                                                                  \
			double _real, _imag;                                                                                                             \
			_real = GENLIB_safe_strtox(strtod, _ptr, _len, _endptr);                                                                         \
			_ptr = _endptr;                                                                                                                  \
			_ptr += GENLIB_find_non_ws(_ptr, _N - (_ptr - _str));                                                                            \
			_len = GENLIB_find_ws(_ptr, _N - (_ptr - _str));                                                                                 \
			_imag = GENLIB_safe_strtox(strtod, _ptr, _len, _endptr);                                                                         \
			_real + _imag * I;                                                                                                               \
			})),                                                                                                                             \
		GENLIB_rule_expand_storage_classes(complex long double *, ({                                                                             \
			long double _real, _imag;                                                                                                        \
			_real = GENLIB_safe_strtox(strtold, _ptr, _len, _endptr);                                                                        \
			_ptr = _endptr;                                                                                                                  \
			_ptr += GENLIB_find_non_ws(_ptr, _N - (_ptr - _str));                                                                            \
			_len = GENLIB_find_ws(_ptr, _N - (_ptr - _str));                                                                                 \
			_imag = GENLIB_safe_strtox(strtold, _ptr, _len, _endptr);                                                                        \
			_real + _imag * I;                                                                                                               \
			}))                                                                                                                              \
	);                                                                                                                                               \
	_ptr = _endptr;                                                                                                                                  \
	_ptr - _str;                                                                                                                                     \
})

#endif /* __cplusplus */


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                       Generics Over Doubles                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==========================================================================================================================================
//= Basic Type to Double Converter Functions
//==========================================================================================================================================


#ifdef __cplusplus

	#define gen_functor_basic_type_to_double(var_ptr)  ([](void * x, long i) -> double { return (double) (((typeof(var_ptr)) x)[i]); })

#else

static inline double gen_c2d  (void * x, long i) { return (double)      (((char *)                x)[i]); }
static inline double gen_sc2d (void * x, long i) { return (double)      (((signed char *)         x)[i]); }
static inline double gen_uc2d (void * x, long i) { return (double)      (((unsigned char *)       x)[i]); }
static inline double gen_s2d  (void * x, long i) { return (double)      (((short *)               x)[i]); }
static inline double gen_us2d (void * x, long i) { return (double)      (((unsigned short *)      x)[i]); }
static inline double gen_i2d  (void * x, long i) { return (double)      (((int *)                 x)[i]); }
static inline double gen_ui2d (void * x, long i) { return (double)      (((unsigned int *)        x)[i]); }
static inline double gen_l2d  (void * x, long i) { return (double)      (((long *)                x)[i]); }
static inline double gen_ul2d (void * x, long i) { return (double)      (((unsigned long *)       x)[i]); }
static inline double gen_ll2d (void * x, long i) { return (double)      (((long long *)           x)[i]); }
static inline double gen_ull2d(void * x, long i) { return (double)      (((unsigned long long *)  x)[i]); }
static inline double gen_f2d  (void * x, long i) { return (double)      (((float *)               x)[i]); }
static inline double gen_d2d  (void * x, long i) { return (double)      (((double *)              x)[i]); }
static inline double gen_ld2d (void * x, long i) { return (double)      (((long double *)         x)[i]); }
static inline double gen_cf2d (void * x, long i) { return (double) cabsf(((complex float *)       x)[i]); }
static inline double gen_cd2d (void * x, long i) { return (double) cabs (((complex double *)      x)[i]); }
static inline double gen_cld2d(void * x, long i) { return (double) cabsl(((complex long double *) x)[i]); }

#define gen_functor_basic_type_to_double(var_ptr)                                        \
({                                                                                       \
	/* fail for non-simple array types */                                            \
	gen_assert_ptr_basic_type((var_ptr),                                             \
		"for non-simple array types a <get-and-cast-to-double> "                 \
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

#endif /* __cplusplus */


#endif /* GENLIB_H */

