//
//  my_str.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/09/11.
//

#include "my_str.h"

char *ext_search(char *file_path) {
    int ext_bool = 0;
    while(*file_path != '\0') {
        if(*file_path == '.') {
            ext_bool = 1;
        }
        file_path++;
    }
    if(!ext_bool) {
        return NULL;
    }
    do{
        file_path--;
    }while(*file_path != '.');
    file_path++;
    return file_path;
}
int str_comparison(const char *str1, const char *str2) {
    while(*str1 != '\0' && *str2 != '\0') {
        if(*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
    return 1;
}
int str_count(const char *str) {
    int count = 0;
    while(*str != '\0') {
        str++;
        count++;
    }
    return count;
}
