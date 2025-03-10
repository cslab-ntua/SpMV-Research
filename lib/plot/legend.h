#ifndef LEGEND_H
#define LEGEND_H

#include <stdlib.h>
#include <stdio.h>


//==========================================================================================================================================
//= Add Legend
//==========================================================================================================================================


static
void
add_legend(struct Figure * fig, char * filename)
{
	char * title = fig->legend_conf.title;
	char * cmd;
	long cmd_n;
	int ret;
	long i, j;

	int x = fig->x_num_pixels;
	int y = fig->y_num_pixels;

	double base = 100;
	double scale_x = x / base;
	double scale_y = y / base;

	// double scale_min = scale_x > scale_y ? scale_y : scale_x;
	// if (scale_min < 1)
		// scale_min = 1;
	// scale_x = scale_y = scale_min;

	double px_per_pt = 1.3281472327365;    // 1 pt = 1.3281472327365 px
	double pt_per_px = 1 / px_per_pt;

	// int max_chars = 0;
	// i = snprintf(NULL, 0, "%g", fig->y_min);
	// if (i > max_chars)
		// max_chars = i;
	// i = snprintf(NULL, 0, "%g", fig->y_max);
	// if (i > max_chars)
		// max_chars = i;
	// printf("%d\n", max_chars);

	// If space_h is 10% and pointsize_small is 1.5% in width, minus the text_spacer, then we can fit max ~ 15 characters horizontally.
	int pointsize_small = pt_per_px * scale_x * 1.5;
	int pointsize_big = pointsize_small * 4 / 3;
	int text_spacer = px_per_pt * pointsize_small / 3;

	int space_h = scale_x * 10;
	int space_v_u = scale_y * 10 / 2;
	int space_v_d = scale_y * 10 / 2;

	int title_len = strlen(title);
	int num_lines = 0;
	for (j=0;j<title_len;j++)
	{
		if (title[j] == '\n')
			num_lines++;
	}
	num_lines++;
	space_v_u += (pointsize_big * px_per_pt) * num_lines;

	/*
	 * Because of the possible compression of the file from the ppm format (e.g. to png),
	 * the lower the value (or exactly white, test 254,255,255), the faster it runs e.g. 0.3 vs 1.9 sec.
	 * It also is influenced by the color of the text, white on grey seems to work ok, but not black on grey.
	 * So it should be the relation of the colors, i.e. the similarity that the compression sees.
	 */

	cmd_n = title_len + 2*strlen(filename) + 100000;
	cmd = malloc(cmd_n);

	i = 0;
	i += snprintf(cmd+i, cmd_n-i, "magick");

	// Input file.
	i += snprintf(cmd+i, cmd_n-i, " '%s'", filename);

	// Output options.

	// Border inner.
	int b1_x = x + 14;
	int b1_y = y + 14;
	i += snprintf(cmd+i, cmd_n-i, " -gravity center");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(190,190,190)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b1_x, b1_y);

	// Border outer left.
	int b2_x = b1_x + space_h;
	int b2_y = b1_y;
	i += snprintf(cmd+i, cmd_n-i, " -gravity east");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(128,128,128)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b2_x, b2_y);
	i += snprintf(cmd+i, cmd_n-i, " -fill white");

	// Border outer up.
	int b3_x = b2_x;
	int b3_y = b2_y + space_v_u;
	i += snprintf(cmd+i, cmd_n-i, " -gravity southeast");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(128,128,128)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b3_x, b3_y);
	i += snprintf(cmd+i, cmd_n-i, " -fill white");

	// Border outer down.
	int b4_x = b3_x;
	int b4_y = b3_y + space_v_d;
	i += snprintf(cmd+i, cmd_n-i, " -gravity northeast");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(128,128,128)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b4_x, b4_y);
	i += snprintf(cmd+i, cmd_n-i, " -fill white");


	// Title text.
	int title_y = (space_v_u - num_lines * pointsize_big * px_per_pt) / 2;
	i += snprintf(cmd+i, cmd_n-i, " -gravity north");
	i += snprintf(cmd+i, cmd_n-i, " -pointsize %d", pointsize_big);
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text 0,%d '%s'\"", title_y, title);


	i += snprintf(cmd+i, cmd_n-i, " -pointsize %d", pointsize_small);

	// y axis text
	int label_l_x = b1_x + text_spacer/2;
	int label_lu_y = space_v_u;
	int label_ld_y = space_v_d;
	double text_ld = fig->y_min;
	double text_lu = fig->y_max;
	char * percentage_sign_y = fig->legend_conf.y_in_percentages ? "%" : "";
	if (fig->axes_flip_y)
	{
		text_ld = fig->y_max;
		text_lu = fig->y_min;
	}
	i += snprintf(cmd+i, cmd_n-i, " -gravity southeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_l_x, label_ld_y, text_ld, percentage_sign_y);
	i += snprintf(cmd+i, cmd_n-i, " -gravity northeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_l_x, label_lu_y, text_lu, percentage_sign_y);

	// x axis text
	int label_dl_x = space_h;
	int label_d_y = space_v_d - pointsize_small - text_spacer;
	double text_dl = fig->x_min;
	double text_dr = fig->x_max;
	char * percentage_sign_x = fig->legend_conf.x_in_percentages ? "%" : "";
	i += snprintf(cmd+i, cmd_n-i, " -gravity southwest");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_dl_x, label_d_y, text_dl, percentage_sign_x);
	i += snprintf(cmd+i, cmd_n-i, " -gravity southeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", text_spacer, label_d_y, text_dr, percentage_sign_x);

	// Output file.
	i += snprintf(cmd+i, cmd_n-i, " '%s'", filename);

	// printf("%s\n", cmd);

	ret = system(cmd);
	if (ret)
		printf("convert failed with code: %d\n", ret);

	free(cmd);
}

#endif /* LEGEND_H */

