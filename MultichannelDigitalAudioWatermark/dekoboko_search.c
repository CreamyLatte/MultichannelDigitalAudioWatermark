//
//  dekoboko_search.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/12/09.
//

#include "dekoboko_search.h"

//#define El 1000
//#define Eh 2000

int *create_histgram_with_SoundBlock(SoundBlock block) {
    int *histgram = (int*)malloc(sizeof(int) * ENCODING_BIT_RATE);
    if(histgram == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.\n", __func__);
        exit(EXIT_FAILURE);
    }
    
    int i;
    for(i = 0; i < ENCODING_BIT_RATE; i++) {
        histgram[i] = 0;
    }
    for(i = 0; i < block.size; i++) {
        histgram[block.start_mark[i] + ZERO_AMP] += 1;
    }
    
    return histgram;
}

int similar_dekoboko_check(int *histgram, int i, int th, int search_deko_flag) {
    if(search_deko_flag) {
        //デコサーチ
        if(histgram[i] >= th && histgram[i+2] >= th && histgram[i] < histgram[i+1] && histgram[i+2] < histgram[i+1]) {
            return 1;
        }
    } else {
        //ボコサーチ
        if(histgram[i+1] >= th && histgram[i] > histgram[i+1] && histgram[i+2] > histgram[i+1]) {
            return 1;
        }
    }
    return 0;
}

int search_similar_dekoboko(int *histgram, int search_deko_flag) {
    int th = 10;
    int i;
    for(th = 10; th > 0; th--) {
        for(i = 1; i < ZERO_AMP-2; i++) {
            int negative_point = ZERO_AMP-(2+i);
            int positive_point = ZERO_AMP+i;
            if((similar_dekoboko_check(histgram, negative_point, th, search_deko_flag) == 1) && (similar_dekoboko_check(histgram, positive_point, th, search_deko_flag) == 1)) {
                return i;
            }
        }
    }
    return -1;
}

int dekoboko_check(int *histgram, int i, int th, int search_deko_flag) {
    if(search_deko_flag) {
        //デコサーチ
        if(histgram[i] >= th && histgram[i] < histgram[i+1] && histgram[i+2] < histgram[i+1] && histgram[i] == histgram[i+2]) {
            return 1;
        }
    } else {
        //ボコサーチ
        if(histgram[i+1] >= th && histgram[i] > histgram[i+1] && histgram[i+2] > histgram[i+1] && histgram[i] == histgram[i+2]) {
            return 1;
        }
    }
    return 0;
}

int search_dekoboko(int *histgram, int search_deko_flag) {
    int th = 10;
    int i;
    for(th = 10; th > 0; th--) {
        for(i = 1; i < ZERO_AMP-2; i++) {
            int negative_point = ZERO_AMP-(2+i);
            int positive_point = ZERO_AMP+i;
            if((dekoboko_check(histgram, negative_point, th, search_deko_flag) == 1) && (dekoboko_check(histgram, positive_point, th, search_deko_flag) == 1)) {
                return i;
            }
        }
    }
    return -1;
}

void create_dekoboko(SoundBlock block, int *histgram, int create_point) {
    int i, j; //loop
    int point = create_point;
    int moving_distance;
//    int sequence_number = block.sequence_number;
    int *sig_data = block.start_mark;
    int **target_sample = (int**)malloc(sizeof(int*) * block.size);
    int target_count = 0;
    if(target_sample == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    moving_distance = histgram[point] - histgram[point+2];
    if(moving_distance < 0) {
        point += 2;
    }
    
    for(i = 0; i < block.size; i++) {
        if(sig_data[i]+ZERO_AMP==point) {
            target_sample[target_count] = &sig_data[i];
            target_count++;
        }
    }
    
    for(i = 0; i < abs(moving_distance); i++) {
        int move_sample_index = 0;//rand() % target_count;
        if(moving_distance < 0) {
            *target_sample[move_sample_index] += 1;
        } else {
            *target_sample[move_sample_index] -= 1;
        }
        for(j = move_sample_index; j < target_count; j++) {
            target_sample[j] = target_sample[j+1];
        }
        target_count--;
    }
    free(target_sample);
    
}

SoundData dekoboko_embed(SoundData original_data) {
    SoundData stego_data = soundData_copy(original_data);
    
    int i, j; //loop
    int nchannels = stego_data.nchannels;
    int *histgram;
    int *mulch_blocks_size = (int*)malloc(sizeof(int) * nchannels);
    if(mulch_blocks_size == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    SoundBlock **mulch_blocks = (SoundBlock**)malloc(sizeof(SoundBlock*) * nchannels);
    if(mulch_blocks == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < nchannels; i++) {
        SoundBlock *blocks = (SoundBlock*)malloc(sizeof(SoundBlock) * BLOCK_N);
        if(blocks == NULL) {
            fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }
        mulch_blocks[i] = blocks;
    }
    
    //block分割
    for(i = 0; i < nchannels; i++) {
        mulch_blocks_size[i] = block_separation(stego_data.data[i], mulch_blocks[i], stego_data.sample_size, F, El, Eh);
    }
    FILE *ofp = fopen("block_size.csv", "w");
    for(i = 0; i < nchannels; i++) {
        fprintf(ofp, "%d ch,", i+1);
        for(j = 0; j < mulch_blocks_size[i]; j++) {
            fprintf(ofp, "%d,", mulch_blocks[i][j].size);
        }
        fprintf(ofp, "\n");
    }
    fclose(ofp);
    
    //透かし埋め込み
    for(i = 0; i < nchannels; i++) {
        SoundBlock *blocks = mulch_blocks[i];
        int blocks_size = mulch_blocks_size[i];
        printf("ch:%d, blocks_size: %d\n", i, blocks_size);
        for(j = 0; j < blocks_size; j++) {
            SoundBlock block = blocks[j];
//            printf("\tj: %3d, block.size: %7d\n", j, block.size);
            histgram = create_histgram_with_SoundBlock(block);
            int point = search_similar_dekoboko(histgram, (i+j)%2);
            if(point == -1) {
                fprintf(stderr, "埋め込み可能な箇所が見つかりませんでした。(ch: %d, b_i: %d, embed: %s)\n", i, j, (i+j)%2?"凸":"凹");
                free(histgram);
                continue;
            }
            
            int negative_point = ZERO_AMP-(2+point);
            int positive_point = ZERO_AMP+point;
//            fprintf(stderr, "n_p: %d, %d, %d\np_p: %d, %d, %d\n", histgram[negative_point], histgram[negative_point+1], histgram[negative_point+2], histgram[positive_point], histgram[positive_point+1], histgram[positive_point+2]);
            create_dekoboko(block, histgram, negative_point);
            create_dekoboko(block, histgram, positive_point);
            free(histgram);
            histgram = create_histgram_with_SoundBlock(block);
            if(search_dekoboko(histgram, (i+j)%2) == -1) {
                fprintf(stderr, "透かしが正しく作成されませんでした。\nn_p: %d, %d, %d\np_p: %d, %d, %d\n", histgram[negative_point], histgram[negative_point+1], histgram[negative_point+2], histgram[positive_point], histgram[positive_point+1], histgram[positive_point+2]);
                free(histgram);
                exit(EXIT_FAILURE);
            }
            free(histgram);
        }
    }
    
    //free
    free(mulch_blocks_size);
    for(i = 0; i < nchannels; i++) {
        free(mulch_blocks[i]);
    }
    free(mulch_blocks);
    
    return stego_data;
}

void dekoboko_tamper_detection(SoundData source_data, const char *result_file_path) {
    int i, j; //loop
    //int result = 0;
    int nchannels = source_data.nchannels;
    int *histgram;
    int *mulch_blocks_size = (int*)malloc(sizeof(int) * nchannels);
    if(mulch_blocks_size == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    SoundBlock **mulch_blocks = (SoundBlock**)malloc(sizeof(SoundBlock*) * nchannels);
    if(mulch_blocks == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < nchannels; i++) {
        SoundBlock *blocks = (SoundBlock*)malloc(sizeof(SoundBlock) * BLOCK_N);
        if(blocks == NULL) {
            fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }
        mulch_blocks[i] = blocks;
    }
    
    /*
     ## dekoboko_list
     sourece_dataの凸凹を記録する二次元配列
     # value
     - 1:凸
     - 2:凹
     - 3:凸凹両方
     - -1:凸凹未検出
     - 0:未検査のblock
     */
    int **dekoboko_list = (int**)malloc(sizeof(int*) * nchannels);
    if(dekoboko_list == NULL) {
        fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < nchannels; i++) {
        int *pointer = (int*)calloc(BLOCK_N+1, sizeof(int));
        if(pointer == NULL) {
            fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }
        dekoboko_list[i] = pointer;
    }
    
    //block分割
    for(i = 0; i < nchannels; i++) {
        mulch_blocks_size[i] = block_separation(source_data.data[i], mulch_blocks[i], source_data.sample_size, F, El, Eh);
    }
    
    //改ざん検出
    for(i = 0; i < nchannels; i++) {
        SoundBlock *blocks = mulch_blocks[i];
        int blocks_size = mulch_blocks_size[i];
        printf("ch:%d, blocks_size: %d\n", i, blocks_size);
        for(j = 0; j < blocks_size; j++) {
            SoundBlock block = blocks[j];
            histgram = create_histgram_with_SoundBlock(block);
            if(search_dekoboko(histgram, 1) != -1) {
                dekoboko_list[i][j] += 1;
            }
            if(search_dekoboko(histgram, 0) != -1) {
                dekoboko_list[i][j] += 2;
            }
            if(dekoboko_list[i][j] == 0) {
                dekoboko_list[i][j] = -1;
            }
            free(histgram);
        }
    }
    
    //一旦表示
    FILE *ofp = fopen(result_file_path, "w");
    int max_blocks_size = 0;
    for(i = 0; i < nchannels; i++) {
        if(mulch_blocks_size[i] > max_blocks_size) {
            max_blocks_size = mulch_blocks_size[i];
        }
    }
    fprintf(ofp, "ch,");
    for(i = 0; i < max_blocks_size; i++) {
        fprintf(ofp, "%d,", i);
    }
    fprintf(ofp, "\n");
    for(i = 0; i < nchannels; i++) {
        fprintf(ofp, "%d ch,", i+1);
        int blocks_size = mulch_blocks_size[i];
        for(j = 0; j < blocks_size; j++) {
            switch (dekoboko_list[i][j]) {
                case 1:
                    fprintf(ofp, "%s,", "凸");
                    break;
                case 2:
                    fprintf(ofp, "%s,", "凹");
                    break;
                case 3:
                    fprintf(ofp, "%s,", "凸凹");
                    break;
                case -1:
                    fprintf(ofp, "%s,", "!");
                    break;
                case 0:
                    fprintf(ofp, "%s,", "-");
                    break;
                default:
                    fprintf(stderr, "未定義のデータ検出(ch: %d, b_n: %d)\n", i, j);
                    exit(EXIT_FAILURE);
                    break;
            }
        }
        fprintf(ofp, "\n");
    }
    fflush(ofp);
    fclose(ofp);
    
    //free
    free(mulch_blocks_size);
    for(i = 0; i < nchannels; i++) {
        free(mulch_blocks[i]);
        free(dekoboko_list[i]);
    }
    free(mulch_blocks);
    free(dekoboko_list);

    return;
}
