#include "plot.h"

#include "legend.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                             Utilities                                                                  -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// #define get_x(s, i)  ((s)->get_x_as_double((s)->x, i))
// #define get_y(s, i)  ((s)->get_y_as_double((s)->y, i))
// #define get_z(s, i)  ((s)->get_z_as_double((s)->z, i))


static
double
id(__attribute__((unused)) void * x, int i)
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
	if (fig->title == NULL)
		fig->title = strdup(f_conv);       // Better to immediately understand that it's just the file name, than try to extract a possibly non-existent meaning from it, so keep extension.

	fd = safe_open(f_ppm, O_WRONLY | O_TRUNC | O_CREAT);
	save_pbm_image(pa, fd);
	safe_close(fd);

	if (system("hash convert"))
		goto out;

	// Adding the legend in ppm format, before converting, is MUCH faster than working on an e.g. png file.
	if (fig->legend_enabled)
		add_legend(fig, f_ppm);

	if (!strcmp(ext, "ppm"))
		goto out;

	// File conversion.
	// Actually, the conversion to e.g. png takes most of the time, but the file sizes are orders of magnitude smaller.
	snprintf(buf, buf_n, "convert %s %s", f_ppm, f_conv);
	ret = system(buf);
	if (ret)
	{
		printf("convert failed with code: %d\n", ret);
		goto out;
	}
	snprintf(buf, buf_n, "rm %s", f_ppm);
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
		double (* get_x_as_double)(void * x, int i),
		double (* get_y_as_double)(void * y, int i),
		double (* get_z_as_double)(void * z, int i)
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

	if (x == NULL)
		get_x_as_double = id;
	if (y == NULL)
		get_y_as_double = id;
	s->get_x_as_double = get_x_as_double;
        s->get_y_as_double = get_y_as_double;
        s->get_z_as_double = get_z_as_double;

	s->color_mapping = figure_color_mapping_heatmap;
	s->r = 0x00;
	s->g = 0x00;
	s->b = 0x00;

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
figure_series_destroy(struct Figure_Series * s)
{
	if (s->deallocate_data)
	{
		free(s->x);
		free(s->y);
		free(s->z);
	}
}


void
figure_init(struct Figure * fig, int x_num_pixels, int y_num_pixels)
{
	fig->num_series = 0;
	fig->size = 4;
	fig->series = malloc(fig->size * sizeof(*fig->series));
	fig->x_num_pixels = x_num_pixels <= 0 ? 1920 : x_num_pixels;
	fig->y_num_pixels = y_num_pixels <= 0 ? 1920 : y_num_pixels;
	fig->axes_flip_x = 0;
	fig->axes_flip_y = 0;
	fig->custom_bounds_x = 0;
	fig->custom_bounds_y = 0;
	fig->legend_enabled = 0;
	fig->title = NULL;
}


void
figure_destroy(struct Figure * fig)
{
	long i;
	for (i=0;i<fig->num_series;i++)
		figure_series_destroy(&(fig->series[i]));
	free(fig->series);
	free(fig->title);
	free(fig);
}


struct Figure_Series *
figure_add_series_base(struct Figure * fig, void * x, void * y, void * z, long N, long M,
		double (* get_x_as_double)(void * x, int i),
		double (* get_y_as_double)(void * y, int i),
		double (* get_z_as_double)(void * z, int i)
		)
{
	struct Figure_Series * s;
	long i, size;

	if (fig->num_series == fig->size)
	{
		size = 2 * fig->size;
		s = malloc(size * sizeof(*s));
		for (i=0;i<fig->size;i++)
			s[i] = fig->series[i];
		free(fig->series);
		fig->size = size;
		fig->series = s;
	}
	fig->num_series++;
	s = &fig->series[fig->num_series - 1];
	figure_series_init(s, x, y, z, N, M, get_x_as_double, get_y_as_double, get_z_as_double);
	return s;
}


void
figure_series_set_color(struct Figure_Series * s, int16_t r, int16_t g, int16_t b)
{
	s->r = r;
	s->g = g;
	s->b = b;
}


void
figure_series_set_color_mapping(struct Figure_Series * s, void color_mapping(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out))
{
	s->color_mapping = color_mapping;
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
figure_enable_legend(struct Figure * fig)
{
	fig->legend_enabled = 1;
}


void
figure_set_title(struct Figure * fig, char * title)
{
	free(fig->title);
	fig->title = strdup(title);
}


void
figure_series_type_density_map(struct Figure_Series * s)
{
	s->type_density_map = 1;
}


// Histogram is a 2D plot of the occurrence frequencies of the series values ('y' for 2D and 'z' for 3D series).
// The x axis is a discretization of the range of the values, with a given 'num_bins'.
// This function converts the series to the 2D type_histogram.
static
void
convert_to_histogram(struct Figure_Series * s, long num_bins, int plot_percentages)
{
	void * values;
	long values_n;
	double (* get_value_as_double)(void * val, int i);
	long * freq;
	double * x, * y;
	double quantum_size;
	double min, max;

	freq = malloc(num_bins * sizeof(*freq));
	x = malloc(num_bins * sizeof(*x));
	y = malloc(num_bins * sizeof(*y));

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

	matrix_min_max(values, values_n, &min, &max, get_value_as_double);

	quantum_size = (max - min) / (num_bins);

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
			if (pos >= num_bins)                 // This occurs for the max values only.
				pos = num_bins - 1;
			else if (pos < 0)                    // This should logically never happen (better safe than sorry, floats are weird).
				pos = 0;
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
}


void
figure_series_type_histogram_base(struct Figure_Series * s, long num_bins, int plot_percentages)
{
	convert_to_histogram(s, num_bins, plot_percentages);
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


// static inline
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
		// b = ((double) 0xFF) * normal_distribution(-1, 1, val_proj);
		b = ((double) 0xFF) * normal_distribution(0, 1, val_proj);
	}
	*r_out = r;
	*g_out = g;
	*b_out = b;
}


void
figure_color_mapping_heatmap(double val_norm, __attribute__((unused)) double val,
		uint8_t * r_out, uint8_t * g_out, uint8_t * b_out)
{
	double val_proj = (val_norm - 0.5) * 4;
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



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//------------------------------------------------------------------------------------------------------------------------------------------
//-                                                          Plotting Functions                                                            -
//------------------------------------------------------------------------------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/* 
 * - User can set custom (value) bounds, which might not fit all data.
 * - Inline functions somehow are slower.
 */  


#define find_pixel_coord(num_pixels, val, min, step, flip, q_out)    \
({                                                                   \
	long ret = 1;                                                \
	q_out = (long) floor((val - min) / step + 0.5);              \
	if (flip)                                                    \
		q_out = num_pixels - 1 - q_out;                      \
	if ((q_out < 0) || (q_out >= num_pixels))                    \
		ret = 0;                                             \
	ret;                                                         \
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
	long pos;
	double val;
	double val_norm;  // value normalized to [0, 1].
	uint8_t r, g, b;
	struct Pixel_8 * p;
	long i, j;
	pos = y_pix*x_num_pixels + x_pix;
	if (!pixel_array_lock_pixel(pa, pos))
		return;
	p = &pixels[pos];
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

	p->r = r;
	p->g = g;
	p->b = b;

	// barplot
	if (s->type_barplot && s->barplot_bar_width > 0)
	{
		double x_pix_left, x_pix_right;
		long bar_width_pix;
		bar_width_pix = (long) floor(s->barplot_bar_width / fig->x_step + 0.5);
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

	matrix_min_max(counts, counts_n, &min, &max, gen_i2d);

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


__attribute__((unused))
static
double
MATRIX_METRICS_closest_pair_distance(void * A, long N, double (* get_val_as_double)(void * A, int i))
{
	double * B = malloc(N * sizeof(*B));
	double dist, min;
	long i;
	if (N <= 1)
		return 0;
	int cmpfunc(const void * a, const void * b)
	{
		return (*(double *)a > *(double *)b) ? 1 : (*(double *)a < *(double *)b) ? -1 : 0;
	}
	for (i=0;i<N;i++)
		B[i] = get_val_as_double(A, i);
	qsort(B, N, sizeof(*B), cmpfunc);
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
		matrix_min_max(s->x, s->N, &s->x_min, &s->x_max, s->get_x_as_double);
	else
	{
		s->x_min = 0;
		s->x_max = s->N - 1;
	}
	if (s->y != NULL)
		matrix_min_max(s->y, s->M, &s->y_min, &s->y_max, s->get_y_as_double);
	else
	{
		s->y_min = 0;
		s->y_max = s->M - 1;
	}
	if (s->z != NULL && !s->type_density_map)
	{
		long n = s->cart_prod ? s->M * s->N : s->N;
		matrix_min_max(s->z, n, &s->z_min, &s->z_max, s->get_z_as_double);
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
			fig->x_min = (s->x_min < fig->x_min) ? s->x_min : fig->x_min;
			fig->x_max = (s->x_max > fig->x_max) ? s->x_max : fig->x_max;
		}
	}
	if (!fig->custom_bounds_y)
	{
		fig->y_min = INFINITY;
		fig->y_max = -INFINITY;
		for (i=0;i<fig->num_series;i++)
		{
			s = &fig->series[i];
			fig->y_min = (s->y_min < fig->y_min) ? s->y_min : fig->y_min;
			fig->y_max = (s->y_max > fig->y_max) ? s->y_max : fig->y_max;
		}
	}
	fig->z_min = INFINITY;
	fig->z_max = -INFINITY;
	for (i=0;i<fig->num_series;i++)
	{
		s = &fig->series[i];
		if (s->z != NULL && !s->type_density_map)
		{
			fig->z_min = (s->z_min < fig->z_min) ? s->z_min : fig->z_min;
			fig->z_max = (s->z_max > fig->z_max) ? s->z_max : fig->z_max;
		}
	}

	fig->x_in_percentages = 1;
	fig->y_in_percentages = 1;
	for (i=0;i<fig->num_series;i++)
	{
		s = &fig->series[i];

		// The labels will have a percentage sign, if all series are in percentages.
		fig->x_in_percentages &= s->x_in_percentages;
		fig->y_in_percentages &= s->y_in_percentages;

		if (s->type_barplot)
		{
			double min_dist = MATRIX_METRICS_closest_pair_distance(s->x, s->N, s->get_x_as_double);
			// printf("min dist = %lf\n", min_dist);
			// printf("barplot_max_bar_width = %lf\n", s->barplot_max_bar_width);
			if (s->barplot_max_bar_width > 0 && s->barplot_max_bar_width < min_dist)
				min_dist = s->barplot_max_bar_width;
			s->barplot_bar_width = s->barplot_bar_width_fraction * min_dist;
			// printf("barplot_bar_width = %lf\n", s->barplot_bar_width);
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

	// -1 to fit the max values, which are at the position of the 'number of pixels' we divide each length by.
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

	free(pa);
}

