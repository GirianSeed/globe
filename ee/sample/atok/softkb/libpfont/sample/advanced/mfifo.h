/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __mfifo__
#define __mfifo__

#include <malloc.h>

// MFIFO 簡易ライブラリ
// MFIFOライブラリで使えるタグは cnt, ref, end のみ。
// end が来るとpause状態に。
// MFIFO では ref の次に必ず cnt か end を置くこと。
// そうでない場合 ref が正しく転送されない場合があるかも
// cycle stearing モードのほうが効率が良いかも


// bit : MFIFOの大きさ指定 ４より大きくなければならない
void* _mfifo_alloc(int bit);
void _mfifo_free(void* ptr);

// MFIFO 初期化
// ch : 出力DMAチャンネル (SCE_DMA_VIF1 | SCE_DMA_GIF)
// buffer : MFIFOバッファ(アラインメントに注意！)
// size : バッファの大きさ(2^n)
// return : 0=success 0>error
int _mfifo_init(int ch, void* buffer, size_t size);

// MFIFO 終了
void _mfifo_close(void);

// MFIFO スタート
// tte : タグ転送制御 vif1 なら通常１
// tie : 0=IRQ bit 無視 1=IRQ bit 有効
void _mfifo_start(int tte, int tie);

// MFIFO 停止
void _mfifo_stop(void);

// 一時停止
void _mfifo_pause(void);

// 一時停止状態からの再開
void _mfifo_continue(void);

// MFIFO 転送終了同期(空きができるまでポーリング)
// 注）ポーズ状態ではブロックしない。
// async : 0=転送終了を待つ 1=即時リターン
// return : 0=データ無い 1=転送中
int _mfifo_sync(int async);

// end tag による転送終了を検出(Semaphore)
int _mfifo_sync2(int async);

// MFIFO の空き容量を返す。(最大サイズ=((1 << (bit - 4)) - 1))
// return : 出力できるパケットの数 (qwc)
u_int _mfifo_getspace(void);

// MFIFO へキック
// spr_packet : スクラッチパッド上にあるパケットのアドレス
// qwc : 出力パケットサイズ
// mode : 0=mfifoに空きができて出力に成功するまでブロック。1:駄目ならすぐに抜ける。
// return : 0=成功 1=空きがない
int _mfifo_kick(u_long128 const* spr_packet, u_int qwc, int mode);

// 割り込みとセマフォ使ってMFIFO空きの同期待ちをする。
// デバッグでしか使っちゃいけないらしい T-T
// return : 0=空いた 1=データあるけど動いて無い 0>エラー
int _mfifo_sync_dbg(void);



extern __inline__ void* _mfifo_alloc(int bit){
	size_t size = 1 << bit;
	return memalign(size, size);
}

extern __inline__ void _mfifo_free(void* ptr){
	free(ptr);
}

#endif /* __mfifo__ */
