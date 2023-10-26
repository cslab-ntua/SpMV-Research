#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <complex.h>
#ifdef __cplusplus
	#define complex  _Complex
#endif
#include <math.h>

#include "macros/macrolib.h"
#include "debug.h"
#include "io.h"
#include "parallel_io.h"
#include "genlib.h"
#include "array_metrics.h"
#include "storage_formats/ppm.h"

#include "plot.h"
#include "legend.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Utilities                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static
double
id(__attribute__((unused)) void * x, long i)
{
	return (double) i;
}


static inline
double
normal_distribution(double m, double s, double x)
{
	static const double c = sqrt(2 * M_PI);
	double y, tmp;
	tmp = (x - m) / s;
	y = exp(tmp*tmp / -2);
	// y = ldexp(1, tmp*tmp / -2 + 0.5);
	y /= s * c;
	return y;
}


//==========================================================================================================================================
//= Write And Convert Image File
//==========================================================================================================================================


static
void
write_image_file(struct Figure * fig, struct Pixel_Array * pa, char * filename)
{
	__label__ out;
	char * f = NULL, * f_ppm = NULL, * f_conv = NULL;
	char * buf = NULL;
	char * base, * ext;
	long len, buf_n;
	int fd, ret;
	long i;

	f = strdup(filename);
	len = strlen(f);
	for (i=len-1;i>=0;i--)
		if (f[i] == '.')
			break;
	if (i < 0 || i == len - 1)
		error("image filename extension not given (e.g. .ppm): %s", filename);
	f[i] = 0;
	base = f;
	ext = &f[i+1];

	buf_n = len + 4; // +4 to be sure it fits 'ppm\0' at the end.
	buf_n = 2 * buf_n + 1000; // More space to fit a command line.
	buf = malloc(buf_n);

	snprintf(buf, buf_n, "%s.ppm", base);
	f_ppm = strdup(buf);
	snprintf(buf, buf_n, "%s.%s", base, ext);
	f_conv = strdup(buf);
	if (fig->legend_conf.title == NULL)
		fig->legend_conf.title = strdup(f_conv);       // Better to immediately understand that it's just the file name, than try to extract a possibly non-existent meaning from it, so keep extension.

	fd = safe_open(f_ppm, O_WRONLY | O_TRUNC | O_CREAT);
	save_pbm_image(pa, fd);
	safe_close(fd);

	if (system("hash convert"))
		goto out;

	// Adding the legend in ppm format, before converting, is MUCH faster than working on an e.g. png file.
	if (fig->legend_conf.legend_enabled)
		add_legend(fig, f_ppm);

	if (!strcmp(ext, "ppm"))
		goto out;

	// File conversion.
	// Actually, the conversion to e.g. png takes most of the time, but the file sizes are orders of magnitude smaller.
	snprintf(buf, buf_n, "convert '%s' '%s'", f_ppm, f_conv);
	ret = system(buf);
	if (ret)
	{
		printf("convert failed with code: %d\n", ret);
		goto out;
	}
	snprintf(buf, buf_n, "rm '%s'", f_ppm);
	if (system(buf))
		goto out;

out:
	// Freeing NULL is well defined, manpage: If ptr is NULL, no operation is performed.
	free(f);
	free(f_ppm);
	free(f_conv);
	free(buf);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                         Exported Interface                                                             -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


static
void
figure_series_init(struct Figure_Series * s, void * x, void * y, void * z, long N, long M,
		double (* get_x_as_double)(void * x, long i),
		double (* get_y_as_double)(void * y, long i),
		double (* get_z_as_double)(void * z, long i)
		)
{
	s->N = N;
	if (M != 0)
	{
		s->cart_prod = 1;
		s->M = M;
	}
	else
	{
		s->cart_prod = 0;
		s->M = N;
	}

	s->x = x;
	s->y = y;
	s->z = z;

	s->x_in_percentages = 0;
	s->y_in_percentages = 0;

	if (get_x_as_double == NULL)
		get_x_as_double = id;
	if (get_y_as_double == NULL)
		get_y_as_double = id;
	s->get_x_as_double = get_x_as_double;
        s->get_y_as_double = get_y_as_double;
        s->get_z_as_double = get_z_as_double;

	s->color_mapping = figure_color_mapping_normal;
	s->r = 0x00;
	s->g = 0x00;
	s->b = 0x00;
	s->dot_size_pixels = 1;

	s->type_density_map = 0;

	s->type_histogram = 0;
	s->histogram_num_bins = 0;
	s->histogram_in_percentages = 0;
	s->histogram_in_percentages = 0;

	s->type_barplot = 0;
	s->barplot_bar_width_fraction = 0.6;
	s->barplot_max_bar_width = 0;
	s->barplot_bar_width = 0;

	s->deallocate_data = 0;
}


static
void
figure_series_clean(struct Figure_Series * s)
{
	if (s->deallocate_data)
	{
		free(s->x);
		free(s->y);
		free(s->z);
	}
	s->x = NULL;
	s->y = NULL;
	s->z = NULL;
}


void
figure_init(struct Figure * fig, int x_num_pixels, int y_num_pixels)
{
	fig->num_series = 0;
	fig->max_num_series = 4;
	fig->series = malloc(fig->max_num_series * sizeof(*fig->series));
	fig->x_num_pixels = x_num_pixels <= 0 ? 1920 : x_num_pixels;
	fig->y_num_pixels = y_num_pixels <= 0 ? 1920 : y_num_pixels;
	fig->axes_flip_x = 0;
	fig->axes_flip_y = 0;
	fig->custom_bounds_x = 0;
	fig->custom_bounds_y = 0;
	fig->legend_conf.legend_enabled = 0;
	fig->legend_conf.title = NULL;
}


void
figure_clean(struct Figure * fig)
{
	long i;
	for (i=0;i<fig->num_series;i++)
		figure_series_clean(&(fig->series[i]));
	free(fig->series);
	fig->series = NULL;
	free(fig->legend_conf.title);
	fig->legend_conf.title = NULL;
}


void
figure_destroy(struct Figure ** fig_ptr)
{
	figure_clean(*fig_ptr);
	free(*fig_ptr);
	*fig_ptr = NULL;
}


struct Figure_Series *
figure_add_series_base(struct Figure * fig, void * x, void * y, void * z, long N, long M,
		double (* get_x_as_double)(void * x, long i),
		double (* get_y_as_double)(void * y, long i),
		double (* get_z_as_double)(void * z, long i)
		)
{
	struct Figure_Series * s;
	long i, max_num_series;

	if (fig->num_series == fig->max_num_series)
	{
		max_num_series = 2 * fig->max_num_series;
		s = malloc(max_num_series * sizeof(*s));
		for (i=0;i<fig->max_num_series;i++)
			s[i] = fig->series[i];
		free(fig->series);
		fig->max_num_series = max_num_series;
		fig->series = s;
	}
	fig->num_series++;
	s = &fig->series[fig->num_series - 1];
	figure_series_init(s, x, y, z, N, M, get_x_as_double, get_y_as_double, get_z_as_double);
	return s;
}


void
figure_axes_flip_x(struct Figure * fig)
{
	fig->axes_flip_x = 1;
}


void
figure_axes_flip_y(struct Figure * fig)
{
	fig->axes_flip_y = 1;
}


void
figure_set_bounds_x(struct Figure * fig, double min, double max)
{
	fig->custom_bounds_x = 1;
	fig->x_min = min;
	fig->x_max = max;
}


void
figure_set_bounds_y(struct Figure * fig, double min, double max)
{
	fig->custom_bounds_y = 1;
	fig->y_min = min;
	fig->y_max = max;
}


void
figure_series_set_color(struct Figure_Series * s, int16_t r, int16_t g, int16_t b)
{
	s->r = r;
	s->g = g;
	s->b = b;
}


/*
 * 'val_norm': normalized value to [0, 1]
 * 'val'     : original value
 */
void
figure_series_set_color_mapping(struct Figure_Series * s, void color_mapping(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out))
{
	s->color_mapping = color_mapping;
}


void
figure_series_set_dot_size_pixels(struct Figure_Series * s, int size)
{
	s->dot_size_pixels = size > 0 ? size : 1;
}


void
figure_enable_legend(struct Figure * fig)
{
	fig->legend_conf.legend_enabled = 1;
}


void
figure_set_title(struct Figure * fig, char * title)
{
	free(fig->legend_conf.title);
	fig->legend_conf.title = strdup(title);
}


//==========================================================================================================================================
//= Figure Series Types
//==========================================================================================================================================


void
figure_series_type_density_map(struct Figure_Series * s)
{
	s->type_density_map = 1;
}


/* Histogram is a 2D plot of the occurrence frequencies of the series values ('y' for 2D and 'z' for 3D series).
 * The x axis is a discretization of the range of the values, with a given 'num_bins'.
 * This function converts the series to the 2D type_histogram.
 *
 * If 'num_bins' = 0 then plots in integer mode and calculates the number of bins as: max - min + 1.
 *
 * Returns the number of bins.
 * Through 'freq_out' it returns the bins frequencies as doubles.
 *
 * Integer / Float Modes:
 *     Like when matching value to pixel, the max values are an inclusive boundary, and are at the position of the 'number of bins' we divide each length by.
 *     But we have to be more careful with frequencies.
 *
 *     For integer frequencies:
 *         e.g. [0, 10]:
 *         num_bins = 11
 *         (max - min) / (num_bins - 1) = (10 - 0) / 10 = 1 , OK
 *
 *     For floating point frequencies:
 *         e.g. [0, 1]:
 *         num_bins = 11
 *         (max - min) / (num_bins - 1) = 0.1
 *         bin  0: [0-0.1)
 *         bin  1: [0.1-0.2)
 *         ...
 *         bin  9: [0.9-1.0)
 *         bin 10: [1.0-1.0] -> only max values, basically an empty bin
 *     It is more fair (i.e. same intervals for all bins) to do it this way:
 *         e.g. [0, 1]:
 *         num_bins = 10
 *         (max - min) / (num_bins) = 0.1
 *         bin  0: [0-0.1)
 *         bin  1: [0.1-0.2)
 *         ...
 *         bin  9: [0.9-1.0] : Check for max values and send them to this last bin.
 *
 *     We can't know which mode is appropriate (integer / float), so user has to select via the 'num_bins' argument.
 */
long
figure_series_type_histogram_base(struct Figure_Series * s, long num_bins, int plot_percentages,
		double ** freq_out)
{
	void * values;
	long values_n;
	double (* get_value_as_double)(void * val, long i);
	long * freq;
	double * x, * y;
	double quantum_size;
	double min, max;

	values_n = (s->cart_prod) ? s->M * s->N : s->N;
	if (s->z == NULL)
	{
		values = s->y;
		get_value_as_double = s->get_y_as_double;
	}
	else
	{
		values = s->z;
		get_value_as_double = s->get_z_as_double;
	}

	array_min_max(values, values_n, &min, NULL, &max, NULL, get_value_as_double);

	if (num_bins == 0)     // Integer mode.
	{
		num_bins = max - min + 1;
		quantum_size = 1;    // i.e.: quantum_size = (max - min) / (num_bins - 1);
	}
	else                   // Float mode.
		quantum_size = (max - min) / (num_bins);

	// if (num_bins > 1000000000)
		// warning("too many bins: %ld", num_bins);
	freq = malloc(num_bins * sizeof(*freq));
	x = malloc(num_bins * sizeof(*x));
	y = malloc(num_bins * sizeof(*y));

	#pragma omp parallel
	{
		long i;
		long pos;
		double v;
		#pragma omp for
		for (i=0;i<num_bins;i++)
			freq[i] = 0;
		#pragma omp for
		for (i=0;i<values_n;i++)
		{
			v = get_value_as_double(values, i);
			pos = (v - min) / quantum_size;
			if (pos >= num_bins)                 // This CAN happen here for the max values in float mode.
			{
				pos = num_bins - 1;
			}
			else if (pos < 0)                    // This should logically never happen (better safe than sorry, floats are weird).
			{
				error("frequency bin out of bounds: %ld\n", pos);
				pos = 0;
			}
			__atomic_fetch_add(&freq[pos], 1, __ATOMIC_RELAXED);
		}

		#pragma omp for
		for (i=0;i<num_bins;i++)
		{
			x[i] = quantum_size * i + min;
			y[i] = (double) freq[i];
			if (plot_percentages)
				y[i] = y[i] / values_n * 100;
		}
	}

	free(freq);

	s->N = num_bins;
	s->M = s->N;
	s->cart_prod = 0;
	s->x = x;
	s->y = y;
	s->z = NULL;
	if (plot_percentages)
		s->y_in_percentages = 1;
	s->get_x_as_double = gen_d2d;
	s->get_y_as_double = gen_d2d;
	s->get_z_as_double = NULL;
	s->type_histogram = 1;
	s->histogram_num_bins = num_bins;
	s->histogram_in_percentages = plot_percentages;
	s->deallocate_data = 1;
	if (freq_out != NULL)
		*freq_out = y;
	return num_bins;
}


void
figure_series_type_barplot_base(struct Figure_Series * s, double max_bar_width, double bar_width_fraction)
{
	s->type_barplot = 1;
	s->barplot_max_bar_width = max_bar_width;
	s->barplot_bar_width_fraction = bar_width_fraction;
}


//==========================================================================================================================================
//= Color Mappings
//==========================================================================================================================================


void
figure_color_mapping_geodesics(double val_norm, double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double val_proj = (val_norm - 0.5) * 4;
	uint8_t r = 0, g = 0, b = 0;
	if (val > 0)
	{
		r = ((double) 0xFF) * normal_distribution(1, 1, val_proj);
		g = ((double) 0xFF) * normal_distribution(0, 1, val_proj);
		// p->b = ((double) 0xFF) * normal_distribution(-1, 1, val_proj);
	}
	else
	{
		g = ((double) 0x88) * normal_distribution(0, 1, val_proj);
		b = ((double) 0xFF) * normal_distribution(-1, 1, val_proj);
		// b = ((double) 0xFF) * normal_distribution(0, 1, val_proj);
	}
	*r_out = r;
	*g_out = g;
	*b_out = b;
}


void
figure_color_mapping_normal(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double val_proj = (val_norm - 0.5) * 4;    // [-2, 2]
	*r_out = ((double) 0xFF) * normal_distribution( 1.2, 0.9, val_proj);
	*g_out = ((double) 0xFF) * normal_distribution( 0, 0.9, val_proj);
	*b_out = ((double) 0xFF) * normal_distribution(-1.2, 0.9, val_proj);
	// *r_out = ((double) 0xFF) * normal_distribution( 1, 1, val_proj);
	// *g_out = ((double) 0xFF) * normal_distribution( 0, 1, val_proj);
	// *b_out = ((double) 0xFF) * normal_distribution(-1, 1, val_proj);
}


void
figure_color_mapping_normal_logscale(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double val_proj = log2(((double) 0xFF) * val_norm + 1);  // [0, log2(256)=8]
	val_proj = (val_proj - 4) / 2;    // [-2, 2]
	*r_out = ((double) 0xFF) * normal_distribution( 1.2, 0.9, val_proj);
	*g_out = ((double) 0xFF) * normal_distribution( 0, 0.9, val_proj);
	*b_out = ((double) 0xFF) * normal_distribution(-1.2, 0.9, val_proj);
	// *r_out = ((double) 0xFF) * normal_distribution( 1, 1, val_proj);
	// *g_out = ((double) 0xFF) * normal_distribution( 0, 1, val_proj);
	// *b_out = ((double) 0xFF) * normal_distribution(-1, 1, val_proj);
}


void
figure_color_mapping_linear(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double r, g, b;
	r = (val_norm > 2.0/3) ? 3*(val_norm - 2.0/3) : 0;
	g = (val_norm < 0.5) ? 2*val_norm : 2*(1 - val_norm);
	b = (val_norm < 1.0/3) ? 1 - 3*val_norm: 0;
	*r_out = ((double) 0xFF) * r;
	*g_out = ((double) 0xFF) * g;
	*b_out = ((double) 0xFF) * b;
}


void
figure_color_mapping_cyclic(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double r, g, b;
	r = (val_norm > 2.0/3) ? 3*(val_norm - 2.0/3) : (val_norm < 1.0/3) ? 3*(1.0/3 - val_norm) : 0;
	g = (val_norm > 2.0/3) ? 3*(3.0/3 - val_norm) : (val_norm > 1.0/3) ? 3*(val_norm - 1.0/3) : 0;
	b = (val_norm < 1.0/3) ? 3*val_norm : (val_norm < 2.0/3) ? 3*(2.0/3 - val_norm) : 0;
	*r_out = ((double) 0xFF) * r;
	*g_out = ((double) 0xFF) * g;
	*b_out = ((double) 0xFF) * b;
}


void
figure_color_mapping_greyscale(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double val_proj = ((double) 0xFF) * val_norm;
	*r_out = *g_out = *b_out = val_proj;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Plotting Functions                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* 
 * - User can set custom (value) bounds, which might not fit all data.
 * - Inline functions somehow are slower.
 */  


#define find_pixel_coord(num_pixels, val, min, step, flip, q_out)                                                        \
({                                                                                                                       \
	long ret = 1;                                                                                                    \
	q_out = (long) floor((val - min) / step + 0.5);                                                                  \
	if (flip)                                                                                                        \
		q_out = num_pixels - 1 - q_out;                                                                          \
	if ((q_out < 0) || (q_out >= num_pixels)) /* This should logically NEVER happen. */                              \
	{                                                                                                                \
		error("find_pixel_coord: pixel coordinate out of bounds: %ld , num_pixels = %ld", q_out, num_pixels);    \
		ret = 0;                                                                                                 \
	}                                                                                                                \
	ret;                                                                                                             \
})


#define find_pixel_coord_x(fig, s, i, q_out)                                                         \
({                                                                                                   \
	double x = s->get_x_as_double(s->x, i);                                                      \
	find_pixel_coord(fig->x_num_pixels, x, fig->x_min, fig->x_step, fig->axes_flip_x, q_out);    \
})


#define find_pixel_coord_y(fig, s, i, q_out)                                                          \
({                                                                                                    \
	double y = s->get_y_as_double(s->y, i);                                                       \
	find_pixel_coord(fig->y_num_pixels, y, fig->y_min, fig->y_step, !fig->axes_flip_y, q_out);    \
})


//==========================================================================================================================================
//= Series Plot
//==========================================================================================================================================


static inline
void
color_pixels(struct Figure * fig, struct Pixel_Array * pa, long x_pix, long y_pix, struct Figure_Series * s, long z_pos)
{
	struct Pixel_8 * pixels = pa->pixels;
	long x_num_pixels = pa->width;
	long y_num_pixels = pa->height;
	double x_pix_left, x_pix_right;
	double y_pix_down, y_pix_up;
	long pos;
	double val;
	double val_norm;  // value normalized to [0, 1].
	uint8_t r, g, b;
	long dot_size_pixels;
	struct Pixel_8 * p;
	long i, j;

	dot_size_pixels = s->dot_size_pixels;

	pos = y_pix*x_num_pixels + x_pix;
	if ((dot_size_pixels <= 1) && !pixel_array_lock_pixel(pa, pos))
		return;
	if (s->z == NULL)
	{
		r = s->r;
		g = s->g;
		b = s->b;
	}
	else
	{
		val = s->get_z_as_double(s->z, z_pos); 
		val_norm = (val - s->z_min) / (s->z_max - s->z_min);
		s->color_mapping(val_norm, val, &r, &g, &b);
	}

	p = &pixels[pos];
	p->r = r;
	p->g = g;
	p->b = b;

	// barplot
	if (s->type_barplot)
	{
		long bar_width_pix;
		bar_width_pix = (long) floor(s->barplot_bar_width / fig->x_step + 0.5);
		if (bar_width_pix <= 0)
			bar_width_pix = 1;
		x_pix_left = x_pix - bar_width_pix/2;
		x_pix_right = x_pix + bar_width_pix/2;
		if (x_pix_left < 0)
			x_pix_left = 0;
		if (x_pix_right >= fig->x_num_pixels)
			x_pix_right = fig->x_num_pixels - 1;
		for (i=y_pix;i<y_num_pixels;i++)
		{
			for (j=x_pix_left;j<=x_pix_right;j++)
			{
				pos = i*x_num_pixels + j;
				if (!pixel_array_lock_pixel(pa, pos))
					continue;
				p = &pixels[pos];
				p->r = r;
				p->g = g;
				p->b = b;
			}
		}
	}
	else
	{
		y_pix_down = y_pix - dot_size_pixels/2;
		y_pix_up = y_pix + (dot_size_pixels-1)/2;
		if (y_pix_down < 0)
			y_pix_down = 0;
		if (y_pix_up >= fig->y_num_pixels)
			y_pix_up = fig->y_num_pixels - 1;
		for (i=y_pix_down;i<=y_pix_up;i++)
		{
			x_pix_left = x_pix - dot_size_pixels/2;
			x_pix_right = x_pix + (dot_size_pixels-1)/2;
			if (x_pix_left < 0)
				x_pix_left = 0;
			if (x_pix_right >= fig->x_num_pixels)
				x_pix_right = fig->x_num_pixels - 1;
			for (j=x_pix_left;j<=x_pix_right;j++)
			{
				pos = i*x_num_pixels + j;
				if (!pixel_array_lock_pixel(pa, pos))
					continue;
				p = &pixels[pos];
				p->r = r;
				p->g = g;
				p->b = b;
			}
		}
	}
}


static
void
series_plot(struct Figure * fig, struct Figure_Series * s, struct Pixel_Array * pa)
{
	#pragma omp parallel
	{
		long x_pix, y_pix;
		long i, j;
		if (s->cart_prod)
		{
			#pragma omp for schedule(static)
			for (i=0;i<s->M;i++)
			{
				if (!find_pixel_coord_y(fig, s, i, y_pix))
					continue;
				for (j=0;j<s->N;j++)
				{
					if (!find_pixel_coord_x(fig, s, j, x_pix))
						continue;
					color_pixels(fig, pa, x_pix, y_pix, s, i*s->N+j);
				}
			}
		}
		else
		{
			#pragma omp for schedule(static)
			for (i=0;i<s->M;i++)
			{
				if (!find_pixel_coord_y(fig, s, i, y_pix))
					continue;
				if (!find_pixel_coord_x(fig, s, i, x_pix))
					continue;
				color_pixels(fig, pa, x_pix, y_pix, s, i);
			}
		}
	}
}


//==========================================================================================================================================
//= Series Plot Densities of Occurrences
//==========================================================================================================================================


// Must be done in the end, right before plotting, so that the plotting parameters like image size have been finalized.
static
void
series_plot_density_map(struct Figure * fig, struct Figure_Series * s, struct Pixel_Array * pa)
{
	struct Pixel_8 * pixels = pa->pixels;
	long x_num_pixels = fig->x_num_pixels;
	long y_num_pixels = fig->y_num_pixels;
	long counts_n = x_num_pixels * y_num_pixels;
	int * counts;
	double min, max;

	counts = malloc(counts_n * sizeof(*counts));

	// Calculate densities for the pixels.
	#pragma omp parallel
	{
		long i, j, pos;
		long x_pix, y_pix;
		#pragma omp for schedule(static)
		for (i=0;i<counts_n;i++)
			counts[i] = 0;
		#pragma omp for schedule(static)
		for (i=0;i<s->M;i++)
		{
			if (!find_pixel_coord_y(fig, s, i, y_pix))
				continue;
			if (s->cart_prod)
			{
				for (j=0;j<s->N;j++)
				{
					if (!find_pixel_coord_x(fig, s, j, x_pix))
						continue;
					pos = y_pix*x_num_pixels + x_pix;
					__atomic_fetch_add(&counts[pos], 1, __ATOMIC_RELAXED);
				}
			}
			else
			{
				if (!find_pixel_coord_x(fig, s, i, x_pix))
					continue;
				pos = y_pix*x_num_pixels + x_pix;
				__atomic_fetch_add(&counts[pos], 1, __ATOMIC_RELAXED);
			}
		}
	}

	array_min_max(counts, counts_n, &min, NULL, &max, NULL, gen_i2d);

	#pragma omp parallel
	{
		struct Pixel_8 * p;
		double val, val_norm;
		long i, j, pos;
		#pragma omp for schedule(static)
		for (i=0;i<y_num_pixels;i++)
		{
			for (j=0;j<x_num_pixels;j++)
			{
				pos = i*x_num_pixels + j;
				val = counts[pos];
				if (val == 0)    // Only plot existing points of the series.
					continue;
				p = &pixels[pos];
				val_norm = (val - min) / (max - min);
				s->color_mapping(val_norm, val, &p->r, &p->g, &p->b);
			}
		}
	}

	free(counts);
}


//==========================================================================================================================================
//= Calculate bounds
//==========================================================================================================================================


static
int closest_pair_distance_cmpfunc(const void * a, const void * b)
{
	return (*(double *)a > *(double *)b) ? 1 : (*(double *)a < *(double *)b) ? -1 : 0;
}


static
double
closest_pair_distance(void * A, long N, double (* get_val_as_double)(void * A, long i))
{
	double * B = malloc(N * sizeof(*B));
	double dist, min;
	long i;
	if (N <= 1)
		return 0;
	for (i=0;i<N;i++)
		B[i] = get_val_as_double(A, i);
	qsort(B, N, sizeof(*B), closest_pair_distance_cmpfunc);
	min = fabs(B[1] - B[0]);
	for (i=0;i<N-1;i++)
	{
		dist = fabs(B[i+1] - B[i]);
		if (dist < min)
			min = dist;
	}
	free(B);
	return min;
}


static inline
void
calc_series_bounds(struct Figure_Series * s)
{
	if (s->x != NULL)
	{
		array_min_max(s->x, s->N, &s->x_min, NULL, &s->x_max, NULL, s->get_x_as_double);
		array_mean(s->x, s->N, &s->x_avg, s->get_x_as_double);
	}
	else
	{
		s->x_min = 0;
		s->x_max = s->N - 1;
		s->x_avg = s->N / 2;
	}
	if (s->y != NULL)
	{
		array_min_max(s->y, s->M, &s->y_min, NULL, &s->y_max, NULL, s->get_y_as_double);
		array_mean(s->y, s->N, &s->y_avg, s->get_y_as_double);
	}
	else
	{
		s->y_min = 0;
		s->y_max = s->M - 1;
		s->y_avg = s->M / 2;
	}
	if (s->z != NULL && !s->type_density_map)
	{
		long n = s->cart_prod ? s->M * s->N : s->N;
		array_min_max(s->z, n, &s->z_min, NULL, &s->z_max, NULL, s->get_z_as_double);
		array_mean(s->z, s->N, &s->z_avg, s->get_z_as_double);
	}
}


static
void
calc_figure_bounds(struct Figure * fig)
{
	struct Figure_Series * s;
	long i;

	for (i=0;i<fig->num_series;i++)
	{
		s = &fig->series[i];
		calc_series_bounds(s);
	}
	if (!fig->custom_bounds_x)
	{
		fig->x_min = INFINITY;
		fig->x_max = -INFINITY;
		for (i=0;i<fig->num_series;i++)
		{
			s = &fig->series[i];
			if (s->x_min < fig->x_min)
				fig->x_min = s->x_min;
			if (s->x_max > fig->x_max)
				fig->x_max = s->x_max;
		}
	}
	if (!fig->custom_bounds_y)
	{
		fig->y_min = INFINITY;
		fig->y_max = -INFINITY;
		for (i=0;i<fig->num_series;i++)
		{
			s = &fig->series[i];
			if (s->y_min < fig->y_min)
				fig->y_min = s->y_min;
			if (s->y_max > fig->y_max)
				fig->y_max = s->y_max;
		}
	}
	fig->z_min = INFINITY;
	fig->z_max = -INFINITY;
	for (i=0;i<fig->num_series;i++)
	{
		s = &fig->series[i];
		if (s->z != NULL && !s->type_density_map)
		{
			if (s->z_min < fig->z_min)
				fig->z_min = s->z_min;
			if (s->z_max > fig->z_max)
				fig->z_max = s->z_max;
		}
	}

	fig->legend_conf.x_in_percentages = 1;
	fig->legend_conf.y_in_percentages = 1;
	for (i=0;i<fig->num_series;i++)
	{
		s = &fig->series[i];

		// The labels will have a percentage sign, if all series are in percentages.
		fig->legend_conf.x_in_percentages &= s->x_in_percentages;
		fig->legend_conf.y_in_percentages &= s->y_in_percentages;

		if (s->type_barplot)
		{
			double min_dist = closest_pair_distance(s->x, s->N, s->get_x_as_double);
			// No need to check for sane values, if pixels width is <= 0 it will be corrected to = 1.
			if (s->barplot_max_bar_width > 0 && s->barplot_max_bar_width < min_dist)
				min_dist = s->barplot_max_bar_width;
			s->barplot_bar_width = s->barplot_bar_width_fraction * min_dist;
		}
	}
}


//==========================================================================================================================================
//= Plot
//==========================================================================================================================================


void
figure_plot(struct Figure * fig, char * filename)
{
	double x_num_pixels = fig->x_num_pixels;
	double y_num_pixels = fig->y_num_pixels;
	double x_len, y_len;
	double x_step, y_step;
	struct Pixel_Array * pa;
	struct Figure_Series * s;
	long i;

	pa = malloc(sizeof(*pa));
	pixel_array_init(pa, (long) x_num_pixels, (long) y_num_pixels, 0xFF);
	pixel_array_fill(pa, 0xFF, 0xFF, 0xFF);

	calc_figure_bounds(fig);

	x_len = fabs(fig->x_max - fig->x_min);
	y_len = fabs(fig->y_max - fig->y_min);
	if (x_len == 0)
		x_len = (y_len == 0) ? 1 : y_len;
	if (y_len == 0)
		y_len = x_len;

	// -1 to fit the max values which are an inclusive boundary, and are at the position of the 'number of pixels' we divide each length by.
	x_step = x_len / (x_num_pixels - 1);
	y_step = y_len / (y_num_pixels - 1);

	fig->x_step = x_step;
	fig->y_step = y_step;

	for (i=0;i<fig->num_series;i++)
	{
		pixel_array_reset_locks(pa);
		s = &fig->series[i];
		if (s->type_density_map == 1)
			series_plot_density_map(fig, s, pa);
		else
			series_plot(fig, s, pa);
	}

	write_image_file(fig, pa, filename);

	pixel_array_destroy(&pa);
	free(pa);
}

