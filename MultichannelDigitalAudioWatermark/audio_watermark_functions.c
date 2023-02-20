//
//  audio_watermark_functions.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/10.
//

#include <stdio.h>
#include "audio_watermark_functions.h"

int wave_energie(int *start_data, int frame) {
    int i, amp_sum = 0;
    for(i = 0; i < frame; i++) {
        amp_sum += abs(start_data[i]);
    }
    return amp_sum / frame;
}
int get_random(int min,int max) {
    return min+(int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}
int level_selection(int energie, int threshold_low, int threshold_high) {
    int level = 0;
    if(energie > threshold_low) {
        level++;
    }
    if(energie > threshold_high) {
        level++;
    }
    return level;
}
void energyData_free(EnergyData free_data) {
    free(free_data.signal_energie);
    return;
}
EnergyData energie_setting(int *mono_data, int data_size, int frame, FILE *out_stream) {
    int i; // loop
    
    int energie;
    double e_ave = 0.0; // エネルギー平均
    int e_min = AMP_SIZE, e_max = 0;
    
    // 結果出力用
    int energie_low;
    int energie_high;
    
    //モノラルデータの信号エネルギ取得
    int *signal_energie = (int*)malloc(sizeof(int) * data_size - frame);
    for(i = 0; i < data_size-frame; i++) {
        energie = wave_energie(&mono_data[i], frame);
        signal_energie[i] = energie;
        e_ave += (double)energie/(data_size-frame);
        if(energie < e_min) {
            e_min = energie;
        }
        if(energie > e_max) {
            e_max = energie;
        }
    }
    
    //energie_low, energie_high設定
    energie_low = (0.03*(e_max-e_min)+e_min < 4*e_min) ? 0.03*(e_max-e_min)+e_min : 4*e_min;
    energie_low = (energie_low / 512 + (energie_low%512!=0)) * 512;
    energie_high = (int)e_ave;
    energie_high = (energie_high/ 512 + (energie_high%512!=0)) * 512;
    if(energie_high <= energie_low) {
        energie_high = energie_low + 512;
    }
    if (out_stream != NULL) {
        fprintf(out_stream, "energie_low: %d, energie_high%d\n", energie_low, energie_high);
    }
    
    return (EnergyData) {signal_energie, energie_low, energie_high};
}
int block_separation(int *in_data, SoundBlock *out_sound_blocks, int data_size, int frame, int energie_low, int energie_high) {
    int i; //loop
    int block_count = 0, sample_count = 0;
    int Ax, Px; //frame energie
    int Ax_level, Px_level;
    int exists_voice = 0; //bool
    
    out_sound_blocks[block_count].start_mark = in_data;
    out_sound_blocks[block_count].sequence_number = block_count + 1;
    
    for(i = 1; i <= data_size-2*frame; i++) {
        Ax = wave_energie(&in_data[i], frame);
        Ax_level = level_selection(Ax, energie_low, energie_high);
        Px = wave_energie(&in_data[i+frame], frame);
        Px_level = level_selection(Px, energie_low, energie_high);
        sample_count++;
        if(exists_voice) {
            if(Ax_level == 1 && Px_level == 0) {
                exists_voice = 0;
                out_sound_blocks[block_count].size = sample_count;
                block_count++;
                out_sound_blocks[block_count].start_mark = &in_data[i];
                out_sound_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        } else {
            if(Ax_level == 1 && Px_level == 2) {
                exists_voice = 1;
                out_sound_blocks[block_count].size = sample_count;
                block_count++;
                out_sound_blocks[block_count].start_mark = &in_data[i];
                out_sound_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        }
    }
    out_sound_blocks[block_count].size = sample_count + 2 * frame;
    out_sound_blocks[block_count].sequence_number = block_count + 1;
    block_count++;
    
    return block_count;
}
int auto_block_separation(int *in_data, SoundBlock *out_sound_blocks, int data_size, int frame) {
    int i; //loop
    int block_count = 0, sample_count = 0;
    int Ax, Px; //frame energie
    int Ax_level, Px_level;
    int exists_voice = 0; //bool
    //エネルギー敷地の設定
    EnergyData threshold = energie_setting(in_data, data_size, frame, stdout);
    int *signal_energie = threshold.signal_energie;
    int energie_low = threshold.low;
    int energie_high = threshold.high;
    
    out_sound_blocks[block_count].start_mark = in_data;
    out_sound_blocks[block_count].sequence_number = block_count + 1;
    
    for(i = 1; i <= data_size-2*frame; i++) {
        Ax = signal_energie[i];
        Ax_level = level_selection(Ax, energie_low, energie_high);
        Px = signal_energie[i+F];
        Px_level = level_selection(Px, energie_low, energie_high);
        sample_count++;
        if(exists_voice) {
            if(Ax_level == 1 && Px_level == 0) {
                exists_voice = 0;
                out_sound_blocks[block_count].size = sample_count;
                block_count++;
                out_sound_blocks[block_count].start_mark = &in_data[i];
                out_sound_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        } else {
            if(Ax_level == 1 && Px_level == 2) {
                exists_voice = 1;
                out_sound_blocks[block_count].size = sample_count;
                block_count++;
                out_sound_blocks[block_count].start_mark = &in_data[i];
                out_sound_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        }
    }
    out_sound_blocks[block_count].size = sample_count + 2 * frame;
    out_sound_blocks[block_count].sequence_number = block_count + 1;
    block_count++;
    
    energyData_free(threshold);
    return block_count;
}
int *monaural_converter(SoundData in_data) {
    if(in_data.data == NULL) {
        fprintf(stderr, "stereo_monaural_conversion() error!\nin_data.dataの値がNULLです。\n");
    }
    
    int *mono_data = (int*)malloc(sizeof(int) * in_data.sample_size);
    if(mono_data == NULL) {
        fprintf(stderr, "monaural_converter() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    
    int i, j;
    for(i = 0; i < in_data.sample_size; i++) {
        double tmp_data = 0.0;
        for(j = 0; j < in_data.nchannels; j++) {
            tmp_data += in_data.data[j][i] / in_data.nchannels;
        }
        mono_data[i] = (int)tmp_data;
    }
    
    return mono_data;
}
/// ブロック分割地点のポインタを記録
///
/// ブロック分割位置の始点のポインタを記録する
/// @Attention この関数の返り値は必ずmultiBlocks_free()でメモリを解放してください。
/// @param in_data 音声データ
/// @param mark_frame ブロック分割点のインデックス
/// @return MultiBlock型のブロック分割情報
MultiBlock multi_block_marker(SoundData in_data, int mark_frame) {
    MultiBlock mark_point;
    
    mark_point.start_mark = (int**)malloc(sizeof(int*) * in_data.nchannels);
    if(mark_point.start_mark == NULL) {
        fprintf(stderr, "multi_block_marker() error! Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0; i < in_data.nchannels; i++) {
        mark_point.start_mark[i] = &in_data.data[i][mark_frame];
    }
    
    return mark_point;
}
int multi_block_separation(SoundData in_data, MultiBlock *out_stereo_blocks, int frame) {
    //パラメータ
    int i; //loop
    int block_count = 0, sample_count = 0;
    int Ax, Px; //frame energie
    int Ax_level, Px_level;
    int exists_voice = 0; //bool
    int data_size = in_data.sample_size;
    int *mono_data = monaural_converter(in_data); //Dynamic memory
    
    //エネルギー敷地の設定
    EnergyData threshold = energie_setting(mono_data, data_size, frame, stdout);
    int *signal_energie = threshold.signal_energie;
    int energie_low = threshold.low;
    int energie_high = threshold.high;
    
    out_stereo_blocks[block_count] = multi_block_marker(in_data, 0);
    out_stereo_blocks[block_count].sequence_number = block_count + 1;
    
    for(i = 1; i <= data_size-2*frame; i++) {
        Ax = signal_energie[i];
        Ax_level = level_selection(Ax, energie_low, energie_high);
        Px = signal_energie[i+F];
        Px_level = level_selection(Px, energie_low, energie_high);
        sample_count++;
        if(exists_voice) {
            if(Ax_level == 1 && Px_level == 0) {
                exists_voice = 0;
                out_stereo_blocks[block_count].size = sample_count;
                block_count++;
                out_stereo_blocks[block_count] = multi_block_marker(in_data, i);
                out_stereo_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        } else {
            if(Ax_level == 1 && Px_level == 2) {
                exists_voice = 1;
                out_stereo_blocks[block_count].size = sample_count;
                block_count++;
                out_stereo_blocks[block_count] = multi_block_marker(in_data, i);
                out_stereo_blocks[block_count].sequence_number = block_count + 1;
                sample_count = 0;
            }
        }
    }
    out_stereo_blocks[block_count].size = sample_count + 2 * frame;
    out_stereo_blocks[block_count].sequence_number = block_count + 1;
    block_count++;
    
    free(mono_data);
    energyData_free(threshold);
    return block_count;
}
void multiBlocks_free(MultiBlock *free_data, int size) {
    int i;
    for(i = 0; i < size; i++) {
        free(free_data[i].start_mark);
        free_data[i].start_mark = NULL;
    }
}
int sosu_check(int num) { // return -> bool
    int i;
    int sosu_base[4] = {2, 3, 5, 7};
    for(i = 0; i < 4; i++) {
        if(num % sosu_base[i] == 0) {
            return 0;
        }
    }
    return 1;
}
int block_sum(SoundBlock data) {
    int i;
    int sum = 0;
    for(i = 0; i < data.size; i++) {
        sum += data.start_mark[i];
    }
    return sum;
}
int block_sign_sum(SoundBlock data, int sign) {
    int i;
    int sum = 0;
    for(i = 0; i < data.size; i++) {
        int value = data.start_mark[i];
        if((value < 0) == sign) {
            sum += value;
        }
    }
    return abs(sum);
}
int array_sum(int *data, int size) {
    if(data == NULL) {
        fprintf(stderr, "array_sum() error! data is NULL.\n");
        return -1;
    }
    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum;
}
int array_sign_sum(int *data, int size, int sign) {
    if(data == NULL) {
        fprintf(stderr, "array_sum() error! data is NULL.\n");
        return -1;
    }
    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
        int value = data[i];
        if((value < 0) == sign) {
            sum += value;
        }
    }
    return abs(sum);
}
EmbedArea search_embed_area(SoundBlock block, int search_flag) {
    int signal;
    //block_sum_negativeを取得
    int block_sum_negative = search_flag;
    
    //embed_area_size, embed_start_index, embed_end_index探索
    int start_index, end_index, area_size;
    //Search start index
    for(start_index = 0;; start_index++) {
        signal = block.start_mark[start_index];
        if((signal < 0) == block_sum_negative) {
            break;
        }
        if(start_index >= block.size - 2) {
            fprintf(stderr, "%s()<Line: %d> error. Cannot set embedding start position.\nstart_index: %6d, block.size: %6d\n", __func__, __LINE__, start_index, block.size);
            exit(EXIT_FAILURE);
        }
        start_index++;
    }
    for(start_index++;; start_index++) {
        signal = block.start_mark[start_index];
        if(signal >= 0 == block_sum_negative) {
            break;
        }
        if(start_index >= block.size - 2) {
            fprintf(stderr, "%s()<Line: %d> error. Cannot set embedding start position.\nstart_index: %6d, block.size: %6d\n", __func__, __LINE__, start_index, block.size);
            exit(EXIT_FAILURE);
        }
    }
    start_index++;
    //search end point.
    for(end_index = block.size-1;; end_index--) {
        signal = block.start_mark[end_index];
        if(signal < 0 == block_sum_negative) {
            break;
        }
        if(end_index <= start_index) {
            fprintf(stderr, "%s()<Line: %d> error. Cannot set embedding end position.\nend_index: %6d, start_index: %6d\n", __func__, __LINE__, end_index, start_index);
            exit(EXIT_FAILURE);
        }
    }
    for(end_index--;;end_index--) {
        signal = block.start_mark[end_index];
        if(signal >= 0 == block_sum_negative) {
            break;
        }
        if(end_index <= start_index) {
            fprintf(stderr, "%s()<Line: %d> error. Cannot set embedding end position.\nend_index: %6d, start_index: %6d\n", __func__, __LINE__, end_index, start_index);
            exit(EXIT_FAILURE);
        }
    }
    end_index--;
    //Calculate area size.
    area_size = end_index - start_index + 1;
    if(area_size <= 0) {
        fprintf(stderr, "%s()<Line: %d> error. The range required for embedding could not be secured.\nstart_index: %6d, end_index: %6d, area_size: %6d\n", __func__, __LINE__, start_index, end_index, area_size);
        exit(EXIT_FAILURE);
    }
    return (EmbedArea) {.start_index = start_index, .end_index = end_index, .size = area_size};
}

int blocks_search_refer_prime_index(SoundBlock *blocks, int search_sample_index) {
    int sample_count = 0;
    
    int i;
    for (i = 0; ; i++) {
        if (search_sample_index >= sample_count && search_sample_index < sample_count + blocks[i].size) {
            break;
        }
        sample_count += blocks[i].size;
    }
    
    return i;
}

int defacing_record(DefacingBlock *record, int defacing_count, int start_sample, int end_sample) {
    if(defacing_count != 0) {
        if(record[defacing_count-1].end_sample + 1 == start_sample) {
            record[defacing_count-1].end_sample = end_sample;
            return defacing_count;
        }
    }
    record[defacing_count].start_sample = start_sample;
    record[defacing_count].end_sample = end_sample;
    return defacing_count+1;
}

int max(int *data, int size) {
    int result = 0;
    int i;
    for (i = 0; i < size; i++) {
        if (data[i] > result) {
            result = data[i];
        }
    }
    return result;
}
