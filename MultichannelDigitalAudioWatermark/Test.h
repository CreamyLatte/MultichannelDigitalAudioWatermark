//
//  Test.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/10/18.
//

#ifndef Test_h
#define Test_h

#include <stdio.h>
#include "Header.h"
#include "data_tools.h"
#include "audio_watermark_functions.h"

/// 埋め込み試験テスト
int embed_test(void);
int embed_test_hybrid(void);
int embed_test_hybrid_snr(void);
int multi_channels_embed_test(void);

/// 改ざん検知試験テスト
int defacing_detection_test(void);
int defacing_detection_test_hybrid(int noise_amp);
int defacing_detection_test_com(int noise_amp);

#endif /* Test_h */
