/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#include <eekernel.h>
#include <libpad.h>

// FIXME: NTSC/PALで変更すべき値
#define REPEAT_GUARD  (30)
#define REAT_INTERVAL (4)

volatile u_long PadVSyncCounter; // VSYNCカウント用

// コントローラ用のワーク
typedef struct tagCONTROLLER {
	u_short key;      // キーの押し下げ状態(正論理)
	u_short oldkey;   // 1ループ前のキー情報(正論理)
	u_short down;     // キーの押し下げエッジ(正論理)
	u_short up;       // キーの押し上げエッジ(正論理)
	u_short rep;      // キーのリピート(正論理)

	u_long ulLastTime; // 最後にキーを評価した時間(VSYNCカウント時間)
	int repcount1[9];  // リピートカウンタ1
	int repcount2[9];  // リピートカウンタ2

	u_char pressure[16];  // 押し下げ圧力の強さ

	u_char RawBuf[32];    // コントローラからの受信バッファ
	u_char Id;            // コントローラモードID
	int    State;         // 認識フェイズ
} CONTROLLER;

static CONTROLLER g_Ctrl[2];

// リピートの系列を定義
static const u_short g_RepKeyTbl[9] = {
	SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright,  // 方向キー
	SCE_PADRup   ,   // [△]ボタン
	SCE_PADRdown ,   // [×]ボタン
	SCE_PADRleft ,   // [□]ボタン
	SCE_PADRright,   // [○]ボタン
	SCE_PADL1,       // [L1]ボタン
	SCE_PADL2,       // [L2]ボタン
	SCE_PADR1,       // [R1]ボタン
	SCE_PADR2        // [R2]ボタン
};


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	PadGetKey													ref: pad.h
//
//	ボタン情報の更新(ループ先頭で1回呼ぶ)
//
//	int cno : コントローラ番号
//
//	戻り値: u_long : コントローラ情報(解除情報/押し始め情報/押下情報/リピート情報)
u_long PadGetKey(int cno)
{
	// リピートを検出するキーのリスト
	int	i, t, w, state;
	CONTROLLER *pCtrl;

	pCtrl = &g_Ctrl[cno];
	t = PadVSyncCounter;
	w = (t - pCtrl->ulLastTime);
	pCtrl->ulLastTime = t;

	// パッドデータの取得
	pCtrl->oldkey = pCtrl->key;  // 前回のキー押し下げ情報
	pCtrl->key    = 0;           // 今回のキー情報

	// コントローラの状態チェック
	state = scePadGetState(cno, 0);
	pCtrl->State = state;           // 状態を保存
	if(state==scePadStateDiscon) {
		// コントローラが抜かれていたとき
		pCtrl->RawBuf[0] = 0x01;    // バッファ無効
		pCtrl->Id        = 0x00;    // IDは無効
	} else if(state==scePadStateStable || state==scePadStateFindCTP1) {
		// コントローラがコマンドを実行可能な状態
		u_char id;

		// コントローラからのデータを読み込み
		scePadRead(0, 0, pCtrl->RawBuf);
		if(pCtrl->RawBuf[0]!=0x00) {
			// コントローラとの通信に失敗していたとき
			;
		} else {
			// コントローラとの通信成功
			id = pCtrl->RawBuf[1];
			pCtrl->Id = id;

			// コントローラからの受信データ解釈
			switch(id) {
				case 0x41:  // 標準コントローラの場合
				case 0x73:  // DUALSHOCKのアナログモードの場合
					pCtrl->key = ~((pCtrl->RawBuf[2]<<8) | pCtrl->RawBuf[3]); // 現在のキー情報
					break;
			}
		}
	}

	// キーのサービス処理(読み取ったデータを加工)
	pCtrl->down = (pCtrl->oldkey ^ 0xFFFF) & pCtrl->key;    // 押し始めエッヂの情報を設定
	pCtrl->up   = (pCtrl->key    ^ 0xFFFF) & pCtrl->oldkey; // キーを離したエッヂの情報を設定

	// ボタンのリピート
	pCtrl->rep = pCtrl->down;
	for(i=0; i<9; i++) {
		if((pCtrl->oldkey & g_RepKeyTbl[i]) && (pCtrl->key & g_RepKeyTbl[i])) {
			// 前回と今回もキーが押し下げ状態だったとき
			if(pCtrl->repcount1[i]) {
				// ガードタイム期間中のとき
				pCtrl->repcount1[i] -= w;
				if(pCtrl->repcount1[i]<0) {
					pCtrl->repcount1[i] = 0;
				}
			}
			if(pCtrl->repcount1[i]==0) {
				// リピート中
				pCtrl->repcount2[i] -= w;
				if(pCtrl->repcount2[i]<=0) {
					pCtrl->rep |= pCtrl->key & g_RepKeyTbl[i];
					pCtrl->repcount2[i] += REAT_INTERVAL;	// リピートタイム
				}
			}
		} else {
			// 前回と今回でキー状態が変わっていたとき
			pCtrl->repcount1[i] = REPEAT_GUARD; // リピートまでのガードタイムを設定
			pCtrl->repcount2[i] = 0;            // 
		}
	}

	return (((u_long)pCtrl->up<<48) | ((u_long)pCtrl->down<<32) | ((u_long)pCtrl->key<<16) | ((u_long)pCtrl->rep));
}
