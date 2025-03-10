#ifndef VECTORIZATION_UTIL_H
#define VECTORIZATION_UTIL_H

#include "macros/cpp_defines.h"


#define vec_x2_128  256
#define vec_x2_64   128
#define vec_x2_32   64
#define vec_x2_16   32
#define vec_x2_8    16
#define vec_x2_4    8
#define vec_x2_2    4
#define vec_x2_1    2

#define vec_x2_size(size)  CONCAT(vec_x2_, size)



#define vec_loop_expr(type, N, tmp, iter, code)    \
({                                                 \
	type tmp;                                  \
	for (long iter=0;iter<N;iter++)            \
	{                                          \
		code                               \
	}                                          \
	tmp;                                       \
})


#define vec_loop_stmt(N, iter, code)       \
do {                                       \
	for (long iter=0;iter<N;iter++)    \
	{                                  \
		code                       \
	}                                  \
} while (0)


#define vec_reduce_expr(type, N, zero, tmp, iter, code)    \
({                                                         \
	type tmp = zero;                                   \
	for (long iter=0;iter<N;iter++)                    \
	{                                                  \
		code                                       \
	}                                                  \
	tmp;                                               \
})



#define vec_iter_expr_128(iter, base, expr)                                                                                                    \
	({long iter=127+(base); expr;}), ({long iter=126+(base); expr;}), ({long iter=125+(base); expr;}), ({long iter=124+(base); expr;}),    \
	({long iter=123+(base); expr;}), ({long iter=122+(base); expr;}), ({long iter=121+(base); expr;}), ({long iter=120+(base); expr;}),    \
	({long iter=119+(base); expr;}), ({long iter=118+(base); expr;}), ({long iter=117+(base); expr;}), ({long iter=116+(base); expr;}),    \
	({long iter=115+(base); expr;}), ({long iter=114+(base); expr;}), ({long iter=113+(base); expr;}), ({long iter=112+(base); expr;}),    \
	({long iter=111+(base); expr;}), ({long iter=110+(base); expr;}), ({long iter=109+(base); expr;}), ({long iter=108+(base); expr;}),    \
	({long iter=107+(base); expr;}), ({long iter=106+(base); expr;}), ({long iter=105+(base); expr;}), ({long iter=104+(base); expr;}),    \
	({long iter=103+(base); expr;}), ({long iter=102+(base); expr;}), ({long iter=101+(base); expr;}), ({long iter=100+(base); expr;}),    \
	({long iter= 99+(base); expr;}), ({long iter= 98+(base); expr;}), ({long iter= 97+(base); expr;}), ({long iter= 96+(base); expr;}),    \
	({long iter= 95+(base); expr;}), ({long iter= 94+(base); expr;}), ({long iter= 93+(base); expr;}), ({long iter= 92+(base); expr;}),    \
	({long iter= 91+(base); expr;}), ({long iter= 90+(base); expr;}), ({long iter= 89+(base); expr;}), ({long iter= 88+(base); expr;}),    \
	({long iter= 87+(base); expr;}), ({long iter= 86+(base); expr;}), ({long iter= 85+(base); expr;}), ({long iter= 84+(base); expr;}),    \
	({long iter= 83+(base); expr;}), ({long iter= 82+(base); expr;}), ({long iter= 81+(base); expr;}), ({long iter= 80+(base); expr;}),    \
	({long iter= 79+(base); expr;}), ({long iter= 78+(base); expr;}), ({long iter= 77+(base); expr;}), ({long iter= 76+(base); expr;}),    \
	({long iter= 75+(base); expr;}), ({long iter= 74+(base); expr;}), ({long iter= 73+(base); expr;}), ({long iter= 72+(base); expr;}),    \
	({long iter= 71+(base); expr;}), ({long iter= 70+(base); expr;}), ({long iter= 69+(base); expr;}), ({long iter= 68+(base); expr;}),    \
	({long iter= 67+(base); expr;}), ({long iter= 66+(base); expr;}), ({long iter= 65+(base); expr;}), ({long iter= 64+(base); expr;}),    \
	({long iter= 63+(base); expr;}), ({long iter= 62+(base); expr;}), ({long iter= 61+(base); expr;}), ({long iter= 60+(base); expr;}),    \
	({long iter= 59+(base); expr;}), ({long iter= 58+(base); expr;}), ({long iter= 57+(base); expr;}), ({long iter= 56+(base); expr;}),    \
	({long iter= 55+(base); expr;}), ({long iter= 54+(base); expr;}), ({long iter= 53+(base); expr;}), ({long iter= 52+(base); expr;}),    \
	({long iter= 51+(base); expr;}), ({long iter= 50+(base); expr;}), ({long iter= 49+(base); expr;}), ({long iter= 48+(base); expr;}),    \
	({long iter= 47+(base); expr;}), ({long iter= 46+(base); expr;}), ({long iter= 45+(base); expr;}), ({long iter= 44+(base); expr;}),    \
	({long iter= 43+(base); expr;}), ({long iter= 42+(base); expr;}), ({long iter= 41+(base); expr;}), ({long iter= 40+(base); expr;}),    \
	({long iter= 39+(base); expr;}), ({long iter= 38+(base); expr;}), ({long iter= 37+(base); expr;}), ({long iter= 36+(base); expr;}),    \
	({long iter= 35+(base); expr;}), ({long iter= 34+(base); expr;}), ({long iter= 33+(base); expr;}), ({long iter= 32+(base); expr;}),    \
	({long iter= 31+(base); expr;}), ({long iter= 30+(base); expr;}), ({long iter= 29+(base); expr;}), ({long iter= 28+(base); expr;}),    \
	({long iter= 27+(base); expr;}), ({long iter= 26+(base); expr;}), ({long iter= 25+(base); expr;}), ({long iter= 24+(base); expr;}),    \
	({long iter= 23+(base); expr;}), ({long iter= 22+(base); expr;}), ({long iter= 21+(base); expr;}), ({long iter= 20+(base); expr;}),    \
	({long iter= 19+(base); expr;}), ({long iter= 18+(base); expr;}), ({long iter= 17+(base); expr;}), ({long iter= 16+(base); expr;}),    \
	({long iter= 15+(base); expr;}), ({long iter= 14+(base); expr;}), ({long iter= 13+(base); expr;}), ({long iter= 12+(base); expr;}),    \
	({long iter= 11+(base); expr;}), ({long iter= 10+(base); expr;}), ({long iter=  9+(base); expr;}), ({long iter=  8+(base); expr;}),    \
	({long iter=  7+(base); expr;}), ({long iter=  6+(base); expr;}), ({long iter=  5+(base); expr;}), ({long iter=  4+(base); expr;}),    \
	({long iter=  3+(base); expr;}), ({long iter=  2+(base); expr;}), ({long iter=  1+(base); expr;}), ({long iter=  0+(base); expr;})

#define vec_iter_expr_64(iter, base, expr)                                                                                                 \
	({long iter=63+(base); expr;}), ({long iter=62+(base); expr;}), ({long iter=61+(base); expr;}), ({long iter=60+(base); expr;}),    \
	({long iter=59+(base); expr;}), ({long iter=58+(base); expr;}), ({long iter=57+(base); expr;}), ({long iter=56+(base); expr;}),    \
	({long iter=55+(base); expr;}), ({long iter=54+(base); expr;}), ({long iter=53+(base); expr;}), ({long iter=52+(base); expr;}),    \
	({long iter=51+(base); expr;}), ({long iter=50+(base); expr;}), ({long iter=49+(base); expr;}), ({long iter=48+(base); expr;}),    \
	({long iter=47+(base); expr;}), ({long iter=46+(base); expr;}), ({long iter=45+(base); expr;}), ({long iter=44+(base); expr;}),    \
	({long iter=43+(base); expr;}), ({long iter=42+(base); expr;}), ({long iter=41+(base); expr;}), ({long iter=40+(base); expr;}),    \
	({long iter=39+(base); expr;}), ({long iter=38+(base); expr;}), ({long iter=37+(base); expr;}), ({long iter=36+(base); expr;}),    \
	({long iter=35+(base); expr;}), ({long iter=34+(base); expr;}), ({long iter=33+(base); expr;}), ({long iter=32+(base); expr;}),    \
	({long iter=31+(base); expr;}), ({long iter=30+(base); expr;}), ({long iter=29+(base); expr;}), ({long iter=28+(base); expr;}),    \
	({long iter=27+(base); expr;}), ({long iter=26+(base); expr;}), ({long iter=25+(base); expr;}), ({long iter=24+(base); expr;}),    \
	({long iter=23+(base); expr;}), ({long iter=22+(base); expr;}), ({long iter=21+(base); expr;}), ({long iter=20+(base); expr;}),    \
	({long iter=19+(base); expr;}), ({long iter=18+(base); expr;}), ({long iter=17+(base); expr;}), ({long iter=16+(base); expr;}),    \
	({long iter=15+(base); expr;}), ({long iter=14+(base); expr;}), ({long iter=13+(base); expr;}), ({long iter=12+(base); expr;}),    \
	({long iter=11+(base); expr;}), ({long iter=10+(base); expr;}), ({long iter= 9+(base); expr;}), ({long iter= 8+(base); expr;}),    \
	({long iter= 7+(base); expr;}), ({long iter= 6+(base); expr;}), ({long iter= 5+(base); expr;}), ({long iter= 4+(base); expr;}),    \
	({long iter= 3+(base); expr;}), ({long iter= 2+(base); expr;}), ({long iter= 1+(base); expr;}), ({long iter= 0+(base); expr;})

#define vec_iter_expr_32(iter, base, expr)                                                                                                 \
	({long iter=31+(base); expr;}), ({long iter=30+(base); expr;}), ({long iter=29+(base); expr;}), ({long iter=28+(base); expr;}),    \
	({long iter=27+(base); expr;}), ({long iter=26+(base); expr;}), ({long iter=25+(base); expr;}), ({long iter=24+(base); expr;}),    \
	({long iter=23+(base); expr;}), ({long iter=22+(base); expr;}), ({long iter=21+(base); expr;}), ({long iter=20+(base); expr;}),    \
	({long iter=19+(base); expr;}), ({long iter=18+(base); expr;}), ({long iter=17+(base); expr;}), ({long iter=16+(base); expr;}),    \
	({long iter=15+(base); expr;}), ({long iter=14+(base); expr;}), ({long iter=13+(base); expr;}), ({long iter=12+(base); expr;}),    \
	({long iter=11+(base); expr;}), ({long iter=10+(base); expr;}), ({long iter= 9+(base); expr;}), ({long iter= 8+(base); expr;}),    \
	({long iter= 7+(base); expr;}), ({long iter= 6+(base); expr;}), ({long iter= 5+(base); expr;}), ({long iter= 4+(base); expr;}),    \
	({long iter= 3+(base); expr;}), ({long iter= 2+(base); expr;}), ({long iter= 1+(base); expr;}), ({long iter= 0+(base); expr;})

#define vec_iter_expr_16(iter, base, expr)                                                                                                 \
	({long iter=15+(base); expr;}), ({long iter=14+(base); expr;}), ({long iter=13+(base); expr;}), ({long iter=12+(base); expr;}),    \
	({long iter=11+(base); expr;}), ({long iter=10+(base); expr;}), ({long iter= 9+(base); expr;}), ({long iter= 8+(base); expr;}),    \
	({long iter= 7+(base); expr;}), ({long iter= 6+(base); expr;}), ({long iter= 5+(base); expr;}), ({long iter= 4+(base); expr;}),    \
	({long iter= 3+(base); expr;}), ({long iter= 2+(base); expr;}), ({long iter= 1+(base); expr;}), ({long iter= 0+(base); expr;})

#define vec_iter_expr_8(iter, base, expr)                                                                                              \
	({long iter=7+(base); expr;}), ({long iter=6+(base); expr;}), ({long iter=5+(base); expr;}), ({long iter=4+(base); expr;}),    \
        ({long iter=3+(base); expr;}), ({long iter=2+(base); expr;}), ({long iter=1+(base); expr;}), ({long iter=0+(base); expr;})

#define vec_iter_expr_4(iter, base, expr)    \
	({long iter=3+(base); expr;}), ({long iter=2+(base); expr;}), ({long iter=1+(base); expr;}), ({long iter=0+(base); expr;})

#define vec_iter_expr_2(iter, base, expr)    \
	({long iter=1+(base); expr;}), ({long iter=0+(base); expr;})

#define vec_iter_expr_1(iter, base, expr)    \
	({long iter=0+(base); expr;})


#endif /* VECTORIZATION_UTIL_H */

