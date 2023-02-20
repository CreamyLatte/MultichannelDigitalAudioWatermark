//
//  main.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/09.
//

#include <stdio.h>
#include <stdlib.h>
#include "Test.h"
#include "audio_watermark_functions.h"

int main(void) {
    srand((unsigned int)time(NULL));//時間ごとで乱数の種を生成
    int mode = 7;
    
    while(1) {
        printf("mode(1. 埋め込み試験, 2. 改ざん検知試験, 3.embed_test_hybrid, 4.defacing_detection_test_hybrid) -> 0\n");
        //        scanf("%d", &mode);
//        mode = 0;
        if(mode < 0 || mode > 7) {
            printf("有効な数字を入れてください\n");
        } else {
            break;
        }
    }
    switch(mode) {
        case 0: //テストコード
        {
            int audio_size = 110000;
            int *array = (int*)malloc(sizeof(int) * audio_size);
            data_reader("result_ch0.txt", array, audio_size);
//            SoundBlock *blocks = (SoundBlock*)malloc(sizeof(SoundBlock) * BLOCK_N);
//            if (blocks == NULL) {
//                fprintf(stderr, "%s() error! Could not allocate memory.(line: %d)\n", __func__, __LINE__);
//                exit(EXIT_FAILURE);
//            }
//            //        block_division_number[i] = auto_block_separation(stego_data.data[i], blocks, stego_data.sample_size, F);
//            int size = block_separation(array, blocks, audio_size, F, El, Eh);
            FILE *ofp = fopen("voice_div.txt", "w");
            int voice_switch_amp = 40000;
            for (int i = 0; i < 11; i++) {
                for (int j = 0; j < 10000; j++) {
                    fprintf(ofp, "%d\n", i % 2 == 0 ? -voice_switch_amp : voice_switch_amp);
                }
            }
            fclose(ofp);
//            free(blocks);
            free(array);
        }
            break;
        case 1:
            //透かし埋め込み試験
            embed_test();
            break;
        case 2:
            //改ざん検知試験
            defacing_detection_test();
            break;
        case 3:
            embed_test_hybrid();
            break;
        case 4:
            defacing_detection_test_hybrid(200);
            break;
        case 5:
            embed_test_hybrid_snr();
            break;
        case 6:
            defacing_detection_test_com(2000);
            break;
        case 7:
            multi_channels_embed_test();
            break;
        default:
            return 1;
    }
    
    return 0;
}
