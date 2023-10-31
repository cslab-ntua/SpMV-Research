#ifndef PPM_H
#define PPM_H

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
	void * pixels;  // Actually a 2D array.
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
pixel_array_reset_locks(struct Pixel_Array * pa)
{
	long n = pa->width * pa->height;
	long i;
	#pragma omp parallel
	{
		#pragma omp for
		for (i=0;i<n;i++)
		{
			__atomic_store_n(&(pa->locks[i]), 0, __ATOMIC_RELAXED);
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
pixel_array_lock_pixel(struct Pixel_Array * pa, long pos)
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
		_Pragma("omp for schedule(static)")           \
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


static inline
int
next_token(char * str, int len)
{
	int i = 0;
	while (i < len)
	{
		i += str_next_word(str+i, len-i);
		if (str[i] == '#')
			i += str_find_eol(str+i, len-i);
		else
			break;
	}
	return i;
}


#define load_pixel(p, buf, pos)                                       \
do {                                                                  \
	_Generic((p),                                                 \
		struct Pixel_16 *:                                    \
		({                                                    \
			p->r = be16toh(((uint16_t *) buf)[pos]);      \
			p->g = be16toh(((uint16_t *) buf)[pos+1]);    \
			p->b = be16toh(((uint16_t *) buf)[pos+2]);    \
		}),                                                   \
		struct Pixel_8 *:                                     \
		({                                                    \
			p->r = buf[pos];                              \
			p->g = buf[pos+1];                            \
			p->b = buf[pos+2];                            \
		})                                                    \
	);                                                            \
} while (0)


#define store_pixel(p, buf, pos)                                      \
do {                                                                  \
	_Generic((p),                                                 \
		struct Pixel_16 *:                                    \
		({                                                    \
			((uint16_t *) buf)[pos] = htobe16(p->r);      \
			((uint16_t *) buf)[pos+1] = htobe16(p->g);    \
			((uint16_t *) buf)[pos+2] = htobe16(p->b);    \
		}),                                                   \
		struct Pixel_8 *:                                     \
		({                                                    \
			buf[pos]   = p->r;                            \
			buf[pos+1] = p->g;                            \
			buf[pos+2] = p->b;                            \
		})                                                    \
	);                                                            \
} while (0)


#define load_pbm_image_gen(T, pa)                            \
do {                                                         \
	_Pragma("omp parallel")                              \
	{                                                    \
		T * pixels = pa->pixels;                     \
		T * p;                                       \
		int i, j, pix_pos, pos;                      \
		_Pragma("omp for schedule(static)")          \
		for (i=0;i<height;i++)                       \
			for (j=0;j<width;j++)                \
			{                                    \
				pix_pos = width*i + j;       \
				pos = 3 * pix_pos;           \
				p = &pixels[pix_pos];        \
				load_pixel(p, data, pos);    \
			}                                    \
	}                                                    \
} while (0)

/*
 * Loads an image in regular ppm format as a pixel array,
 * with each pixel represented as 3 2-byte integers (rgb), MSB first.
 */
static __attribute__((unused))
struct Pixel_Array *
load_pbm_image(char * filename)
{
	struct File_Atoms * A;
	int width, height, max_value;
	char * data;
	int len;
	int pos;

	struct Pixel_Array * pa;

	A = (typeof(A)) malloc(sizeof(*A));
	file_to_raw_data(A, filename);
	data = A->string;
	len = (int) A->len;

	pos = 0;

	// Tokens are separated by whitespace (blanks, TABs, CRs, LFs).
	char magic_num[2] = {data[0], data[1]};            // A "magic number" for identifying the file type. A ppm image's magic number is the two characters "P6".
	if (magic_num[0] != 'P' || magic_num[1] != '6')
		error("Wrong file format.");
	pos += next_token(data + pos, len - pos);          // The width, formatted as ASCII characters in decimal.
	width = atoi(data + pos);
	pos += next_token(data + pos, len - pos);          // The height, again in ASCII decimal.
	height = atoi(data + pos);
	pos += next_token(data + pos, len - pos);          // The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536 and more than zero.
	max_value = atoi(data + pos);
	pos += str_find_ws(data + pos, len - pos);
	pos += 1;                                          // A single whitespace character (usually a newline).

	data = data + pos;

	pa = malloc(sizeof(*pa));
	pixel_array_init(pa, width, height, max_value);

	if (max_value > 255)
		load_pbm_image_gen(struct Pixel_16, pa);
	else
		load_pbm_image_gen(struct Pixel_8, pa);

	file_atoms_destroy(&A);
	return pa;
}


#define save_pbm_image_gen(T, pa)                             \
do {                                                          \
	_Pragma("omp parallel")                               \
	{                                                     \
		T * pixels = pa->pixels;                      \
		T * p;                                        \
		int i, j, pix_pos, pos;                       \
		_Pragma("omp for schedule(static)")           \
		for (i=0;i<pa->height;i++)                    \
			for (j=0;j<pa->width;j++)             \
			{                                     \
				pix_pos = pa->width*i + j;    \
				pos = 3 * pix_pos;            \
				p = &pixels[pix_pos];         \
				store_pixel(p, data, pos);    \
			}                                     \
	}                                                     \
} while (0)


/*
 * Saves the image in regular pbm format.
 */
static __attribute__((unused))
void
save_pbm_image(struct Pixel_Array * pa, int fd)
{
	char buf[1000];
	char * data;
	int size = 3 * pa->height * pa->width;
	int len;

	if (pa->max_value > 255)
		size *= 2;
	data = malloc(size);

	// fprintf(stderr, "w:%d h:%d max:%d size:%d\n", pa->width, pa->height, pa->max_value, size);
	len = snprintf(buf, sizeof(buf), "P6\n");
	safe_write(fd, buf, len);
	len = snprintf(buf, sizeof(buf), "%d %d %d\n", pa->width, pa->height, pa->max_value);
	safe_write(fd, buf, len);
	if (pa->max_value > 255)
		save_pbm_image_gen(struct Pixel_16, pa);
	else
		save_pbm_image_gen(struct Pixel_8, pa);
	safe_write(fd, data, size);
	free(data);
}


/*
 * Print the image in (r g b) values.
 */
static __attribute__((unused))
void
print_pbm_image(struct Pixel_Array * pa)
{
	struct Pixel_8 * pixels_8, * p8;
	struct Pixel_16 * pixels_16, * p16;
	int i, j;
	printf("P6\n");
	printf("%d %d %d\n", pa->width, pa->height, pa->max_value);
	fflush(stdout);
	for (i=0;i<pa->height;i++)
	{
		for (j=0;j<pa->width;j++)
		{
			if (pa->max_value > 255)
			{
				pixels_16 = pa->pixels;
				p16 = &pixels_16[pa->width*i + j];
				printf("(%d %d %d) ", p16->r, p16->g, p16->b);
			}
			else
			{
				pixels_8 = pa->pixels;
				p8 = &pixels_8[pa->width*i + j];
				printf("(%d %d %d) ", p8->r, p8->g, p8->b);
			}
		}
		printf("\n");
	}
}


#endif /* PPM_H */

