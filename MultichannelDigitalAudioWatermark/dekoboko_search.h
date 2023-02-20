//
//  dekoboko_search.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/12/09.
//

#ifndef dekoboko_search_h
#define dekoboko_search_h

#include <stdio.h>
#include "data_tools.h"
#include "audio_watermark_functions.h"

#define ENCODING_BIT_RATE 65536
#define ZERO_AMP 32768

SoundData dekoboko_embed(SoundData original_data);
void dekoboko_tamper_detection(SoundData source_data, const char *result_file_path);

#endif /* dekoboko_search_h */
