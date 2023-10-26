#ifndef PLOT_H
#define PLOT_H

#include <stdlib.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"
#include "genlib.h"


struct Figure_Legend_Conf {
	int x_in_percentages;
	int y_in_percentages;

	int legend_enabled;
	char * title;
};

/*
 * N - columns (x axis)
 * M - lines   (y axis)
 */
struct Figure_Series {
	long N;
	long M;
	int cart_prod;

	void * x;
	void * y;
	void * z;

	double x_min;
	double x_max;
	double x_avg;
	double y_min;
	double y_max;
	double y_avg;
	double z_min;
	double z_max;
	double z_avg;

	// The labels will have a percentage sign, if all series are in percentages.
	int x_in_percentages;
	int y_in_percentages;

	double (* get_x_as_double)(void * x, long i);
	double (* get_y_as_double)(void * y, long i);
	double (* get_z_as_double)(void * z, long i);

	void (* color_mapping)(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
	int16_t r;
	int16_t g;
	int16_t b;
	int dot_size_pixels;

	// Occurence density map.
	int type_density_map;

	// Histogram.
	int type_histogram;
	long histogram_num_bins;
	int histogram_in_percentages;

	// Barplot.
	int type_barplot;
	double barplot_bar_width_fraction;
	double barplot_max_bar_width;
	double barplot_bar_width;

	int deallocate_data;     // Whether to free x, y, z at destructor.
};


struct Figure {
	int max_num_series;
	int num_series;
	struct Figure_Series * series;
	int x_num_pixels;
	int y_num_pixels;
	int axes_flip_x;
	int axes_flip_y;

	int custom_bounds_x;
	int custom_bounds_y;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;
	double x_step;
	double y_step;
	double z_step;

	struct Figure_Legend_Conf legend_conf;
};


void figure_init(struct Figure * fig, int x_num_pixels, int y_num_pixels);
void figure_clean(struct Figure * fig);
void figure_destroy(struct Figure ** fig_ptr);

void figure_plot(struct Figure * fig, char * filename);

struct Figure_Series * figure_add_series_base(struct Figure * fig, void * x, void * y, void * z, long N, long M,
		double (* get_x_as_double)(void * x, long i),
		double (* get_y_as_double)(void * y, long i),
		double (* get_z_as_double)(void * z, long i)
		);
#define figure_add_series(fig, x, y, z, N, M, ... /* get_x_as_double(), get_y_as_double(), get_z_as_double() */ )    \
	figure_add_series_base(fig, x, y, z, N, M,                                                                   \
			DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(x), __VA_ARGS__),                     \
			DEFAULT_ARG_2(gen_functor_convert_basic_type_to_double(y), __VA_ARGS__),                     \
			DEFAULT_ARG_3(gen_functor_convert_basic_type_to_double(z), __VA_ARGS__))

void figure_axes_flip_x(struct Figure * fig);
void figure_axes_flip_y(struct Figure * fig);
void figure_set_bounds_x(struct Figure * fig, double min, double max);
void figure_set_bounds_y(struct Figure * fig, double min, double max);

// Color
void figure_series_set_color(struct Figure_Series * s, int16_t r, int16_t g, int16_t b);
void figure_series_set_color_mapping(struct Figure_Series * s, void color_mapping(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out));
void figure_series_set_dot_size_pixels(struct Figure_Series * s, int size);
void figure_color_mapping_geodesics(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
void figure_color_mapping_normal(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
void figure_color_mapping_normal_logscale(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
void figure_color_mapping_linear(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
void figure_color_mapping_cyclic(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);
void figure_color_mapping_greyscale(double val_norm, double val, uint8_t * r_out, uint8_t * g_out, uint8_t * b_out);

// Text
void figure_enable_legend(struct Figure * fig);
void figure_set_title(struct Figure * fig, char * title);


// Series Types

void figure_series_type_density_map(struct Figure_Series * s);

// Returns the number of bins.
// Through 'freq_out' it returns the bins frequencies as doubles.
long figure_series_type_histogram_base(struct Figure_Series * s, long num_bins, int plot_percentages, double ** freq_out);
#define figure_series_type_histogram(s, num_bins, ... /* plot_percentages, freq_out */)                                     \
({                                                                                                                          \
	figure_series_type_histogram_base(s, num_bins, DEFAULT_ARG_1(0, __VA_ARGS__), DEFAULT_ARG_2(NULL, __VA_ARGS__));    \
})

void figure_series_type_barplot_base(struct Figure_Series * s, double max_bar_width, double bar_width_fraction);
#define figure_series_type_barplot(s, ... /* max_bar_width, bar_width_fraction */)                             \
do {                                                                                                           \
	figure_series_type_barplot_base(s, DEFAULT_ARG_1(0, __VA_ARGS__), DEFAULT_ARG_2(0.6, __VA_ARGS__));    \
} while (0)


// Simple Plot

// For the UNPACK() expansion.
#define _figure_simple_plot_add_series(...)  figure_add_series(__VA_ARGS__)

/*
 * Exported variables (for user code in __VA_ARGS__):
 *     _fig : struct Figure *
 *     _s   : struct Figure_Series *
 */
#define figure_simple_plot(file_out, x_num_pixels, y_num_pixels, series_args, ...)    \
do {                                                                                  \
	struct Figure * _fig;                                                         \
	__attribute__((unused)) struct Figure_Series * _s;                            \
	_fig = (typeof(_fig)) malloc(sizeof(*_fig));                                  \
	figure_init(_fig, x_num_pixels, y_num_pixels);                                \
	_s = _figure_simple_plot_add_series(_fig, UNPACK(series_args));               \
	__VA_ARGS__                                                                   \
	figure_plot(_fig, file_out);                                                  \
	figure_destroy(&_fig);                                                        \
} while (0)


#endif /* PLOT_H */

