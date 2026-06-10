#ifndef PIXEL_ARRAY_H
#define PIXEL_ARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <endian.h>
#include <omp.h>

#include "macros/cpp_defines.h"
#include "debug.h"
#include "io.h"
#include "string_util.h"
#include "parallel_io.h"


// A pixel is represented as a triplet (red,green,blue).

struct Pixel_8 {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};


struct Pixel_16 {
	uint16_t r;
	uint16_t g;
	uint16_t b;
};


struct Pixel_Array {
	int width;
	int height;
	int max_value;
	void * pixels;  // The 2D pixel array, first element is the top-left pixel.
	char * locks;
};


static inline
void
pixel_array_init(struct Pixel_Array * pa, int width, int height, int max_value)
{
	int pixel_size = (max_value > 255) ? sizeof(struct Pixel_16) : sizeof(struct Pixel_8);
	pa->width = width;
	pa->height = height;
	pa->max_value = max_value;
	pa->pixels = malloc(width * height * pixel_size);
	pa->locks = malloc(width * height * sizeof(*(pa->locks)));
}


static inline
void
pixel_array_clean(struct Pixel_Array * pa)
{
	if (pa == NULL)
		return;
	free(pa->pixels);
	pa->pixels = NULL;
	free(pa->locks);
	pa->locks = NULL;
}


static inline
void
pixel_array_destroy(struct Pixel_Array ** pa_ptr)
{
	pixel_array_clean(*pa_ptr);
	free(*pa_ptr);
	*pa_ptr = NULL;
}


static inline
void
pixel_array_reset_locks_serial(struct Pixel_Array * pa)
{
	long n = pa->width * pa->height;
	long i;
	for (i=0;i<n;i++)
	{
		__atomic_store_n(&(pa->locks[i]), 0, __ATOMIC_RELAXED);
	}
}

static inline
void
pixel_array_reset_locks_concurrent(struct Pixel_Array * pa)
{
	long n = pa->width * pa->height;
	long i;
	#pragma omp for
	for (i=0;i<n;i++)
	{
		__atomic_store_n(&(pa->locks[i]), 0, __ATOMIC_RELAXED);
	}
}

static inline
void
pixel_array_reset_locks(struct Pixel_Array * pa)
{
	if (omp_get_level() > 0)
	{
		pixel_array_reset_locks_serial(pa);
	}
	else
	{
		_Pragma("omp parallel")
		{
			pixel_array_reset_locks_concurrent(pa);
		}
	}
}


static inline
long
pixel_array_pixel_is_locked(struct Pixel_Array * pa, long pos)
{
	return __atomic_load_n(&(pa->locks[pos]), __ATOMIC_RELAXED) == 1;
}


static inline
long
pixel_array_try_lock_pixel(struct Pixel_Array * pa, long pos)
{
	if (__atomic_exchange_n(&(pa->locks[pos]), 1, __ATOMIC_ACQUIRE))
		return 0;
	return 1;
}


#define pixel_array_fill_gen(T, pa, _r, _g, _b)               \
do {                                                          \
	_Pragma("omp parallel")                               \
	{                                                     \
		T * pixels = pa->pixels;                      \
		T * p;                                        \
		int i, j, pix_pos;                            \
		_Pragma("omp for")                            \
		for (i=0;i<pa->height;i++)                    \
			for (j=0;j<pa->width;j++)             \
			{                                     \
				pix_pos = pa->width*i + j;    \
				p = &pixels[pix_pos];         \
				p->r = _r;                    \
				p->g = _g;                    \
				p->b = _b;                    \
			}                                     \
	}                                                     \
} while (0)

static inline
void
pixel_array_fill(struct Pixel_Array * pa, int16_t r, int16_t g, int16_t b)
{
	if (pa->max_value > 255)
		pixel_array_fill_gen(struct Pixel_16, pa, r, g, b);
	else
		pixel_array_fill_gen(struct Pixel_8, pa, r, g, b);
}


#endif /* PIXEL_ARRAY_H */

