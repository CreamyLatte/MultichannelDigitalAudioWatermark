//
//  data_tools.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/10.
//

#ifndef data_tools_h
#define data_tools_h

#define FILE_PATH_SIZE 50 //ファイル長

/// SoundData構造体
typedef struct SoundData{
    /// 音声データ
    int **data;
    /// チャネル数
    int nchannels;
    /// サンプル数
    int sample_size;
    /// サンプリング周波数
    int sample_rate;
} SoundData;

/// 整数値データ読み込み
///
/// file_pathのファイルより整数値を任意のサイズ読み込む。
/// @param file_path 読み込み先のファイルパス
/// @param out_data 読み込んだ値を記録するint型配列
/// @param size 要素数
void data_reader(const char *file_path, int *out_data, int size);

/// 整数値データの出力
///
/// file_pathのファイルに整数値を任意のサイズ書き込む。
/// すでにfile_pathにファイルが存在している場合、上書き保存される。
/// @param file_path 出力先のファイルパス
/// @param in_data 出力する値が格納されたint型配列
/// @param size 要素数
void data_writer(const char *file_path, int *in_data, int size);

/// 音声データ読み込み
///
/// 音声データをテキストデータに変換したファイルを読み込み、SoundDataにし出力する。
/// @Attention この関数の返り値は必ずsoundData_free()でdataメモリを解放してください。
/// @param file_path 読み込むテキストファイルのファイルパス
/// @param nchannels 読み込むデータのチャネル数
/// @param sample_size 読み込むサイズ
/// @param sample_rate 読み込むデータのサンプリングレート
/// @return 引数から読み込んだSoundData型
SoundData soundData_reader(const char *file_path, int nchannels, int sample_size, int sample_rate);

/// SoundData変数の複製
///
/// 引数から受け取ったSoundData型変数を複製し、出力する。
/// 引数で受け取ったdataと出力される返り値のdataとは干渉しない。
/// @Attention この関数の返り値は必ずsoundData_free()でdataメモリを解放してください。
/// @param source 複製するSoundData変数
/// @return 複製されたSoundData型
SoundData soundData_copy(SoundData source);

/// SoundData.dataのメモリ解放
///
/// 引数で受け取ったSoundData型の変数のdataで保持しているメモリを解放する。
/// @param data メモリを解放するSoundData型変数
void soundData_free(SoundData data);

/// SoundDatawをテキストファイルに書き込み
///
/// SoundData型の値をテキストファイルに出力する。
/// @param file_name 書き込み先のファイルパス
/// @param data 書き込みをするデータ
void soundData_writer(const char *file_name, SoundData data);

/// int型配列を複製
///
/// 引数で受け取った配列を値をコピーし、新たな配列を生成する。
/// @Attention この関数の返り値は必ずfree()でメモリの解放をしてください。
/// @param source 複製する配列
/// @param size 複製する配列のサイズ
/// @return 複製された配列
int *arrayInt_copy(int *source, int size);

//Sound edit tools
/// 音声切り取り
///
/// SoundData型の音声データを指定区間切り取る。
/// 返り値に音声を切り取った新たなパラメータを返す。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// @param data 切り取る対象の音声データ
/// @param cut_start_index 切り取り開始位置
/// @param cut_frame 切り取るサンプル数
SoundData sound_cut(SoundData data, int cut_start_index, int cut_frame);

/// ランダム音声切り取り
///
/// SoundData型の音声データをランダムにcut_frame分だけ切り取る。
/// 返り値に音声を切り取った新たなパラメータを返す。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// 切り取り範囲はfpにストリーム出力される。
/// @param data 切り取る対象の音声データ
/// @param cut_frame 切り取り量
/// @param fp 切り取った範囲を出力するストリーム先
SoundData sound_cut_random(SoundData data, int cut_frame, FILE *fp);

/// 全てのチャネルへ雑音付加
///
/// SoundData型の音声データの全てのチャネルに対して、指定の範囲に雑音を付加する。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// @param data 雑音付加対象の音声データ
/// @param start_index 雑音付加開始位置
/// @param frame 雑音付加量
/// @param noise_amp 付加雑音の振幅レベル
void sound_noise_addition(SoundData data, int start_index, int frame, int noise_amp);

/// 全てのチャネルへランダムに雑音付加
///
/// SoundData型の音声データの全てのチャネルに対して、ランダムに雑音を付加する。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// 切り取り範囲はfpにストリーム出力される。
/// @param data 雑音付加対象の音声データ
/// @param frame 雑音付加量
/// @param noise_amp 付加雑音の振幅レベル
/// @param fp 雑音付加した範囲を出力するストリーム先
void sound_noise_addition_ramdom(SoundData data, int frame, int noise_amp, FILE *fp);

/// 指定チャネルへ雑音付加
///
/// SoundData型の音声データの指定したチャネルに対して、指定の範囲に雑音を付加する。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// @param data 雑音付加対象の音声データ
/// @param ch 雑音付加対象のチャネル
/// @param start_index 雑音付加開始位置
/// @param frame 雑音付加量
/// @param noise_amp 付加雑音の振幅レベル
void soundch_noise_addition(SoundData data, int ch, int start_index, int frame, int noise_amp);

/// 指定チャネルへランダムに雑音付加
///
/// SoundData型の音声データの指定したチャネルに対して、ランダムに雑音を付加する。
/// この関数では、引数で受け取ったdataを直接書き換える。
/// 切り取り範囲はfpにストリーム出力される。
/// @param data 雑音付加対象の音声データ
/// @param ch 雑音付加対象のチャネル
/// @param frame 雑音付加量
/// @param noise_amp 付加雑音の振幅レベル
/// @param fp  雑音付加した範囲を出力するストリーム先
void soundch_noise_addition_ramdom(SoundData data, int ch, int frame, int noise_amp, FILE *fp);

/// SN比
///
/// 第一引数と第二引数の差分を雑音とし、SN比の計算を行う。
/// @param first_data 元データ
/// @param second_data 変更されたデータ
double snr(SoundData first_data, SoundData second_data);

#endif /* data_tools_h */
