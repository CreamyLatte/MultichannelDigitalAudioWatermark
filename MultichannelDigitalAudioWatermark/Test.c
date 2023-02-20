//
//  Test.c
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/10/18.
//

#include "Test.h"

#define SAMPLE_N 1

int embed_test(void) {
    int i, j; //loop
    
    char original_file_name[SAMPLE_N][256] = {
        "data/heikemonogatari_anago2021.txt"/*,
        "210811_1152.txt",
        "210811_1154.txt"*/
    };
    SoundData original_data[SAMPLE_N];
    int sample_size_list[3] = {834207/*, 5249024, 4655104*/};
    for(i = 0; i < SAMPLE_N; i++) {
        original_data[i] = soundData_reader(original_file_name[i], 2, sample_size_list[i], 44100);
    }
    //埋め込み試験
    int try_times = 100;
    printf("試行回数: 100\n"); //scanf("%d", &try_times);
    int new_count[SAMPLE_N];
    for(i = 0; i < SAMPLE_N; i++) {
        new_count[i] = 0;
    }
    for(i = 0; i < try_times; i++) {
        printf("time: %d\n", i+1);
        for(j = 0; j < SAMPLE_N; j++) {
            printf("\n[%s]\n", original_file_name[j]);
            SoundData stego_data_new;
            
            printf("embed_prime_steleo_watermark()\n");
            stego_data_new = embed_multichannel_communication_watermark(original_data[j]);
            
            if (1 == tamper_detection_communication_watermark(stego_data_new, NULL)) {
                fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
                new_count[j]++;
            }
            
            soundData_free(stego_data_new);
            printf("\n");
        }
    }
    for(i = 0; i < SAMPLE_N; i++) {
        printf("[%s]\n", original_file_name[i]);
        printf("miss_count: %3d(success: %6.2f%%)", new_count[i], ((1-(double)new_count[i]/try_times)*100));
    }
    for(i = 0; i < SAMPLE_N; i++) {
        soundData_free(original_data[i]);
    }
    
    return 0;
}
int embed_test_hybrid(void) {
    int i, j; //loop
    
    char original_file_name[SAMPLE_N][256] = {
        "data/heikemonogatari_anago2021.txt"//,
//        "data/210811_1152.txt",
//        "data/210811_1154.txt"
    };
    SoundData original_data[SAMPLE_N];
    int sample_size_list[3] = {834207/*, 5249024, 4655104*/};
    for(i = 0; i < SAMPLE_N; i++) {
        original_data[i] = soundData_reader(original_file_name[i], 2, sample_size_list[i], 44100);
    }
    //埋め込み試験
    int try_times;
    fprintf(stderr, "試行回数: "); scanf("%d", &try_times);
    int new_count[SAMPLE_N];
    for(i = 0; i < SAMPLE_N; i++) {
        new_count[i] = 0;
    }
    for(i = 0; i < try_times; i++) {
        fprintf(stderr, "\rtime: %d/%d", i+1, try_times);
        for(j = 0; j < SAMPLE_N; j++) {
            printf("\n[%s]\n", original_file_name[j]);
            SoundData stego_data_new;
            
            printf("embed_prime_steleo_watermark()\n");
            stego_data_new = embed_stereo_hybrid_watermark(original_data[j]);
            
            if (1 == tamper_detection_stereo_hybrid_watermark(stego_data_new, NULL)) {
                fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
                new_count[j]++;
            }
            
            soundData_free(stego_data_new);
            printf("\n");
        }
    }
    for(i = 0; i < SAMPLE_N; i++) {
        fprintf(stderr, "\n[%s]\n", original_file_name[i]);
        fprintf(stderr, "miss_count: %3d(success: %6.2f%%)", new_count[i], ((1-(double)new_count[i]/try_times)*100));
    }
    for(i = 0; i < SAMPLE_N; i++) {
        soundData_free(original_data[i]);
    }
    
    return 0;
}
int embed_test_hybrid_snr(void) {
    char *original_file_name = "data/heikemonogatari_anago2021.txt";
    int sample_size = 834207;
    SoundData original_data = soundData_reader(original_file_name, 2, sample_size, 44100);
    SoundData stego_data = embed_multichannel_communication_watermark(original_data);
    if (1 == tamper_detection_communication_watermark(stego_data, NULL)) {
        fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
        return 1;
    }
    double result = snr(original_data, stego_data);
    
    soundData_free(original_data);
    soundData_free(stego_data);
    
    printf("SNR[dB] = %.2f\n", result);
    return 0;
}


#define TEST2_N 8
int defacing_detection_test(void) {
    int i, j; //loop
    
    //original_data読み込み
    SoundData original_data = soundData_reader("data/heikemonogatari_anago2021.txt", 2, 834207, 44100);
    
    //stego_data生成
    SoundData stego_data;
    stego_data = embed_prime_steleo_watermark(original_data);
    if (1 == tamper_detection_prime_steleo_watermark(stego_data, NULL)) {
        fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
        exit(EXIT_FAILURE);
    }
    soundData_writer("data/StegoData", stego_data);
    
    //改ざん検知テスト
    //改ざん場所記録用
    const char defacing_fp_path[TEST2_N][256] = {
        "defacing_sample_noise_44100.txt",
        "defacing_sample_noise_441.txt",
        "defacing_sample_noise_0ch_44100.txt",
        "defacing_sample_noise_0ch_441.txt",
        "defacing_sample_noise_1ch_44100.txt",
        "defacing_sample_noise_1ch_441.txt",
        "defacing_sample_cut_44100.txt",
        "defacing_sample_cut_441.txt"
    };
    FILE *defacing_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        defacing_fp[i] = fopen(defacing_fp_path[i], "w");
        if(defacing_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //改ざん検知場所記録用
    const char result_fp_path[TEST2_N][256] = {
        "dp_noise_44100.txt",
        "dp_noise_441.txt",
        "dp_noise_0ch_44100.txt",
        "dp_noise_0ch_441.txt",
        "dp_noise_1ch_44100.txt",
        "dp_noise_1ch_441.txt",
        "dp_cut_44100.txt",
        "dp_cut_441.txt"
    };
    FILE *result_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        result_fp[i] = fopen(result_fp_path[i], "w");
        if(result_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //テスト実行
    for(j = 0; j < 10; j++) {
        printf("*-------test %2d-------*", j+1);
        SoundData sample = soundData_copy(stego_data);
        
        printf("\nノイズ付加テスト sample:44100\n");
        sound_noise_addition_ramdom(sample, 44100, 5000, defacing_fp[0]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[0])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_44100", sample);
        }
        soundData_free(sample);


        sample = soundData_copy(stego_data);
        printf("\nノイズ付加テスト sample:441\n");
        sound_noise_addition_ramdom(sample, 441, 5000, defacing_fp[1]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[1])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 0, 44100, 5000, defacing_fp[2]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[2])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 0, 441, 5000, defacing_fp[3]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[3])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 1, 44100, 5000, defacing_fp[4]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[4])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 1, 441, 5000, defacing_fp[5]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[5])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_441", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:44100\n");
        sample = sound_cut_random(sample, 44100, defacing_fp[6]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[6])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_44100", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:441\n");
        sample = sound_cut_random(sample, 441, defacing_fp[7]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_prime_steleo_watermark(sample, result_fp[7])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_441", sample);
        }
        soundData_free(sample);
    }
    
    //fflush・fclose実行
    for(i = 0; i < TEST2_N; i++) {
        fflush(defacing_fp[i]);
        fclose(defacing_fp[i]);
    }
    for(i = 0; i < TEST2_N; i++) {
        fflush(result_fp[i]);
        fclose(result_fp[i]);
    }
    
    soundData_free(original_data);
    soundData_free(stego_data);
    
    return 0;
}

int defacing_detection_test_hybrid(int noise_amp) {
    int i, j; //loop
    
    //original_data読み込み
    SoundData original_data = soundData_reader("data/heikemonogatari_anago2021.txt", 2, 834207, 44100);
    
    //stego_data生成
    SoundData stego_data;
    stego_data = embed_stereo_hybrid_watermark(original_data);
    if (1 == tamper_detection_stereo_hybrid_watermark(stego_data, NULL)) {
        fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
        exit(EXIT_FAILURE);
    }
    soundData_writer("data/StegoData", stego_data);
    
    //改ざん検知テスト
    //改ざん場所記録用
    const char defacing_fp_path[TEST2_N][256] = {
        "defacing/sample_noise_44100.txt",
        "defacing/sample_noise_441.txt",
        "defacing/sample_noise_0ch_44100.txt",
        "defacing/sample_noise_0ch_441.txt",
        "defacing/sample_noise_1ch_44100.txt",
        "defacing/sample_noise_1ch_441.txt",
        "defacing/sample_cut_44100.txt",
        "defacing/sample_cut_441.txt"
    };
    FILE *defacing_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        defacing_fp[i] = fopen(defacing_fp_path[i], "w");
        if(defacing_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //改ざん検知場所記録用
    const char result_fp_path[TEST2_N][256] = {
        "dp/dp_noise_44100.txt",
        "dp/dp_noise_441.txt",
        "dp/dp_noise_0ch_44100.txt",
        "dp/dp_noise_0ch_441.txt",
        "dp/dp_noise_1ch_44100.txt",
        "dp/dp_noise_1ch_441.txt",
        "dp/dp_cut_44100.txt",
        "dp/dp_cut_441.txt"
    };
    FILE *result_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        result_fp[i] = fopen(result_fp_path[i], "w");
        if(result_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //テスト実行
    for(j = 0; j < 10; j++) {
        printf("*-------test %2d-------*", j+1);
        SoundData sample = soundData_copy(stego_data);
        
        printf("\nノイズ付加テスト sample:44100\n");
        sound_noise_addition_ramdom(sample, 44100, noise_amp, defacing_fp[0]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[0])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_44100", sample);
        }
        soundData_free(sample);


        sample = soundData_copy(stego_data);
        printf("\nノイズ付加テスト sample:441\n");
        sound_noise_addition_ramdom(sample, 441, noise_amp, defacing_fp[1]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[1])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 0, 44100, noise_amp, defacing_fp[2]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[2])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 0, 441, noise_amp, defacing_fp[3]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[3])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 1, 44100, noise_amp, defacing_fp[4]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[4])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 1, 441, noise_amp, defacing_fp[5]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[5])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_441", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:44100\n");
        sample = sound_cut_random(sample, 44100, defacing_fp[6]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[6])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_44100", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:441\n");
        sample = sound_cut_random(sample, 441, defacing_fp[7]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_stereo_hybrid_watermark(sample, result_fp[7])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_441", sample);
        }
        soundData_free(sample);
    }
    
    //fflush・fclose実行
    for(i = 0; i < TEST2_N; i++) {
        fflush(defacing_fp[i]);
        fclose(defacing_fp[i]);
    }
    for(i = 0; i < TEST2_N; i++) {
        fflush(result_fp[i]);
        fclose(result_fp[i]);
    }
    
    soundData_free(original_data);
    soundData_free(stego_data);
    
    return 0;
}
int defacing_detection_test_com(int noise_amp) {
    int i, j; //loop
    
    //original_data読み込み
    SoundData original_data = soundData_reader("data/heikemonogatari_anago2021.txt", 2, 834207, 44100);
    
    //stego_data生成
    SoundData stego_data;
    stego_data = embed_multichannel_communication_watermark(original_data);
    if (1 == tamper_detection_communication_watermark(stego_data, NULL)) {
        fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
        exit(EXIT_FAILURE);
    }
    soundData_writer("data/StegoData", stego_data);
    
    //改ざん検知テスト
    //改ざん場所記録用
    const char defacing_fp_path[TEST2_N][256] = {
        "defacing/sample_noise_44100.txt",
        "defacing/sample_noise_441.txt",
        "defacing/sample_noise_0ch_44100.txt",
        "defacing/sample_noise_0ch_441.txt",
        "defacing/sample_noise_1ch_44100.txt",
        "defacing/sample_noise_1ch_441.txt",
        "defacing/sample_cut_44100.txt",
        "defacing/sample_cut_441.txt"
    };
    FILE *defacing_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        defacing_fp[i] = fopen(defacing_fp_path[i], "w");
        if(defacing_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //改ざん検知場所記録用
    const char result_fp_path[TEST2_N][256] = {
        "dp/dp_noise_44100.txt",
        "dp/dp_noise_441.txt",
        "dp/dp_noise_0ch_44100.txt",
        "dp/dp_noise_0ch_441.txt",
        "dp/dp_noise_1ch_44100.txt",
        "dp/dp_noise_1ch_441.txt",
        "dp/dp_cut_44100.txt",
        "dp/dp_cut_441.txt"
    };
    FILE *result_fp[TEST2_N];
    for(i = 0; i < TEST2_N; i++) {
        result_fp[i] = fopen(result_fp_path[i], "w");
        if(result_fp[i] == NULL) {
            fprintf(stderr, "fopen error! line: %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    //テスト実行
    for(j = 0; j < 10; j++) {
        printf("*-------test %2d-------*", j+1);
        SoundData sample = soundData_copy(stego_data);
        
        printf("\nノイズ付加テスト sample:44100\n");
        sound_noise_addition_ramdom(sample, 44100, noise_amp, defacing_fp[0]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[0])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_44100", sample);
        }
        soundData_free(sample);


        sample = soundData_copy(stego_data);
        printf("\nノイズ付加テスト sample:441\n");
        sound_noise_addition_ramdom(sample, 441, noise_amp, defacing_fp[1]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[1])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 0, 44100, noise_amp, defacing_fp[2]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[2])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nlノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 0, 441, noise_amp, defacing_fp[3]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[3])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_0ch_441", sample);
        }
        soundData_free(sample);
        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:44100\n");
        soundch_noise_addition_ramdom(sample, 1, 44100, noise_amp, defacing_fp[4]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[4])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_44100", sample);
        }
        soundData_free(sample);

        
        sample = soundData_copy(stego_data);
        printf("\nrノイズ付加テスト sample:441\n");
        soundch_noise_addition_ramdom(sample, 1, 441, noise_amp, defacing_fp[5]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[5])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Noise_1ch_441", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:44100\n");
        sample = sound_cut_random(sample, 44100, defacing_fp[6]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[6])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_44100", sample);
        }
        soundData_free(sample);
        
        
        sample = soundData_copy(stego_data);
        printf("\nデータ切り取りテスト sample:441\n");
        sample = sound_cut_random(sample, 441, defacing_fp[7]);
        printf("\n改ざん検知試験\n");
        if(1 == tamper_detection_communication_watermark(sample, result_fp[7])) {
            fprintf(stderr, "改ざんを検知しました。\n");
        }
        if(j == 0) {
            soundData_writer("data/Cut_441", sample);
        }
        soundData_free(sample);
    }
    
    //fflush・fclose実行
    for(i = 0; i < TEST2_N; i++) {
        fflush(defacing_fp[i]);
        fclose(defacing_fp[i]);
    }
    for(i = 0; i < TEST2_N; i++) {
        fflush(result_fp[i]);
        fclose(result_fp[i]);
    }
    
    soundData_free(original_data);
    soundData_free(stego_data);
    
    return 0;
}

int multi_channels_embed_test(void) { 
    int i, j; //loop
    
    const int sample_n = 3;
    char original_file_name[sample_n][256] = {
        "data/AudioText/AudioText_ch1_2B4.txt",
        "data/AudioText/AudioText_ch2_2B4.txt",
        "data/AudioText/AudioText_ch3_2B4.txt"
    };
    SoundData original_data[sample_n];
    int sample_size_list[sample_n] = {1597632, 1597632, 1597632};
    for(i = 0; i < sample_n; i++) {
        original_data[i] = soundData_reader(original_file_name[i], i+1, sample_size_list[i], 44100);
    }
    //埋め込み試験
    int try_times = 1;
    printf("試行回数: %d\n", try_times); //scanf("%d", &try_times);
    int new_count[sample_n];
    double snr_list[sample_n];
    for(i = 0; i < sample_n; i++) {
        new_count[i] = 0;
    }
    for(i = 0; i < try_times; i++) {
        printf("time: %d\n", i+1);
        for(j = 0; j < sample_n; j++) {
            printf("\n[%s]\n", original_file_name[j]);
            SoundData stego_data_new;
            
            printf("embed_prime_steleo_watermark()\n");
            stego_data_new = embed_multichannel_communication_watermark(original_data[j]);
            
            if (1 == tamper_detection_communication_watermark(stego_data_new, NULL)) {
                fprintf(stderr, "正しく埋め込みが行えませんでした。\n");
                new_count[j]++;
            }
            
            // Calc snr.
            snr_list[j] = snr(original_data[j], stego_data_new);
            
            // Write stego data.
            char stego_file_name[512];
            sprintf(stego_file_name, "%s.stego.txt", original_file_name[j]);
            soundData_writer(stego_file_name, stego_data_new);
            
            soundData_free(stego_data_new);
            printf("\n");
        }
    }
    for(i = 0; i < sample_n; i++) {
        printf("[%s]\n", original_file_name[i]);
        printf("miss_count: %3d(success: %6.2f%%), snr: %f\n", new_count[i], ((1-(double)new_count[i]/try_times)*100), snr_list[i]);
    }
    for(i = 0; i < sample_n; i++) {
        soundData_free(original_data[i]);
    }
    
    return 0;
}
