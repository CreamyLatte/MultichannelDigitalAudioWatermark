//
//  view_gnuplot.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/13.
//

#include "view_gnuplot.h"
#include "my_str.h"
#include <stdlib.h>
#include <string.h>

void output_2D_gnuplot(GPlot2D data) {
    FILE *ifp, *ofp, *gpp;
    
    //read_file_pathチェック
    ifp = fopen(data.read_file_path, "r");
    if(ifp == NULL) {
        fprintf(stderr, "view_2D_gnuplot: Read file open error!\n\"%s\"を読み込むことができません。\n\n", data.read_file_path);
        return;
    }
    fclose(ifp);
    
    //output_file_path拡張子チェック
    if(!str_comparison(ext_search(data.output_file_path), data.out_format)) {
        char *new_path = (char*)alloca(str_count(data.output_file_path)+str_count(data.out_format));
        sprintf(new_path, "%s.%s", data.output_file_path, data.out_format);
        data.output_file_path = new_path;
    }
    //output_file_pathチェック
    ofp = fopen(data.output_file_path, "w");
    if(ofp == NULL) {
        fprintf(stderr, "view_2D_gnuplot: Output file open error!\n\"%s\"に書き込むことができません。\n\n", data.output_file_path);
        return;
    }
    fclose(ofp);
    
    
    //gnuplot出力
    char *error_text = "view_2D_gnuplot: gnuplot path error!\ngnuplotに接続できませんでした。\n\n";
    char *text = (char*)alloca(str_count(GNUPLOT_PATH)+17);
    sprintf(text, "%s -V &> /dev/null", GNUPLOT_PATH);
    if(0 != system(text)) {
        fprintf(stderr, "%s", error_text);
        return;
    }
    text = (char*)alloca(str_count(GNUPLOT_PATH)+10);
    sprintf(text, "%s -persist", GNUPLOT_PATH);
    gpp = popen(text, "w");
    if(gpp == NULL) {
        fprintf(stderr, "%s", error_text);
        return;
    }
    //csvチェック
    if(str_comparison(ext_search(data.read_file_path), "csv")) {
        fprintf(gpp, "set datafile separator \",\"\n");
    }
    //set terminal
    fprintf(gpp, "set terminal %s\n", data.out_format);
    //set output
    fprintf(gpp, "set output \"%s\"\n", data.output_file_path);
    //set grid
    if(data.grid) {
        fprintf(gpp, "set grid\n");
    }
    //set label
    if(data.x_label != NULL) {
        fprintf(gpp, "set xlabel \"%s\"\n", data.x_label);
    }
    if(data.y_label != NULL) {
        fprintf(gpp, "set ylabel \"%s\"\n", data.y_label);
    }
    //set range
    if(data.x_range.min != 0 || data.x_range.max != 0) {
        fprintf(gpp, "set xrange [%d:%d]\n", data.x_range.min, data.x_range.max);
    }
    if(data.y_range.min != 0 || data.y_range.max != 0) {
        fprintf(gpp, "set yrange [%d:%d]\n", data.y_range.min, data.y_range.max);
    }
    //plot
    fprintf(gpp, "plot \"%s\"", data.read_file_path);
    if(data.plot_option != NULL) {
        fprintf(gpp, " %s", data.plot_option);
    }
    fprintf(gpp, "\n");
    pclose(gpp);
}

void raw_gnuplot(const char *script) {
    FILE *gpp;
    char *error_text = "view_2D_gnuplot: gnuplot path error!\ngnuplotに接続できませんでした。\n\n";
    char *text = (char*)alloca(str_count(GNUPLOT_PATH)+17);
    sprintf(text, "%s -V &> /dev/null", GNUPLOT_PATH);
    if(0 != system(text)) {
        fprintf(stderr, "%s", error_text);
        return;
    }
    text = (char*)alloca(str_count(GNUPLOT_PATH)+10);
    sprintf(text, "%s -persist", GNUPLOT_PATH);
    gpp = popen(text, "w");
    if(gpp == NULL) {
        fprintf(stderr, "%s", error_text);
        return;
    }
    fprintf(gpp, "%s", script);
    pclose(gpp);
}

GPlot2D init_GPlot2D(int grid, char *x_label, char *y_label, int x_min, int x_max, int y_min, int y_max, char *out_format, char *output_file_path, char *read_file_path, char *plot_option) {
    GPlot2D property;
    GPlotRange x_range = {
        .min = x_min,
        .max = x_max
    };
    GPlotRange y_range = {
        .min = y_min,
        .max = y_max
    };
    property = (GPlot2D) {
        .grid = grid,
        .x_label = x_label,
        .y_label = y_label,
        .x_range = x_range,
        .y_range = y_range,
        .out_format = out_format,
        .output_file_path = output_file_path,
        .read_file_path = read_file_path,
        .plot_option = plot_option
    };
    return property;
}

