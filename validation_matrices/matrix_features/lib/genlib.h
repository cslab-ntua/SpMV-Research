#ifndef GENLIB_H
#define GENLIB_H

#include <assert.h>
#include <complex.h>
#include <tgmath.h>
#include "macros/macrolib.h"
#include "debug.h"


#define GENLIB_is_ws(c)  (c == ' ' || c == '\n' || c == '\r' || c == '\t')

/* 
 * Complex numbers:
 *     The C programming language, as of C99, supports complex number math with the three built-in types double _Complex, float _Complex, and long double _Complex (see _Complex).
 *     When the header <complex.h> is included, the three complex number types are also accessible as double complex, float complex, long double complex.
 */


//==========================================================================================================================================
//= Asserts
//==========================================================================================================================================


#define gen_type_is_basic(var)                                                     \
({                                                                                 \
	_Generic((var),                                                            \
		char : 1, signed char : 1, unsigned char : 1,                      \
		short : 1, unsigned short : 1,                                     \
		int : 1, unsigned int : 1,                                         \
		long : 1, unsigned long : 1,                                       \
		long long : 1, unsigned long long : 1,                             \
		float : 1, double : 1, long double : 1,                            \
		complex float : 1, complex double : 1, complex long double : 1,    \
		default: 0                                                         \
	);                                                                         \
})


#define gen_ptr_type_is_basic(var)         \
({                                         \
	_Generic((var),                    \
		void * : 1,                \
		default: 0                 \
	) || gen_type_is_basic(*(var));    \
})


#define gen_assert_basic_type(var, ... /* error_message */)                                             \
do {                                                                                                    \
	_Static_assert(                                                                                 \
		gen_type_is_basic(var),                                                                 \
		"type of <" STRING(var) "> is not a basic type" OPT(": ", ##__VA_ARGS__) __VA_ARGS__    \
	);                                                                                              \
} while (0)


#define gen_assert_ptr_basic_type(var, ... /* error_message */)                                                    \
do {                                                                                                               \
	_Static_assert(                                                                                            \
		gen_ptr_type_is_basic(var),                                                                        \
		"type referenced by <" STRING(var) "> is not a basic type" OPT(": ", ##__VA_ARGS__) __VA_ARGS__    \
	);                                                                                                         \
} while (0)


//==========================================================================================================================================
//= Basic Type to String
//==========================================================================================================================================


#define gen_basic_type_to_string(var)                                                                                               \
({                                                                                                                                  \
	gen_assert_basic_type(var, "gen_basic_type_to_string");                                                                     \
	_Generic((var),                                                                                                             \
		char : "char", signed char : "signed char", unsigned char : "unsigned char",                                        \
		short : "short", unsigned short : "unsigned short",                                                                 \
		int : "int", unsigned int : "unsigned int",                                                                         \
		long : "long", unsigned long : "unsigned long",                                                                     \
		long long : "long long", unsigned long long : "unsigned long long",                                                 \
		float : "float", double : "double", long double : "long double",                                                    \
		complex float : "complex float", complex double : "complex double", complex long double : "complex long double",    \
		default: ""                                                                                                         \
	);                                                                                                                          \
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
GENLIB_cpy(char * src, char * dst, int n)
{
	int i = 0;
	for (i=0;i<n;i++)
		dst[i] = src[i];
}


// In tgmath.h macros are defined with names like their 'double' counterparts of math.h or complex.h.
// So we need to explicitly cast to 'double' or 'complex double' to avoid warnings when the type is different (e.g. float).

#define gen_numtostr(str, n, val)                                                                                                \
({                                                                                                                               \
	int _len = 0;                                                                                                            \
	char * _str = (str);                                                                                                     \
	__auto_type _val = (val);                                                                                                \
	gen_assert_basic_type(_val, "gen_numtostr");                                                                             \
	_len = _Generic(_val,                                                                                                    \
		char:                snprintf(_str, n, "%d"     , (char               ) _val),                                   \
		signed char:         snprintf(_str, n, "%d"     , (signed char        ) _val),                                   \
		unsigned char:       snprintf(_str, n, "%d"     , (unsigned char      ) _val),                                   \
		short:               snprintf(_str, n, "%d"     , (short              ) _val),                                   \
		unsigned short:      snprintf(_str, n, "%d"     , (unsigned short     ) _val),                                   \
		int:                 snprintf(_str, n, "%d"     , (int                ) _val),                                   \
		unsigned int:        snprintf(_str, n, "%u"     , (unsigned int       ) _val),                                   \
		long:                snprintf(_str, n, "%ld"    , (long               ) _val),                                   \
		unsigned long:       snprintf(_str, n, "%lu"    , (unsigned long      ) _val),                                   \
		long long:           snprintf(_str, n, "%lld"   , (long long          ) _val),                                   \
		unsigned long long:  snprintf(_str, n, "%llu"   , (unsigned long long ) _val),                                   \
		float:               snprintf(_str, n, "%g"     , (float              ) _val),                                   \
		double:              snprintf(_str, n, "%lg"    , (double             ) _val),                                   \
		long double:         snprintf(_str, n, "%Lg"    , (long double        ) _val),                                   \
		complex float:       snprintf(_str, n, "%g %g"  , crealf(_val), cimagf(_val)),                                   \
		complex double:      snprintf(_str, n, "%lg %lg", creal((complex double) _val), cimag((complex double)_val)),    \
		complex long double: snprintf(_str, n, "%Lg %Lg", creall(_val), cimagl(_val))                                    \
	);                                                                                                                       \
	_len;                                                                                                                    \
})


#define GENLIB_safe_strtox(strtox, str, len, endptr, ... /* base */)    \
({                                                                      \
	char _buf[len + 1];                                             \
	GENLIB_cpy((str), _buf, len);                                   \
	_buf[len] = '\0';                                               \
	errno = 0;                                                      \
	__auto_type _ret = strtox(_buf, &endptr, ##__VA_ARGS__);        \
	if (errno != 0)                                                 \
	{                                                               \
		char _buf[1000];                                        \
		gen_numtostr(_buf, sizeof(_buf), _ret);                 \
		error(STRING(strtox) ", returned value: %s", _buf);     \
	}                                                               \
	endptr = str + (endptr - _buf);                                 \
	_ret;                                                           \
})


#define gen_strtonum(str, n, var_ptr, base...)                                                                       \
({                                                                                                                   \
	__auto_type _var_ptr = var_ptr;                                                                              \
	int _len;                                                                                                    \
	char * _str = (str);                                                                                         \
	char * _ptr = _str;                                                                                          \
	char * _endptr;                                                                                              \
	gen_assert_basic_type(*_var_ptr, "gen_strtonum");                                                            \
	_ptr += GENLIB_find_non_ws(_ptr, n - (_ptr - _str));                                                         \
	_len = GENLIB_find_ws(_ptr, n - (_ptr - _str));                                                              \
	(*_var_ptr) = _Generic((_var_ptr),                                                                           \
		char *:               GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		signed char *:        GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		unsigned char *:      GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		short *:              GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		unsigned short *:     GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		int *:                GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		unsigned int *:       GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		long *:               GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		unsigned long *:      GENLIB_safe_strtox(strtol , _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		long long *:          GENLIB_safe_strtox(strtoll, _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		unsigned long long *: GENLIB_safe_strtox(strtoll, _ptr, _len, _endptr, DEFAULT_ARG_1(0, ##base)),    \
		float *:              GENLIB_safe_strtox(strtof , _ptr, _len, _endptr),                              \
		double *:             GENLIB_safe_strtox(strtod , _ptr, _len, _endptr),                              \
		long double *:        GENLIB_safe_strtox(strtold, _ptr, _len, _endptr),                              \
		complex float *: ({                                                                                  \
			float _real, _imag;                                                                          \
			_real = GENLIB_safe_strtox(strtof, _ptr, _len, _endptr);                                     \
			_ptr = _endptr;                                                                              \
			_ptr += GENLIB_find_non_ws(_ptr, n - (_ptr - _str));                                         \
			_len = GENLIB_find_ws(_ptr, n - (_ptr - _str));                                              \
			_imag = GENLIB_safe_strtox(strtof, _ptr, _len, _endptr);                                     \
			_real + _imag * I;                                                                           \
			}),                                                                                          \
		complex double *: ({                                                                                 \
			double _real, _imag;                                                                         \
			_real = GENLIB_safe_strtox(strtod, _ptr, _len, _endptr);                                     \
			_ptr = _endptr;                                                                              \
			_ptr += GENLIB_find_non_ws(_ptr, n - (_ptr - _str));                                         \
			_len = GENLIB_find_ws(_ptr, n - (_ptr - _str));                                              \
			_imag = GENLIB_safe_strtox(strtod, _ptr, _len, _endptr);                                     \
			_real + _imag * I;                                                                           \
			}),                                                                                          \
		complex long double *: ({                                                                            \
			long double _real, _imag;                                                                    \
			_real = GENLIB_safe_strtox(strtold, _ptr, _len, _endptr);                                    \
			_ptr = _endptr;                                                                              \
			_ptr += GENLIB_find_non_ws(_ptr, n - (_ptr - _str));                                         \
			_len = GENLIB_find_ws(_ptr, n - (_ptr - _str));                                              \
			_imag = GENLIB_safe_strtox(strtold, _ptr, _len, _endptr);                                    \
			_real + _imag * I;                                                                           \
			})                                                                                           \
	);                                                                                                           \
	_ptr = _endptr;                                                                                              \
	_ptr - _str;                                                                                                 \
})


//==========================================================================================================================================
//= Basic Type to Double Converter Functions
//==========================================================================================================================================


__attribute__((unused)) static double gen_c2d  (void * x, int i) { return (double)      (((char *)                x)[i]); }
__attribute__((unused)) static double gen_sc2d (void * x, int i) { return (double)      (((signed char *)         x)[i]); }
__attribute__((unused)) static double gen_uc2d (void * x, int i) { return (double)      (((unsigned char *)       x)[i]); }
__attribute__((unused)) static double gen_s2d  (void * x, int i) { return (double)      (((short *)               x)[i]); }
__attribute__((unused)) static double gen_us2d (void * x, int i) { return (double)      (((unsigned short *)      x)[i]); }
__attribute__((unused)) static double gen_i2d  (void * x, int i) { return (double)      (((int *)                 x)[i]); }
__attribute__((unused)) static double gen_ui2d (void * x, int i) { return (double)      (((unsigned int *)        x)[i]); }
__attribute__((unused)) static double gen_l2d  (void * x, int i) { return (double)      (((long *)                x)[i]); }
__attribute__((unused)) static double gen_ul2d (void * x, int i) { return (double)      (((unsigned long *)       x)[i]); }
__attribute__((unused)) static double gen_ll2d (void * x, int i) { return (double)      (((long long *)           x)[i]); }
__attribute__((unused)) static double gen_ull2d(void * x, int i) { return (double)      (((unsigned long long *)  x)[i]); }
__attribute__((unused)) static double gen_f2d  (void * x, int i) { return (double)      (((float *)               x)[i]); }
__attribute__((unused)) static double gen_d2d  (void * x, int i) { return (double)      (((double *)              x)[i]); }
__attribute__((unused)) static double gen_ld2d (void * x, int i) { return (double)      (((long double *)         x)[i]); }
__attribute__((unused)) static double gen_cf2d (void * x, int i) { return (double) cabsf(((complex float *)       x)[i]); }
__attribute__((unused)) static double gen_cd2d (void * x, int i) { return (double) cabs (((complex double *)      x)[i]); }
__attribute__((unused)) static double gen_cld2d(void * x, int i) { return (double) cabsl(((complex long double *) x)[i]); }


#define gen_basic_type_to_double_converter(var)                                        \
({                                                                                     \
	/* fail for non-simple array types */                                          \
	gen_assert_ptr_basic_type(var,                                                 \
		"for non-simple array types a <get-and-cast-to-double> "               \
		"function must be provided, i.e.:  double get_val(void * a, int i)"    \
	);                                                                             \
	_Generic((var),                                                                \
		char *:                gen_c2d  ,                                      \
		signed char *:         gen_sc2d ,                                      \
		unsigned char *:       gen_uc2d ,                                      \
		short *:               gen_s2d  ,                                      \
		unsigned short *:      gen_us2d ,                                      \
		int *:                 gen_i2d  ,                                      \
		unsigned int *:        gen_ui2d ,                                      \
		long *:                gen_l2d  ,                                      \
		unsigned long *:       gen_ul2d ,                                      \
		long long *:           gen_ll2d ,                                      \
		unsigned long long *:  gen_ull2d,                                      \
		float *:               gen_f2d  ,                                      \
		double *:              gen_d2d  ,                                      \
		long double *:         gen_ld2d ,                                      \
		complex float *:       gen_cf2d ,                                      \
		complex double *:      gen_cd2d ,                                      \
		complex long double *: gen_cld2d,                                      \
		default:               NULL                                            \
	);                                                                             \
})


#endif /* GENLIB_H */

