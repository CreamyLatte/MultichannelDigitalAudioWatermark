//
//  view.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/12/01.
//

#ifndef view_h
#define view_h

#include <stdio.h>
#include "data_tools.h"
#include "audio_watermark_functions.h"


/// <#Description#>
/// @param sound_data <#sound_data description#>
/// @param frame <#frame description#>
/// @param file_name <#file_name description#>
/// @param eh_type <#eh_type description#>
void soundData_energie_view(SoundData sound_data, int frame, const char *file_name, int eh_type);

/// <#Description#>
/// @param sound_data <#sound_data description#>
/// @param frame <#frame description#>
/// @param file_name <#file_name description#>
void soundData_energie_histogram_view(SoundData sound_data, int frame, const char *file_name);

/// <#Description#>
/// @param data <#data description#>
/// @param size <#size description#>
/// @param nchannels <#nchannels description#>
/// @param file_name <#file_name description#>
void block_signal_view(MultiBlock data[], int size, int nchannels, char *file_name);

/// <#Description#>
/// @param data <#data description#>
/// @param size <#size description#>
/// @param nchannels <#nchannels description#>
/// @param view_range <#view_range description#>
/// @param file_name <#file_name description#>
void block_separation_view(MultiBlock data[], int size, int nchannels, int view_range, char *file_name);

/// <#Description#>
/// @param data <#data description#>
/// @param size <#size description#>
/// @param nchannels <#nchannels description#>
/// @param file_name <#file_name description#>
void block_histogram_view(MultiBlock data[], int size, int nchannels, char *file_name);

#endif /* view_h */
