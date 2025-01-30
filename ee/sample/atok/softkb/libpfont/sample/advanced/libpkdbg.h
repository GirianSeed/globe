/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#ifndef __libpkdbg__
#define __libpkdbg__

#include <eetypes.h>

#if defined(__cplusplus)
extern "C"{
#endif

typedef enum{
	DBG_GSPATH_IDLE			= 0,
	DBG_GSPATH_VIF1_BUSY	= 0x01,
	DBG_GSPATH_VIF1_STALL	= 0x02,
	DBG_GSPATH_VU1_BUSY		= 0x04,
	DBG_GSPATH_VU1_STALL	= 0x08,
	DBG_GSPATH_GIF_BUSY		= 0x10,
	DBG_GSPATH_GIF_STALL	= 0x20,
}DBG_GSPATH_STATE;

typedef enum{
	DBG_VIF1_IDLE		= 0,
	DBG_VIF1_RUN		= 1,
	DBG_VIF1_WAIT_VU1	= 2,
	DBG_VIF1_WAIT_GIF	= 3,
	DBG_VIF1_FBREAK		= 4,
	DBG_VIF1_STOP		= 5,
	DBG_VIF1_ISTALL		= 6,
}DBG_VIF1_STATE;

typedef enum{
	DBG_GIF_IDLE		= 0,
	DBG_GIF_PATH1		= 1,
	DBG_GIF_PATH2		= 2,
	DBG_GIF_PATH3		= 3,
	DBG_GIF_PAUSE		= 4,
}DBG_GIF_STATE;

typedef enum{
	DBG_VU1_IDLE		= 0,
	DBG_VU1_RUN			= 1,
	DBG_VU1_FBREAK		= 3,
	DBG_VU1_TSTOP		= 4,
	DBG_VU1_DSTOP		= 5,
}DBG_VU1_STATE;

typedef enum{
	DBG_CMD_CONTINUE	= 0,
	DBG_CMD_CANCEL		= 1,
	DBG_CMD_EXIT		= 2,
	DBG_CMD_WAIT		= 3,
}DBG_CMD;

typedef enum{
	DBG_PKDUMP_NONE,
	DBG_PKDUMP_DMAL,
	DBG_PKDUMP_DMA,
	DBG_PKDUMP_VIFL,
	DBG_PKDUMP_VIF,
	DBG_PKDUMP_GIFL,
	DBG_PKDUMP_GIF,
	DBG_PKDUMP_ALL,
}DBG_PKDUMP;

typedef int (*DBG_PRINTF_FUNC)(char const* str, ...);

// 出力関数 デフォルトは"printf"
DBG_PRINTF_FUNC dbgSetPrintfFunc(DBG_PRINTF_FUNC func);

// 出力レベル設定
DBG_PKDUMP dbgSetPktDumpLevel(DBG_PKDUMP level);

// 同期
int dbgGsSyncPath(int mode, int timeout);

// パケット送信
void dbgGsSendPktVif1(u_long128 const* pkt);
void dbgGsSendPktGif(u_long128 const* pkt);
void dbgGsSendPktGifN(u_long128 const* pkt, u_int qwc);

// ステータス出力
void dbgDumpVif1State(void);
void dbgDumpGifState(void);
void dbgDumpVu1State(void);

// パケット解析の初期化
void dbgResetPacketState(void);

// パケット解析
// path 0=vif1, 1=gif
void dbgDumpPacketDma(u_long128 const* pkt, u_long128 const* limit, int path, DBG_PKDUMP level);
void dbgDumpPacketGif(u_long128 const* pkt, u_long128 const* limit, DBG_PKDUMP level);

// ステータス取得
DBG_VIF1_STATE dbgGetVif1State(void);
DBG_GIF_STATE dbgGetGifState(void);
DBG_VU1_STATE dbgGetVu1State(void);

// 書式化
size_t dbgFormatVif1Code(char* buf, size_t len, u_int code);
size_t dbgFormatVu1Micro(char* buf, size_t len, u_long code);
size_t dbgFormatDmaTag(char* buf, size_t len, u_long128 tag);
size_t dbgFormatGifTag(char* buf, size_t len, u_long tag);


#if defined(__cplusplus)
}
#endif


#endif // __libpkdbg__
