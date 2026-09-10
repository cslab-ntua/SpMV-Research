#ifndef PLOT_H
#define PLOT_H

#include <stdlib.h>
#include <stdint.h>

#include "macros/cpp_defines.h"
#include "macros/macrolib.h"

#ifndef __cplusplus
	#include "genlib.h"
#endif

#include "storage_formats/pixel_array.h"


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
	struct Figure * fig;

	char * name;

	long N;
	long M;
	long L;
	int cart_prod;

	void * x;
	void * y;
	void * z;

	int ignore_invalid_values;

	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;

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

	// Type: Histogram.
	int type_histogram;
	long histogram_num_bins;

	// Type: Barplot.
	int type_barplot;
	double barplot_bar_width_fraction;
	double barplot_max_bar_width;
	double barplot_bar_width;

	// Type: Bounded median curve.
	int type_bounded_median_curve;
	int bounded_median_curve_axis;

	// Type: Pixel coordinates.
	int type_pixel_coords;

	// Type: 3D plot.
	int type_3d;
	double angle_z;   // Rotation around z axis.
	double angle_x;   // Rotation around x axis.
	double proj_z0;   // Depth, distance of plane from xy-plane after the rotations to make them parallel.
	double rotation_matrix[9];
	int grid_enabled;
	long grid_x_num_points;
	long grid_y_num_points;
	double * grid_px;
	double * grid_py;
	double * grid_depth;
	double grid_x_step;
	double grid_y_step;

	int deallocate_data;     // Whether to free x, y, z at destructor.
};


struct Figure {
	int max_num_series;
	int num_series;
	struct Figure_Series * series;
	int x_num_pixels;
	int y_num_pixels;

	int axes_equal_scale;
	int axes_flip_x;
	int axes_flip_y;

	int custom_bounds_x_min;
	int custom_bounds_x_max;
	int custom_bounds_y_min;
	int custom_bounds_y_max;

	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double z_min;
	double z_max;
	double x_step;
	double y_step;
	double z_step;

	struct Pixel_Array * pa;
	struct Figure_Legend_Conf legend_conf;
};


void figure_init(struct Figure * fig, int x_num_pixels, int y_num_pixels);
void figure_clean(struct Figure * fig);
void figure_destroy(struct Figure ** fig_ptr);

void figure_plot(struct Figure * fig);
void figure_save(struct Figure * fig, char * filename);

struct Figure_Series * figure_add_series_base(struct Figure * fig, void * x, void * y, void * z, long N, long M,
		double (* get_x_as_double)(void * x, long i),
		double (* get_y_as_double)(void * y, long i),
		double (* get_z_as_double)(void * z, long i)
		);
#ifdef __cplusplus
	#define figure_add_series(fig, x, y, z, N, M, get_x_as_double, get_y_as_double, get_z_as_double)    \
		figure_add_series_base(fig, x, y, z, N, M, get_x_as_double, get_y_as_double, get_z_as_double)
#else
	#define figure_add_series(fig, x, y, z, N, M, ... /* get_x_as_double(), get_y_as_double(), get_z_as_double() */ )    \
		figure_add_series_base(fig, x, y, z, N, M,                                                                   \
				DEFAULT_ARG_1(gen_functor_convert_basic_type_to_double(x), __VA_ARGS__),                     \
				DEFAULT_ARG_2(gen_functor_convert_basic_type_to_double(y), __VA_ARGS__),                     \
				DEFAULT_ARG_3(gen_functor_convert_basic_type_to_double(z), __VA_ARGS__))
#endif

void figure_series_ignore_invalid_values(struct Figure_Series * s);

void figure_series_set_name(struct Figure_Series * s, const char * name);

void figure_axes_set_equal_scale(struct Figure * fig);
void figure_axes_flip_x(struct Figure * fig);
void figure_axes_flip_y(struct Figure * fig);
void figure_set_bounds_x(struct Figure * fig, double min, double max);   // Bounds are inclusive.
void figure_set_bounds_x_min(struct Figure * fig, double min);           // Bounds are inclusive.
void figure_set_bounds_x_max(struct Figure * fig, double max);           // Bounds are inclusive.
void figure_set_bounds_y(struct Figure * fig, double min, double max);   // Bounds are inclusive.
void figure_set_bounds_y_min(struct Figure * fig, double min);           // Bounds are inclusive.
void figure_set_bounds_y_max(struct Figure * fig, double max);           // Bounds are inclusive.

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
long figure_series_type_histogram_base(struct Figure_Series * s, long num_bins, double ** freq_out, int plot_percentages, int cumulative_sum);
#define figure_series_type_histogram(s, num_bins, ... /* freq_out=NULL, plot_percentages=0, cumulative_sum=0 */)                                           \
({                                                                                                                                                         \
	figure_series_type_histogram_base(s, num_bins, DEFAULT_ARG_1(NULL, __VA_ARGS__), DEFAULT_ARG_2(0, __VA_ARGS__), DEFAULT_ARG_3(0, __VA_ARGS__));    \
})

void figure_series_type_barplot_base(struct Figure_Series * s, double max_bar_width, double bar_width_fraction);
#define figure_series_type_barplot(s, ... /* max_bar_width=0, bar_width_fraction=0.6 */)                       \
do {                                                                                                           \
	figure_series_type_barplot_base(s, DEFAULT_ARG_1(0, __VA_ARGS__), DEFAULT_ARG_2(0.6, __VA_ARGS__));    \
} while (0)

void figure_series_type_bounded_median_curve(struct Figure_Series * s, int axis);

void figure_series_type_pixel_coords(struct Figure_Series * s);


void figure_series_type_3d_base(struct Figure_Series * s, double nx, double ny, double nz, double x0, double y0, double z0);
#define figure_series_type_3d(s, ... /* nx=1, ny=1, nz=1, x0=0, y0=0, z0=0 */)                                                        \
do {                                                                                                                                  \
	figure_series_type_3d_base(s, DEFAULT_ARG_1(1, __VA_ARGS__), DEFAULT_ARG_2(1, __VA_ARGS__), DEFAULT_ARG_3(1, __VA_ARGS__),    \
			DEFAULT_ARG_4(0, __VA_ARGS__), DEFAULT_ARG_5(0, __VA_ARGS__), DEFAULT_ARG_6(0, __VA_ARGS__));                 \
} while (0)

void figure_series_type_3d_enable_grid_base(struct Figure_Series * s, long grid_step_size_in_pixels);
#define figure_series_type_3d_enable_grid(s, ... /* grid_step_size_in_pixels=10 */)    \
do {                                                                                   \
	figure_series_type_3d_enable_grid_base(s, DEFAULT_ARG_1(10, __VA_ARGS__));     \
} while (0)



// Simple Plot

// For the UNPACK() expansion.
#define _figure_simple_plot_add_series(...)  figure_add_series(__VA_ARGS__)

/*
 * Exported variables (for user code in __VA_ARGS__):
 *     _fig : struct Figure *
 *     _s   : struct Figure_Series *
 */
#define figure_simple_plot(file_out, x_num_pixels, y_num_pixels, series_args, ...)                               \
do {                                                                                                             \
	__attribute__((unused)) struct Figure_Series * _s;                                                       \
	__attribute__((cleanup(figure_destroy))) struct Figure * _fig = (typeof(_fig)) malloc(sizeof(*_fig));    \
	figure_init(_fig, x_num_pixels, y_num_pixels);                                                           \
	_s = _figure_simple_plot_add_series(_fig, UNPACK(series_args));                                          \
	__VA_ARGS__                                                                                              \
	figure_plot(_fig);                                                                                       \
	figure_save(_fig, file_out);                                                                             \
} while (0)


#endif /* PLOT_H */

