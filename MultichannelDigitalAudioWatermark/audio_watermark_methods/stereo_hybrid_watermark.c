//
//  stereo_hybrid.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2022/07/04.
//

#include <stdio.h>
#include <math.h>

#include "audio_watermark_functions.h"
#include "data_tools.h"

SoundData embed_stereo_hybrid_watermark(SoundData original_data) {
    /* コード省略 */
    
    return stego_data;
}

int tamper_detection_stereo_hybrid_watermark(SoundData source_data, FILE *resultp) {
    /* コード省略 */
    
    return result;
}
