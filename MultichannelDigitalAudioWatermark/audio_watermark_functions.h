//
//  audio_watermark_functions.h
//  MultichannelDigitalAudioWatermark
//
//  Created by Eisuke KASAHARA on 2021/06/10.
//

#ifndef audio_watermark_functions_h
#define audio_watermark_functions_h

#include <time.h>
#include <stdlib.h>
#include "data_tools.h"

#define F 441 //エネルギー算出フレーム数
#define BLOCK_N 1053 //ブロック数(メモリ確保用)
#define BLOCK_WATERMARK_NUMBER 2 //1ブロックあたりに埋め込む透かしの数

#define AMP_SIZE 32768

#define El 1024//エネルギーレベル(Low)
#define Eh 2048 //エネルギーレベル(High)

/// 1 chブロック
typedef struct SoundBlock {
    /// ブロック開始地点のポインタ
    int *start_mark;
    /// ブロックのサンプル数
    int size;
    /// 埋め込み予定のシーケンス番号
    int sequence_number;
} SoundBlock;

/// マルチチャネルブロック
typedef struct MultiBlock {
    /// 各チャネルのブロック開始地点のポインタ
    int **start_mark;
    // ブロックのサンプル数
    int size;
    /// 埋め込み予定のシーケンス番号
    int sequence_number;
} MultiBlock;

/// 埋め込みエリア
typedef struct EmbedArea {
    /// 埋め込み範囲開始地点のインデックス
    int start_index;
    /// 埋め込み範囲終了地点のインデックス
    int end_index;
    /// 埋め込み範囲のサイズ
    int size;
} EmbedArea;

/// エネルギーしきい値データ
typedef struct EnergyThresholdValue {
    int *signal_energie;
    int low;
    int high;
} EnergyData;

/// 改ざん箇所記録構造体
typedef struct {
    /// 改ざん検知開始地点
    int start_sample;
    /// 改ざん検知終了地点
    int end_sample;
} DefacingBlock;

/// 振幅エネルギー算出
///
/// start_dataからframeまでの振幅エネルギーを算出する。
/// @param start_data 振幅エネルギーを計算する配列
/// @param frame フレーム数
/// @return 音圧エネルギー
int wave_energie(int *start_data, int frame);

/// ランダム
///
/// 引数の範囲で乱数値を出力する。
/// @param min 最低値
/// @param max 最大値
/// @return 乱数値
int get_random(int min,int max);

/// 音圧レベル
///
/// ブロック分割に使用する音圧のレベルを出力する。
/// @param energie 音圧エネルギー
/// @param threshold_low しきい値low
/// @param threshold_high しきい値high
/// @return 音圧レベル(0~2level)
int level_selection(int energie, int threshold_low, int threshold_high);


/// EnergyDataのsignal_energyのメモリ解放
/// 
/// @param free_data メモリ解放対象データ
void energyData_free(EnergyData free_data);

/// モノラルブロック分割
///
/// モノラル音声のブロック分割を行う。
/// @param in_data ブロック分割対象の音声データ
/// @param out_sound_blocks 出力用SoundBlock配列 ブロック分割情報格納用
/// @param data_size in_dataのデータサイズ
/// @param frame エネルギー算出用フレーム数
/// @param energie_low しきい値low
/// @param energie_high しきい値high
/// @return ブロック分割数
int block_separation(int *in_data, SoundBlock *out_sound_blocks, int data_size, int frame, int energie_low, int energie_high);

/// モノラルブロック分割(エネルギー閾値自動設定)
///
/// モノラル音声のブロック分割を行う。
/// エネルギーしきい値は自動設定される。
/// @param in_data ブロック分割対象の音声データ
/// @param out_sound_blocks 出力用SoundBlock配列 ブロック分割情報格納用
/// @param data_size in_dataのデータサイズ
/// @param frame エネルギー算出用フレーム数
int auto_block_separation(int *in_data, SoundBlock *out_sound_blocks, int data_size, int frame);

/// マルチチャネルブロック分割
///
/// マルチチャネル音声のブロック分割を行う。
/// しきい値はin_dataより生成されるモノラル音声から自動で設定される。
/// @param in_data ブロック分割対象の音声データ
/// @param out_stereo_blocks 出力用MultiBlock配列 ブロック分割情報格納用
/// @param frame エネルギー算出用フレーム数
/// @return ブロック分割数
int multi_block_separation(SoundData in_data, MultiBlock *out_stereo_blocks, int frame);

/// モノラル変換
///
/// マルチチャネル音声データよりモノラル音声データを生成する。
/// @Attention この関数の返り値は必ずfree()でメモリを解放してください。
/// @param in_data マルチチャネル音声データ
/// @return モノラル音声配列
int *monaural_converter(SoundData in_data);

/// MultiBlock配列のメモリ解放
/// @param free_data メモリ解放対象のMultiBlock配列
/// @param size free_dataサイズ
void multiBlocks_free(MultiBlock *free_data, int size);

/// 素数チェック
///
/// 引数の値が素数であるかを判定する。
/// @param num 検索対象の値
int sosu_check(int num);

/// ブロックの合計値
///
/// SoundBlockの振幅値の合計を算出する。
/// @param data 算出対象ブロック
/// @return ブロック内の振幅値の合計
int block_sum(SoundBlock data);

/// ブロックの指定した符号の合計値
///
/// @param data 算出対象ブロック
/// @param sign 算出する符号
/// @return 指定した符号のブロック内の振幅値の合計値
int block_sign_sum(SoundBlock data, int sign);

/// int配列の合計値
///
/// int配列の合計値を計算する。
/// @param data 算出対象配列
/// @param size 配列のサイズ
int array_sum(int *data, int size);

/// int配列の指定した符号の合計値
///
/// @param data 算出対象配列
/// @param size 配列のサイズ
int array_sign_sum(int *data, int size, int sign);

/// 埋め込み範囲検索
///
/// ブロック内の埋め込み範囲を探索する。
/// ブロックの始点と終点からseach_flagにより、
/// 振幅値が+から-または、-から+に切り替わるポイントの探索を行う。
/// @param block 検索対象のブロック
/// @param search_flag サーチする種類を指定 true:+→-, false:-→+
/// @return 埋め込み範囲を記したEmbedArea
EmbedArea search_embed_area(SoundBlock block, int search_flag);

/// 取り出したい素数のブロックのインデックスを検索
///
/// 対となるチャネルのブロック列を受け取り、指定したサンプルがどのブロックになるかを探索する。
/// 返り値に探索したインデックスを返す。
/// @param blocks 検索したいチャネルのブロック列
/// @param search_sample_index サーチしたいサンプルのインデックス
/// @return 探索したインデックス
int blocks_search_refer_prime_index(SoundBlock *blocks, int search_sample_index);


int defacing_record(DefacingBlock *record, int defacing_count, int start_sample, int end_sample);


int max(int *data, int size);

#endif /* audio_watermark_functions_h */
