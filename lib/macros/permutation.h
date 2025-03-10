#ifndef PERMUTATION_H
#define PERMUTATION_H

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

/* Only the last unit can be incomplete.
 * The units are differentiated between:
 *     - the max multiple of number of units that are fully populated (main body)
 *           num_units = N / unit_size
 *           div = num_units / num_segments
 *     - the remaining complete units along with the last potentially incomplete one
 *           mod = num_units % num_segments
	     rem_unit_size = N - num_units * unit_size
 *
 * If 'solid_units_main' is true, then interleaving for the main body of the elements is done unit-wise, else it is done element-wise.
 * Same for 'solid_units_rem', but specifically for the remaining units.
 * If unit_size == 1 then it doesn't matter.
 *
 * Example:
 *     N = 21
 *     num_segments = 4
 *     unit_size = 2
 *     div = 2 (units per segment, main body)
 *     mod = 2 (remaining whole units)
 *     rem_unit_size = 1 (there is an incomplete last unit with size 1)
 *     original:
 *         [0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20]
 *     segments:
 *         0: [ 0  1 ,  2  3]  + [ 4  5]
 *         1: [ 6  7 ,  8  9]  + [10 11]
 *         2: [12 13 , 14 15]  + [16   ]  (-> rem_unit_size = 1)
 *         3: [17 18 , 19 20]
 *     permutation:
 *         if solid_units_main:
 *             if solid_units_rem:
 *                 [0 1 6 7 12 13 17 18 2 3 8 9 14 15 19 20] + [4  5 10 11 16]
 *             else:
 *                 [0 1 6 7 12 13 17 18 2 3 8 9 14 15 19 20] + [4 10 16  5 11]
 *         else:
 *             if solid_units_rem:
 *                 [0 6 12 17 1 7 13 18 2 8 14 19 3 9 15 20] + [4  5 10 11 16]
 *             else:
 *                 [0 6 12 17 1 7 13 18 2 8 14 19 3 9 15 20] + [4 10 16  5 11]
 */

#define permutation_interleave(_A_in, _A_out, _N, _num_segments, _unit_size, _solid_units_main, _solid_units_rem)                                                                                \
do {                                                                                                                                                                                             \
	RENAME((_A_in, A_in), (_A_out, A_out), (_N, N, long),                                                                                                                                    \
		(_num_segments, num_segments, long), (_unit_size, unit_size, long),                                                                                                              \
		(_solid_units_main, solid_units_main, long), (_solid_units_rem, solid_units_rem, long));                                                                                         \
	long segments_s[num_segments];                                                                                                                                                           \
	long num_units = N / unit_size;                                                                                                                                                          \
	long rem_unit_size = N - num_units * unit_size;                                                                                                                                          \
                                                                                                                                                                                                 \
	long div = num_units / num_segments;                                                                                                                                                     \
	long mod = num_units % num_segments;                                                                                                                                                     \
                                                                                                                                                                                                 \
	long mul = div * unit_size * num_segments;                                                                                                                                               \
                                                                                                                                                                                                 \
	long i, j, k;                                                                                                                                                                            \
                                                                                                                                                                                                 \
	/* printf("unit_size=%ld, num_segments=%ld, N=%ld, num_units=%ld, mod=%ld, rem_unit_size=%ld, div=%ld, mul=%ld\n", unit_size, num_segments, N, num_units, mod, rem_unit_size, div, mul); */    \
	for (i=0;i<num_segments;i++)                                                                                                                                                             \
	{                                                                                                                                                                                        \
		segments_s[i] = unit_size * (i * div + ((i < mod) ? i : mod));                                                                                                                   \
		if (i > mod)                                                                                                                                                                     \
			segments_s[i] += rem_unit_size;                                                                                                                                          \
		/* printf("%2ld: %2ld\n", i, segments_s[i]); */                                                                                                                                        \
	}                                                                                                                                                                                        \
                                                                                                                                                                                                 \
	for (j=0;j<mul;j++)                                                                                                                                                                      \
	{                                                                                                                                                                                        \
		long segment, segment_pos;                                                                                                                                                       \
		if (solid_units_main)                                                                                                                                                            \
		{                                                                                                                                                                                \
			segment = (j / unit_size) % num_segments;                                                                                                                                \
			segment_pos = unit_size * (j / (unit_size * num_segments)) + j % unit_size;                                                                                              \
		}                                                                                                                                                                                \
		else                                                                                                                                                                             \
		{                                                                                                                                                                                \
			segment = j % num_segments;                                                                                                                                              \
			segment_pos = j / num_segments;                                                                                                                                          \
		}                                                                                                                                                                                \
		k = segments_s[segment] + segment_pos;                                                                                                                                           \
		A_out[j] = A_in[k];                                                                                                                                                              \
	}                                                                                                                                                                                        \
                                                                                                                                                                                                 \
	/* for (i=0;i<N;i++)                                                                                                                                                                        \
		printf("%2ld, ", A_out[i]);                                                                                                                                                      \
	printf("\n"); */                                                                                                                                                                            \
                                                                                                                                                                                                 \
	for (j=mul;j<N;j++)                                                                                                                                                                      \
	{                                                                                                                                                                                        \
		long segment, segment_pos;                                                                                                                                                       \
		if (solid_units_rem)                                                                                                                                                             \
		{                                                                                                                                                                                \
			segment = (j / unit_size) % num_segments;                                                                                                                                \
			segment_pos = unit_size * (j / (unit_size * num_segments)) + j % unit_size;                                                                                              \
		}                                                                                                                                                                                \
		else                                                                                                                                                                             \
		{                                                                                                                                                                                \
			long num_segments_rem = mod + (rem_unit_size > 0 ? 1 : 0);                                                                                                               \
			segment = (j - mul) % num_segments_rem;                                                                                                                                  \
			segment_pos = mul / num_segments;                                                                                                                                        \
			segment_pos += (j - mul) / num_segments_rem;                                                                                                                             \
		}                                                                                                                                                                                \
		k = segments_s[segment] + segment_pos;                                                                                                                                           \
		A_out[j] = A_in[k];                                                                                                                                                              \
	}                                                                                                                                                                                        \
} while (0)


#endif /* PERMUTATION_H */

