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

// USBキーボードライブラリをハンドリングする状態マシン

#include <eekernel.h>
#include <libusbkb.h>
#include <stdio.h>
#include <string.h>
#include "kbd.h"

// フェイズ管理のための状態を定義
enum {
	KB_GETINFO,
	KB_GETINFO_SYNC,
	KB_NEW_LEDSTAT_SYNC,
	KB_NEW_LEDMODE_SYNC,
	KB_READ,
	KB_READSYNC
};

// グローバル変数
static USBKBINFO_t g_kbInfo;				// USBキーボードの接続情報
static USBKBDATA_t g_kbData;				// USBキーボードから得た情報
static int g_kbStat;						// default not connected
static int g_kbPhase;						// フェイズ管理
static int g_kbLED;							// LED管理

static int g_activeKb;						// アクティブなキーボード番号
                                            // 複数つながることに対応 二つまで

static int g_ignore_count;					// キーボードライブラリに溜まった
                                            // メッセージをから読みして捨てるため

static int g_changeKb;						// キーボードが2個さされていた時に
											// アクティブでないキーボードから打たれた文字を
											// アクティブになった時に送信しない為のフラグ

static struct{
	int region;
	int kbType;
	int repeatWait;
	int repeatSpeed;
}KbdCnf;


// キーボードライブラリ初期化
int Kbd_Init( int kbType, int repeatWait, int repeatSpeed, int region )
{
	int dmy;
	int res;

	KbdCnf.region      = region;
	KbdCnf.kbType      = kbType;
	KbdCnf.repeatWait  = repeatWait;
	KbdCnf.repeatSpeed = repeatSpeed;

	g_kbStat  = 0;
	g_kbPhase = KB_GETINFO;
	g_kbLED = g_activeKb =0;
	g_kbLED    |= USBKB_LED_NUM_LOCK;
    g_activeKb |= USBKB_LED_NUM_LOCK;
    g_ignore_count = 0;
	g_changeKb		= 0;

	// USBキーボードライブラリの初期化
	res = sceUsbKbInit(&dmy);
	if(res==USBKB_NG) {
		// USBキーボードライブラリの初期化に失敗
		printf("Initialize error\n");
	}
	// LED初期設定
	sceUsbKbSetLEDStatus(g_activeKb, g_kbLED);

	return(0);
}

// キーボードライブラリ終了
void Kbd_Exit(void)
{
	sceUsbKbEnd();
}


// キーボードの接続監視およびデータの引き取りまでを行うステートマシン関数
// 戻り値
//		0:  新規データなし
//		1:  新規データあり
//		-1: エラー
int Kbd_Main(USBKBDATA_t *pKbdData)
{
	int ret, result;
	int i;

	switch(g_kbPhase) {
		case KB_GETINFO:

			// 非同期でキーボード接続情報を取得
			ret = sceUsbKbGetInfo(&g_kbInfo);
			if(ret!=USBKB_OK) {
				printf("Error%d : sceUsbKbGetInfo\n", ret);
				return(-1);
			}
			g_kbPhase = KB_GETINFO_SYNC;
			break;

		case KB_GETINFO_SYNC:
			{
				int new_kb_detected = 0;
				
				// キーボード接続情報の取得が完了したかどうかポーリング
				ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
				//	printf("USBKB get info%d\n", ret);
				if(ret==USBKB_EXEC) {
					// 非同期関数がまだ実行中
					break;
				}
				if(result!=USBKB_OK) {
					// USBキーボードライブラリからエラーが返された
					printf("Error%d : sceUsbKbSync\n", result);
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
			
				if( (!g_kbInfo.status[0]) && (!g_kbInfo.status[1]) )
				{
					// キーボード見つからず
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
			
				// アクティブキーボードを更新
				{
					if( g_kbInfo.status[g_activeKb] )
					{
						// そのまま
						new_kb_detected = 1;			// 一回抜いてまたさされた時
					}
					else if( g_kbInfo.status[1-g_activeKb] )
					{
						new_kb_detected = 1;
				
						// g_activeKb 切り替え
						g_activeKb = 1-g_activeKb;
				
						printf( "g_activeKb = %d\n", g_activeKb );
					}
					else
					{
						// ありえない
					}
				}
				
				// キーボードが新たに差された
				if( new_kb_detected )
				{
					g_changeKb = 1;
					Kbd_SetConfig();
				
					// LEDの点灯モードの設定
					//	printf("USBKB LED mode\n");
				
					ret = sceUsbKbSetLEDMode(g_activeKb, USBKB_LED_MODE_MANUAL);	// LED制御はマニュアルで
					if(ret!=USBKB_OK) {
						// USBキーボードライブラリからエラーが返された
						printf("Error%d : sceUsbKbSetLEDMode\n", ret);
						g_kbPhase = KB_GETINFO;
						return(-1);
					}
					g_kbPhase = KB_NEW_LEDMODE_SYNC;
					break;
				}
				g_kbPhase = KB_READ;
			}
			break;

		case KB_NEW_LEDMODE_SYNC:

			// LED点灯モードの設定が完了したかどうかポーリング
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// 非同期関数がまだ実行中
				break;
			}
			if(result!=USBKB_OK) {
				// USBキーボードライブラリからエラーが返された
				printf("Error%d : sceUsbKbSync\n", result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}

			g_kbStat  = g_kbInfo.status[g_activeKb];

			// 非同期でLEDステータスを設定
			g_kbLED |= USBKB_LED_NUM_LOCK;
			ret = sceUsbKbSetLEDStatus(g_activeKb, USBKB_LED_NUM_LOCK);
			if(ret!=USBKB_OK) {
				// USBキーボードライブラリからエラーが返された
				printf("Error%d : sceUsbKbSetLEDStatus\n", ret);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}
			g_kbPhase = KB_NEW_LEDSTAT_SYNC;
			break;

		case KB_NEW_LEDSTAT_SYNC:

			// LEDステータスが完了したかどうかポーリング
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// 非同期関数がまだ実行中
				break;
			}
			if(result!=USBKB_OK) {
				// USBキーボードライブラリからエラーが返された
				printf("Error%d : sceUsbKbSync\n", result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}
			g_kbPhase = KB_READ;
			break;

		case KB_READ:

			if(g_changeKb)
			{
				// キーボード
				KbdIgnoreSeveralMessage();
				g_changeKb = 0;
			}
			else
			{
				// キーボードデータのリード
				ret = sceUsbKbRead(g_activeKb, &g_kbData);
				if(ret!=USBKB_OK) {
					// USBキーボードライブラリからエラーが返された
					printf("Error%d : sceUsbKbRead\n", ret);
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
				g_kbPhase = KB_READSYNC;
			}
			break;

		case KB_READSYNC:

			// キーボードデータのリードが完了したかどうかポーリング
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// 非同期関数がまだ実行中
				break;
			}
			if(result!=USBKB_OK) {
				// USBキーボードライブラリからエラーが返された
				printf("Error%d : sceUsbKbSync\n",result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}

			g_kbPhase = KB_READ;
			if(pKbdData) {
				g_kbData.led = g_kbLED;
				memcpy(pKbdData, &g_kbData, sizeof(USBKBDATA_t));
			}

			// たまっていたメッセージを無視する
			if( g_ignore_count > 0 )
			{
				g_ignore_count --;
				if( g_ignore_count < 0 ){ g_ignore_count = 0; }
				return 0;
			}

			// キーコードをスキャンしてLED制御
			{
				u_char led;

				led = g_kbLED;
				for(i=0; i<g_kbData.len; i++) {
					if(g_kbData.keycode[i]==USBKEYC_SCROLL_LOCK) {
						// [ScrollLock]キーが押されたとき
						led ^= USBKB_LED_SCROLL_LOCK;
					} else if(g_kbData.keycode[i]==USBKEYC_NUM_LOCK) {
						// [NumLock]キーが押されたとき
						led ^= USBKB_LED_NUM_LOCK;
					} else if(g_kbData.keycode[i]==USBKEYC_CAPS_LOCK) {
						if( KbdCnf.region == 0 )// 日本向け
						{
							// [CapsLock]キーが押されたとき
							if((g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
							  !(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN))) {
								// [CTRL]キーがプレス、[SHIFT],[ALT],[Windows]キーがリリースされていたとき
	
								// カナロック状態を反転させる
								led ^= USBKB_LED_KANA;
							} else if(//(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) &&	// SHIFTキーが無くてもOK?
									  !(g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN))) {
	
								// [SHIFT]キーがプレス、[CTRL],[ALT],[Windows]キーがリリースされていたとき
								// CapsLock状態を反転させる
								led ^= USBKB_LED_CAPS_LOCK;
							}
						}
						else
						{
							if(//(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) &&	// SHIFTキーが無くてもOK?
									  !(g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT))) {
								// [SHIFT]キーがプレス、[CTRL],[ALT],[Windows]キーがリリースされていたとき
								// CapsLock状態を反転させる
								led ^= USBKB_LED_CAPS_LOCK;
							}
						}
					}
				}
				if(led!=g_kbLED) {
					g_kbLED = led;
					ret = sceUsbKbSetLEDStatus(g_activeKb, led);
					if(ret!=USBKB_OK) {
						// USBキーボードライブラリからエラーが返された
						printf("Error%d : sceUsbKbSetLEDStatus\n", ret);
						g_kbPhase = KB_GETINFO;
						return(-1);
					}
					g_kbPhase = KB_NEW_LEDSTAT_SYNC;
				}
			}
			return(1);		// g_kbData有効
	}
	return(0);			// sync 中
}

// NumLock状態か？
int KbdGetNumLockState(void)
{
	if(g_kbData.led & USBKB_LED_NUM_LOCK)//g_kbLED & USBKB_LED_NUM_LOCK)
	{
		return 1;		// NumLock/ON
	}

	return 0;			// NumLock/OFF
}



void KbdIgnoreSeveralMessage(void)
{
	// 10 回の sceUsbKbRead を 無視する
	// UsbKbのキューにたまったメッセージを無視するため
	g_ignore_count = 10;
}


// KbdMain を長い間呼ばなくなっても良いタイミングかどうかをチェック
int KbdCheckCanPause(void)
{
	if(
		g_kbPhase == KB_GETINFO ||
		g_kbPhase == KB_READ
	)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void Kbd_SetConfig( int kbType, int repeatWait, int repeatSpeed )
{
	int repeatStart = KEYREPW_MIDDLE;
	int repeatInterval = KEYREPS_MIDDLE;

	switch( KbdCnf.repeatWait )
	{
	case ENV_KEYREPW_SHORT:
		repeatStart = KEYREPW_SHORT;
		printf("---%d---\n", __LINE__);
		
		break;
	case ENV_KEYREPW_MIDDLE:
		repeatStart = KEYREPW_MIDDLE;
		printf("---%d---\n", __LINE__);
		break;
	case ENV_KEYREPW_LONG:
		repeatStart = KEYREPW_LONG;
		printf("---%d---\n", __LINE__);
		break;
	}

	switch(  KbdCnf.repeatSpeed )
	{
	case ENV_KEYREPS_FAST:
		repeatInterval = KEYREPS_FAST;
		printf("---%d---\n", __LINE__);
		break;
	case ENV_KEYREPS_MIDDLE:
		repeatInterval = KEYREPS_MIDDLE;
		printf("---%d---\n", __LINE__);
		break;
	case ENV_KEYREPS_SLOW:
		repeatInterval = KEYREPS_SLOW;
		printf("---%d---\n", __LINE__);
		break;
	}

	sceUsbKbSetRepeat(g_activeKb, repeatStart, repeatInterval);
	sceUsbKbSetCodeType(g_activeKb, USBKB_CODETYPE_RAW);		// ASCIIコードに変換しない
	sceUsbKbSetArrangement(g_activeKb, KbdCnf.kbType );	// 配列設定
}

