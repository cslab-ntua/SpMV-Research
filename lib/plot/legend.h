#ifndef LEGEND_H
#define LEGEND_H


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
	long i;

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
	int space_v = scale_y * 10 / 2;

	/*
	 * Because of the possible compression of the file from the ppm format (e.g. to png),
	 * the lower the value (or exactly white, test 254,255,255), the faster it runs e.g. 0.3 vs 1.9 sec.
	 * It also is influenced by the color of the text, white on grey seems to work ok, but not black on grey.
	 * So it should be the relation of the colors, i.e. the similarity that the compression sees.
	 */

	cmd_n = strlen(title) + 2*strlen(filename) + 100000;
	cmd = malloc(cmd_n);

	i = 0;
	i += snprintf(cmd+i, cmd_n-i, "convert");

	int b1_x = x + 14;
	int b1_y = y + 14;
	i += snprintf(cmd+i, cmd_n-i, " -gravity center");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(190,190,190)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b1_x, b1_y);

	int b2_x = b1_x + space_h;
	int b2_y = b1_y + 2*space_v;
	i += snprintf(cmd+i, cmd_n-i, " -gravity east");
	i += snprintf(cmd+i, cmd_n-i, " -background 'rgb(128,128,128)'");
	i += snprintf(cmd+i, cmd_n-i, " -extent %dx%d", b2_x, b2_y);
	i += snprintf(cmd+i, cmd_n-i, " -fill white");

	int title_y = (space_v - pointsize_big * px_per_pt) / 2;
	i += snprintf(cmd+i, cmd_n-i, " -gravity north");
	i += snprintf(cmd+i, cmd_n-i, " -pointsize %d", pointsize_big);
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text 0,%d '%s'\"", title_y, title);

	i += snprintf(cmd+i, cmd_n-i, " -pointsize %d", pointsize_small);

	int label_l_x = b1_x + text_spacer/2;
	int label_l_y = space_v;
	double text_ld = fig->y_min;
	double text_lu = fig->y_max;
	char * percentage_sign_y = fig->legend_conf.y_in_percentages ? "%" : "";
	if (fig->axes_flip_y)
	{
		text_ld = fig->y_max;
		text_lu = fig->y_min;
	}
	i += snprintf(cmd+i, cmd_n-i, " -gravity southeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_l_x, label_l_y, text_ld, percentage_sign_y);
	i += snprintf(cmd+i, cmd_n-i, " -gravity northeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_l_x, label_l_y, text_lu, percentage_sign_y);

	int label_dl_x = space_h;
	int label_d_y = space_v - pointsize_small - text_spacer;
	double text_dl = fig->x_min;
	double text_dr = fig->x_max;
	char * percentage_sign_x = fig->legend_conf.x_in_percentages ? "%" : "";
	i += snprintf(cmd+i, cmd_n-i, " -gravity southwest");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", label_dl_x, label_d_y, text_dl, percentage_sign_x);
	i += snprintf(cmd+i, cmd_n-i, " -gravity southeast");
	i += snprintf(cmd+i, cmd_n-i, " -draw \"text %d,%d '%g%s'\"", text_spacer, label_d_y, text_dr, percentage_sign_x);

	i += snprintf(cmd+i, cmd_n-i, " '%s' '%s'", filename, filename);

	// printf("%s\n", cmd);

	ret = system(cmd);
	if (ret)
		printf("convert failed with code: %d\n", ret);

	free(cmd);
}

#endif /* LEGEND_H */

