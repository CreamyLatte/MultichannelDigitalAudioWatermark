//
//  view_gnuplot.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/13.
//

#ifndef view_gnuplot_h
#define view_gnuplot_h

#include <stdio.h>

#define GNUPLOT_PATH "/usr/local/bin/gnuplot"

typedef struct {
    int min;
    int max;
} GPlotRange;

typedef struct {
    int grid; //is bool
    char *x_label;
    char *y_label;
    GPlotRange x_range;
    GPlotRange y_range;
    char *out_format;
    char *output_file_path;
    char *read_file_path;
    char *plot_option;
} GPlot2D;

void output_2D_gnuplot(GPlot2D data);
void raw_gnuplot(const char *script);
GPlot2D init_GPlot2D(int grid, char *x_label, char *y_label, int x_min, int x_max, int y_min, int y_max, char *out_format, char *output_file_path, char *read_file_path, char *plot_option);

//↓xcode view
//init_GPlot2D(<#int grid#>, <#char *x_label#>, <#char *y_label#>, <#int x_min#>, <#int x_max#>, <#int y_min#>, <#int y_max#>, <#char *output_file_path#>, <#char *read_file_path#>, <#char *plot_option#>)
/*
(GPlot2D) {
    .grid = <#0 or 1#>,
    .x_label = <#x label name#>,
    .y_label = <#y lable name#>,
    .x_range = (GPlotRange) {
        .min = <#int parameter#>,
        .max = <#int parameter#>
    },
    .y_range = (GPlotRange) {
        .min = <#int parameter#>,
        .max = <#int parameter#>
    },
    .out_format = <#out format#>,
    .output_file_path = <#output file path#>,
    .read_file_path = <#read file path#>,
    .plot_option = <#plot_option#>
};
 */
#endif /* view_gnuplot_h */
