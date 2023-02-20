//
//  my_str.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/09/11.
//

#ifndef my_str_h
#define my_str_h

#include <stdio.h>

/// <#Description#>
/// @param file_path <#file_path description#>
char *ext_search(char *file_path);

/// <#Description#>
/// @param str1 <#str1 description#>
/// @param str2 <#str2 description#>
int str_comparison(const char *str1, const char *str2);

/// <#Description#>
/// @param str <#str description#>
int str_count(const char *str);

#endif /* my_str_h */
