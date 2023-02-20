//
//  data_tools.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "data_tools.h"
void data_reader(const char *file_path, int *out_data, int size) {
    int i, flag = 0;
    FILE *fp = fopen(file_path, "r");
    if(fp == NULL) {
        fprintf(stderr, "read_datafile() error!\nFile path has not been entered.\n");
        //プロセス終了
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < size; i++) {
        if(fscanf(fp, "%d", out_data) == EOF || flag) {
            *out_data = 0;
            flag = 1;
        }
        out_data++;
    }
    fclose(fp);
}
void data_writer(const char *file_path, int *in_data, int size) {
    int i;
    FILE *fp = fopen(file_path, "w");
    if(fp == NULL) {
        fprintf(stderr, "data_writer() error!\nCould not open the \"%s\" in write mode.\n", file_path);
        return;
    }
    for(i = 0; i < size; i++) {
        fprintf(fp, "%d\n", *in_data);
        in_data++;
    }
    fclose(fp);
}
SoundData soundData_reader(const char *file_path, int nchannels, int sample_size, int sample_rate) {
    int i,j;
    int *raw_data = (int*)calloc(nchannels * sample_size, sizeof(int));
    if(raw_data == NULL) {
        fprintf(stderr, "soundData_reader() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    data_reader(file_path, raw_data, nchannels*sample_size);
    int **datas = (int**)malloc(sizeof(int*) * nchannels), *data = NULL;
    if(datas == NULL) {
        fprintf(stderr, "soundData_reader() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < nchannels; i++) {
        data = (int*)malloc(sizeof(int) * sample_size);
        if(data == NULL) {
            fprintf(stderr, "soundData_reader() error! Could not allocate memory.\n");
            exit(EXIT_FAILURE);
        }
        for(j = 0; j < sample_size; j++) {
            data[j] = raw_data[i+j*nchannels];
        }
        datas[i] = data;
    }
    free(raw_data);
    raw_data = NULL;
    
    SoundData sound_data = {
        .data = datas,
        .nchannels = nchannels,
        .sample_size = sample_size,
        .sample_rate = sample_rate
    };
    return sound_data;
}
SoundData soundData_copy(SoundData source) {
    int i, j;
    int **datas = (int**)malloc(sizeof(int*) * source.nchannels), *data = NULL;
    if(datas == NULL) {
        fprintf(stderr, "soudData_copy() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < source.nchannels; i++) {
        data = (int*)malloc(sizeof(int) * source.sample_size);
        if(data == NULL) {
            fprintf(stderr, "soundData_reader() error! Could not allocate memory.\n");
            exit(EXIT_FAILURE);
        }
        for(j = 0; j < source.sample_size; j++) {
            data[j] = source.data[i][j];
        }
        datas[i] = data;
    }
    SoundData sound_data = {
        .data = datas,
        .nchannels = source.nchannels,
        .sample_size = source.sample_size,
        .sample_rate = source.sample_rate
    };
    return sound_data;
}
void soundData_free(SoundData data) {
    int i;
    for(i = 0; i < data.nchannels; i++) {
        free(data.data[i]);
        data.data[i] = NULL;
    }
    free(data.data);
    data.data = NULL;
}
void soundData_writer(const char *file_name, SoundData data) {
    char *file_path = (char*)alloca(sizeof(char) * (strlen(file_name) + 10));
    if(file_path == NULL) {
        fprintf(stderr, "soundData_writer() error! Could not allocate memory.\n");
        return;
    }
    char *main_file_path = (char*)alloca(sizeof(char) * (strlen(file_name) + 10));
    if(main_file_path == NULL) {
        fprintf(stderr, "soundData_writer() error! Could not allocate memory.\n");
        return;
    }
    sprintf(main_file_path, "%s.txt", file_name);
    int i, j;
    for(i = 0; i < data.nchannels; i++) {
        sprintf(file_path, "%s_ch%d.txt", file_name, i);
        data_writer(file_path, data.data[i], data.sample_size);
    }
    FILE *ofp = fopen(main_file_path, "w");
    for(i = 0; i < data.sample_size; i++) {
        for(j = 0; j < data.nchannels; j++) {
            fprintf(ofp, "%d\n", data.data[j][i]);
        }
    }
    fclose(ofp);
    
}
int *arrayInt_copy(int *source, int size) {
    int *data = (int*)malloc(sizeof(int) * size);
    if(data == NULL) {
        fprintf(stderr, "arrayInt_copy() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0; i < size; i++) {
        data[i] = source[i];
    }
    return data;
}
SoundData sound_cut(SoundData data, int cut_index_start, int cut_frame) {
    int i, j;
    SoundData new_data = data;
    int new_sample_size = data.sample_size - cut_frame;
    
    //引数チェック
    if(data.sample_size < cut_index_start+cut_frame || cut_index_start < 0 || cut_frame < 0) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return data;
    }
    
    //切り取り
    for(i = 0; i < data.nchannels; i++) {
        for(j = cut_index_start; j < new_sample_size; j++) {
            new_data.data[i][j] = data.data[i][j+cut_frame];
        }
        new_data.data[i] = realloc(new_data.data[i], sizeof(int)*new_sample_size);
        if(new_data.data[i] == NULL) {
            fprintf(stderr, "%s() error! Could not reallocate memory.\n", __func__);
            exit(EXIT_FAILURE);
        }
    }
    new_data.sample_size = new_sample_size;
    
    return new_data;
}
SoundData sound_cut_random(SoundData data, int cut_frame, FILE *fp) {
    int cut_index_start;
    
    //引数チェック
    if(data.sample_size < cut_frame) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return data;
    }
    
    //乱数指定
    cut_index_start = rand() % (data.sample_size - cut_frame);
    printf("cut index: %d ~ %d\n", cut_index_start, cut_index_start+cut_frame);
    if(fp != NULL) {
        fprintf(fp, "%d %d\n", cut_index_start, cut_index_start+cut_frame);
        fflush(fp);
    }
    
    //音声切り取り
    return sound_cut(data, cut_index_start, cut_frame);
}
void sound_noise_addition(SoundData data, int start_index, int frame, int noise_amp) {
    int i, j;
    int nchannels = data.nchannels;
    
    //引数チェック
    if(data.sample_size < start_index+frame || start_index < 0 || frame < 0) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return;
    }
    
    //雑音付加
    for(i = start_index; i < start_index+frame; i++) {
        for(j = 0; j < nchannels; j++) {
            int noise = rand() % noise_amp*2 - noise_amp;
            data.data[j][i] += noise;
            if(data.data[j][i] > 32767) {
                data.data[j][i] = 32767;
            }
            if(data.data[j][i] < -32768) {
                data.data[j][i] = -32768;
            }
        }
    }
}
void sound_noise_addition_ramdom(SoundData data, int frame, int noise_amp, FILE *fp) {
    int start_index;
    
    //引数チェック
    if(data.sample_size < frame) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return;
    }
    
    //乱数指定
    start_index = rand() % (data.sample_size - frame);
    printf("noise additional index: %d ~ %d\n", start_index, start_index+frame);
    if(fp != NULL) {
        fprintf(fp, "%d %d\n", start_index, start_index+frame);
        fflush(fp);
    }
    
    //雑音付加
    sound_noise_addition(data, start_index, frame, noise_amp);
}
void soundch_noise_addition(SoundData data, int ch, int start_index, int frame, int noise_amp) {
    int i;
    
    //引数チェック
    if(data.sample_size < start_index+frame || start_index < 0 || frame < 0 || !(ch < data.nchannels && ch >= 0)) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return;
    }
    
    //雑音付加
    for(i = start_index; i < start_index+frame; i++) {
        int noise = rand() % noise_amp*2 - noise_amp;
        data.data[ch][i] += noise;
        if(data.data[ch][i] > 32767) {
            data.data[ch][i] = 32767;
        }
        if(data.data[ch][i] < -32768) {
            data.data[ch][i] = -32768;
        }
    }
}
void soundch_noise_addition_ramdom(SoundData data, int ch, int frame, int noise_amp, FILE *fp) {
    int start_index;
    
    //引数チェック
    if(data.sample_size < frame) {
        fprintf(stderr, "%s() error! The parameters are incorrect. Check the parameters.\n", __func__);
        return;
    }
    
    //乱数指定
    start_index = rand() % (data.sample_size - frame);
    printf("noise additional index: %d ~ %d\n", start_index, start_index+frame);
    if(fp != NULL) {
        fprintf(fp, "%d %d\n", start_index, start_index+frame);
        fflush(fp);
    }
    
    //雑音付加
    soundch_noise_addition(data, ch, start_index, frame, noise_amp);
}
double snr(SoundData first_data, SoundData second_data) {
    //引数チェック
    if(first_data.nchannels != second_data.nchannels || first_data.sample_size != second_data.sample_size) {
        fprintf(stderr, "%s() error! Data that cannot be compared.\n", __func__);
        return NAN;
    }
    
    int nchannels = first_data.nchannels;
    int sample_size = first_data.sample_size;
    double result = 0.0;
    double *ch_result = (double*)malloc(sizeof(double) * nchannels);
    double *ch_ave = (double*)malloc(sizeof(double) * nchannels);
    double *noise_ave = (double*)malloc(sizeof(double) * nchannels);
    double *ch_rms = (double*)malloc(sizeof(double) * nchannels);
    double *noise_rms = (double*)malloc(sizeof(double) * nchannels);
    
    int i, j;
    for(i = 0; i < nchannels; i++) {
        ch_ave[i] = 0;
        noise_ave[i] = 0;
        for(j = 0; j < sample_size; j++) {
            ch_ave[i] += first_data.data[i][j];
            noise_ave[i] += first_data.data[i][j] - second_data.data[i][j];
        }
        ch_ave[i] /= (double)sample_size;
        noise_ave[i] /= (double)sample_size;
    }
    for(i = 0; i < nchannels; i++) {
        ch_rms[i] = 0.0;
        noise_rms[i] = 0.0;
        for(j = 0; j < sample_size; j++) {
            double rms = (double)first_data.data[i][j] - ch_ave[i];
            ch_rms[i] += rms * rms;
            rms = (double)first_data.data[i][j] - (double)second_data.data[i][j] - noise_ave[i];
            noise_rms[i] += rms * rms;
        }
        ch_rms[i] /= (double)sample_size;
        ch_rms[i] = sqrt(ch_rms[i]);
        noise_rms[i] /= (double)sample_size;
        noise_rms[i] = sqrt(noise_rms[i]);
        
        ch_result[i] = ch_rms[i] / noise_rms[i];
        //デシベル変換
        if(ch_result[i] == 0.0) {
            ch_result[i] = NAN;
        }
        ch_result[i] = 20.0 * log10(ch_result[i]);
        result += ch_result[i];
        printf("ch %d, SNR: %.2lf [dB]\n", i+1, ch_result[i]);
    }
    result /= (double)nchannels;
    printf("average: %.2lf [dB]\n", result);
    
    free(ch_result);
    free(ch_ave);
    free(noise_ave);
    free(ch_rms);
    free(noise_rms);
    return result;
}
