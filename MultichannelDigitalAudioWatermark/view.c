//
//  view.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/12/01.
//

#include "view.h"
#include <stdlib.h>
#include "my_str.h"
#include "view_gnuplot.h"

void soundData_energie_view(SoundData sound_data, int frame, const char *file_name, int eh_type) {
    int i; //loop
    static int block_sum = 0, cnt = 0, min_sum = 0, min_size_sum = 0, e_ave_sum = 0;
    cnt++;
    
    //出力ファイル設定
    if(file_name == NULL) {
        fprintf(stderr, "%s() error. file_name is NULL.", __func__);
        return;
    }
    int str_size = str_count(file_name);
    char *sf_path = (char*)malloc(sizeof(char) * (str_size + 256)); //mono_data出力用
    sprintf(sf_path, "%s_mono.txt", file_name);
    char *ef_path = (char*)malloc(sizeof(char) * (str_size + 256)); //signal_energie出力用
    sprintf(ef_path, "%s_energie.txt", file_name);
    char *pf_path = (char*)malloc(sizeof(char) * (str_size + 256)); //signal_energie出力用
    sprintf(pf_path, "%s_sig_ene.png", file_name);
    
    //初期値設定
    int data_size = sound_data.sample_size;
    int *mono_data = monaural_converter(sound_data);
    int *signal_energie = (int*)malloc(sizeof(int) * data_size);
    FILE *ofp;
    int e_min = AMP_SIZE, e_max = 0;
    long e_ave = 0;
    //mono_data出力
    ofp = fopen(sf_path, "w");
    for(i = 0; i < data_size; i++) {
        fprintf(ofp, "%d\n", mono_data[i]);
    }
    fflush(ofp);
    fclose(ofp);
    
    //signal_energie出力
    ofp = fopen(ef_path, "w");
    for(i = 0; i < data_size; i++) {
        signal_energie[i] = wave_energie(&mono_data[i], i >= data_size-frame ? (data_size-i) : frame);
        e_ave += (long)signal_energie[i];
        if(signal_energie[i] < e_min) {
            e_min = signal_energie[i];
        }
        if(signal_energie[i] > e_max) {
            e_max = signal_energie[i];
        }
        fprintf(ofp, "%d\n", signal_energie[i]);
    }
    e_ave /= (long)data_size;
    e_ave_sum += (int)e_ave;
    fflush(ofp);
    fclose(ofp);
    printf("energie min: %d, energie max: %d\n", e_min, e_max);
    min_sum += e_min;
    
    //[仮置き]VAD block separation test
    SoundBlock vad[BLOCK_N];
    char *vad_path = (char*)malloc(sizeof(char) * (str_size + 256)); //mono_data出力用
    sprintf(vad_path, "%s_vad.txt", file_name);
    char *lv_path = (char*)malloc(sizeof(char) * (str_size + 256)); //mono_data出力用
    sprintf(lv_path, "%s_lv.txt", file_name);
    ofp = fopen(lv_path, "w");
    int el = 0.03*(e_max-e_min) + e_min < 4 * e_min ? 0.03*(e_max-e_min) + e_min : 4 * e_min;
    el = (el / 512 + (el%512!=0)) * 512;
    int eh;
    switch(eh_type) {
        default:
            eh = 5 * el;
            break;
        case 2:
            eh = (e_max-e_min) / 2;
            eh = (eh / 512 + (eh%512!=0)) * 512;
            if(eh <= el) {
                eh = el + 512;
            }
            break;
        case 3:
            eh = (int)e_ave;
            eh = (eh / 512 + (eh%512!=0)) * 512;
            if(eh <= el) {
                eh = el + 512;
            }
    }
    printf("energie low: %d, energie high: %d\n", el, eh);
    int energie, lv;
    for(i = 0; i < data_size - 2 * F; i++) {
        energie = signal_energie[i];
        lv = level_selection(energie, el, eh);
        fprintf(ofp, "%d\n", lv == 0 ? 0 : lv == 1 ? el : eh);
    }
    fflush(ofp);
    fclose(ofp);
    ofp = fopen(vad_path, "w");
    int block_size = block_separation(mono_data, vad, data_size, frame, el, eh);
    block_sum += block_size;
    int min_size = 4410000;
    for(i = 0; i < block_size; i++) {
        if(vad[i].size < min_size) {
            min_size = vad[i].size;
        }
    }
    min_size_sum += min_size;
    int j;
    for(i = 0; i < block_size; i++) {
        printf("[voice: %s]index: %3d, size: %6d, sec: %lf\n", i % 2 == 0 ? "OFF" : " ON", i, vad[i].size, (double)vad[i].size/44100);
        for(j = 0; j < vad[i].size; j++) {
            fprintf(ofp, "%d\n", i % 2 == 0 ? 0 : AMP_SIZE);
        }
    }
    fflush(ofp);
    fclose(ofp);
    printf("cnt: %2d, block_sum: %5d, ave: %lf, min_size_ave: %lf, e_ave: %lf\n", cnt, block_sum, (double)block_sum/cnt, (double)min_size_sum/cnt, (double)e_ave_sum/cnt);
    
    //gnuplot出力
    char plot_txt[1024];
    sprintf(plot_txt, "set term png size 9600, 800\nset grid\nset xlabel 'Samples'\nset ylabel 'Energie'\nset xrange [%d:%d]\nset yrange [%d:%d]\nset output '%s'\nplot '%s' with line, '%s' with line, '%s' with line, '%s' with line", 0, data_size, -AMP_SIZE, AMP_SIZE, pf_path, sf_path, ef_path, lv_path, vad_path);
    raw_gnuplot(plot_txt);
    
    //メモリ開放
    free(mono_data);
    free(signal_energie);
    free(sf_path);
    free(ef_path);
    free(pf_path);
    
    free(lv_path);
    free(vad_path);
    
}

void soundData_energie_histogram_view(SoundData sound_data, int frame, const char *file_name) {
    int i; //loop
    if(file_name == NULL) {
        fprintf(stderr, "%s() error. file_name is NULL.", __func__);
        return;
    }
    int data_size = sound_data.sample_size;
    int *mono_data = monaural_converter(sound_data);
    int str_size = str_count(file_name);
    int energie, energie_histogram[AMP_SIZE];
    for(i = 0; i < AMP_SIZE; i++) {
        energie_histogram[i] = 0;
    }
    char *file_path = (char*)malloc(sizeof(char) * (str_size + 256));
    sprintf(file_path, "%s.txt", file_name);
    char *area_path = (char*)malloc(sizeof(char) * (str_size + 256));
    sprintf(area_path, "%s_area.txt", file_name);
    FILE *fp = fopen(file_path, "w"), *afp = fopen(area_path, "w");
    for(i = 0; i < data_size-frame; i++) {
        energie = wave_energie(&mono_data[i], frame);
        energie_histogram[energie]++;
        fprintf(fp, "%d\n", energie);
    }
    fflush(fp);
    fclose(fp);
    char *plot_path = (char*)malloc(sizeof(char) * (str_size + 256));
    sprintf(plot_path, "%s.png", file_name);
    GPlot2D plot = init_GPlot2D(1, "Samples", "Energie", 0, 0, 0, 0, "png", plot_path, file_path, "w l");
    output_2D_gnuplot(plot);
    sprintf(file_path, "%s_histogram.txt", file_name);
    fp = fopen(file_path, "w");
    int mode = 0;
    for(i = 0; i < AMP_SIZE; i++) {
        if(energie_histogram[i] > energie_histogram[mode]) {
            mode = i;
        }
        fprintf(fp, "%d\n", energie_histogram[i]);
    }
    int count = 0;
//    for(i = 0; i < mode; i++) {
//        if(energie_histogram[i] > 0) {
//            count++;
//        }
//    }
    for(i = 0; i < AMP_SIZE; i++) {
        count += energie_histogram[i];
        if(count >= data_size/2) {
            break;
        }
    }
    printf("mode: %3d, energie_histogram[mode]: %5d, count: %3d\n", mode, energie_histogram[mode], count);
    int low_energie = mode + count; low_energie = i;
    for(i = 0; i < data_size-frame; i++) {
        if(wave_energie(&mono_data[i], frame) < low_energie) {
            fprintf(afp, "%d\n", low_energie);
        } else {
            fprintf(afp, "%d\n", 0);
        }
    }
    fflush(fp);
    fclose(fp);
    sprintf(plot_path, "%s_histogram.png", file_name);
    plot = init_GPlot2D(1, "Energie", "Count", 0, 500, 0, 0, "png", plot_path, file_path, "w boxes");
    output_2D_gnuplot(plot);
    sprintf(file_path, "%s.txt", file_name);
    sprintf(plot_path, "%s_area.png", file_name);
    char plot_txt[1024];
    sprintf(plot_txt, "set term png size 1200, 400\nset grid\nset xlabel 'Samples'\nset ylabel 'Energie'\nset output '%s'\nplot '%s' with line, '%s' with line", plot_path, file_path, area_path);
    raw_gnuplot(plot_txt);
    free(mono_data);
    free(file_path);
    free(plot_path);
    return;
}

void block_signal_view(MultiBlock data[], int size, int nchannels, char *file_name) {
    int i, j, k;
    int block_size;
    int *pointer;
    FILE *fp;
    char file_path[256], plot_name[256];
    
    for(i = 0; i < size; i++) {
        block_size = data[i].size;
        
        for(j = 0; j < nchannels; j++) {
            pointer = data[i].start_mark[j];
            
            sprintf(file_path, "view/signal_view/text/ch%d/%s_%d.json", j, file_name, i);
            fp = fopen(file_path, "w");
            fprintf(fp, "{\"Name\": \"%s_%d\", \"Sample size\": 2, \"Frame num\": %d, \"Sec\": 18.91625850340136, \"Channel num\": 1, \"Sampling rate\": 44100, \"Prams\": [2, 2, 44100, 834207, \"NONE\", \"not compressed\"]}", file_name, i, block_size);
            fflush(fp);
            fclose(fp);
            
            sprintf(file_path, "view/signal_view/text/ch%d/%s_%d.txt", j, file_name, i);
            fp = fopen(file_path, "w");
            for(k = 0; k < block_size; k++) {
                fprintf(fp, "%d\n", pointer[k]);
            }
            fflush(fp);
            fclose(fp);
            
            sprintf(plot_name, "view/signal_view/image/ch%d/%s_%d.png", j, file_name, i);
            GPlot2D plot = init_GPlot2D(1, "sample", "amplitude", 0, 0, -AMP_SIZE, AMP_SIZE, "png", plot_name, file_path, "with line");
            output_2D_gnuplot(plot);
        }
    }
}
void block_separation_view(MultiBlock data[], int size, int nchannels, int view_range, char *file_name) {
    int i, j, k;
    int range;
    int *pointer;
    FILE *fp;
    char file_path[256], plot_name[256];
    
    for(i = 0; i <= size; i++) {
        if(i == 0 || i == size) {
            range = view_range;
        } else {
            range = 2*view_range;
        }
        
        for(j = 0; j < nchannels; j++) {
            if(i == size) {
                pointer = data[i-1].start_mark[j] + data[i-1].size - range;
            } else {
                pointer = data[i].start_mark[j];
                if(i != 0) {
                    pointer -= view_range - 1;
                }
            }
            
            sprintf(file_path, "view/separation_view/text/ch%d/%s_%d.txt", j, file_name, i);
            fp = fopen(file_path, "w");
            for(k = 0; k < range; k++) {
                fprintf(fp, "%d\n", pointer[k]);
            }
            fflush(fp);
            fclose(fp);
            
            sprintf(plot_name, "view/separation_view/image/ch%d/%s_%d.png", j, file_name, i);
            GPlot2D plot = init_GPlot2D(1, "sample", "amplitude", 0, 0, 0, 0, "png", plot_name, file_path, "with boxes");
            output_2D_gnuplot(plot);
        }
    }
}
void block_histogram_view(MultiBlock data[], int size, int nchannels, char *file_name) {    int i, j, k;
    int block_size;
    int amplitude_count[AMP_SIZE];
    int *pointer;
    FILE *fp;
    char file_path[256], plot_name[256];
    
    for(i = 0; i < size; i++) {
        block_size = data[i].size;
        
        for(j = 0; j < nchannels; j++) {
            for(int amp = 0; amp < AMP_SIZE; amp++) {
                amplitude_count[amp] = 0;
            }
            
            pointer = data[i].start_mark[j];
            for(k = 0; k < block_size; k++) {
                int amp = abs(pointer[k]);
                amplitude_count[amp]++;
            }
            
            sprintf(file_path, "view/histogram_view/text/ch%d/%s_%d.txt", j, file_name, i);
            fp = fopen(file_path, "w");
            for(int amp = 0; amp < AMP_SIZE; amp++) {
                fprintf(fp, "%d\n", amplitude_count[amp]);
            }
            fflush(fp);
            fclose(fp);
            
            sprintf(plot_name, "view/histogram_view/image/ch%d/%s_%d.png", j, file_name, i);
            GPlot2D plot = init_GPlot2D(1, "amplitude", "count", 0, 0, 0, 0, "png", plot_name, file_path, "with boxes");
            output_2D_gnuplot(plot);
        }
    }
}
