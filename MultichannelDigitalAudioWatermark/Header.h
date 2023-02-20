//
//  Header.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/10.
//

#ifndef Header_h
#define Header_h

#include "data_tools.h"

SoundData embed_prime_steleo_watermark(SoundData original_data);
int tamper_detection_prime_steleo_watermark(SoundData source_data, FILE *resultp);

SoundData embed_stereo_hybrid_watermark(SoundData original_data);
int tamper_detection_stereo_hybrid_watermark(SoundData source_data, FILE *resultp);

SoundData embed_multichannel_communication_watermark(SoundData original_data);
int tamper_detection_communication_watermark(SoundData source_data, FILE *resultp);

#endif /* Header_h */
