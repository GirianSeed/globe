/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eetypes.h>
#include <eekernel.h>
#include <eeregs.h>
#include <eestruct.h>

#include <libvu0.h>

#include <stdio.h>
#include <string.h>

#include <libdev.h>

#include "libpkdbg.h"

#define UNUSED_ALWAYS(a)	(void)(a)

#define SCE_VIF_NOP			(0x00U)
#define SCE_VIF_STCYCL		(0x01U)
#define SCE_VIF_OFFSET		(0x02U)
#define SCE_VIF_BASE		(0x03U)
#define SCE_VIF_ITOP		(0x04U)
#define SCE_VIF_STMOD		(0x05U)
#define SCE_VIF_MSKPATH3	(0x06U)
#define SCE_VIF_MARK		(0x07U)
#define SCE_VIF_FLUSHE		(0x10U)
#define SCE_VIF_FLUSH		(0x11U)
#define SCE_VIF_FLUSHA		(0x13U)
#define SCE_VIF_MSCAL		(0x14U)
#define SCE_VIF_MSCALF		(0x15U)
#define SCE_VIF_MSCNT		(0x17U)
#define SCE_VIF_STMASK		(0x20U)
#define SCE_VIF_STROW		(0x30U)
#define SCE_VIF_STCOL		(0x31U)
#define SCE_VIF_MPG			(0x4aU)
#define SCE_VIF_DIRECT		(0x50U)
#define SCE_VIF_DIRECTHL	(0x51U)
#define SCE_VIF_UNPACK		(0x60U)

#define SCE_VIF_UPK_S_32	(0x0U)
#define SCE_VIF_UPK_S_16	(0x1U)
#define SCE_VIF_UPK_S_8		(0x2U)
#define SCE_VIF_UPK_V2_32	(0x4U)
#define SCE_VIF_UPK_V2_16	(0x5U)
#define SCE_VIF_UPK_V2_8	(0x6U)
#define SCE_VIF_UPK_V3_32	(0x8U)
#define SCE_VIF_UPK_V3_16	(0x9U)
#define SCE_VIF_UPK_V3_8	(0xaU)
#define SCE_VIF_UPK_V4_32	(0xcU)
#define SCE_VIF_UPK_V4_16	(0xdU)
#define SCE_VIF_UPK_V4_8	(0xeU)
#define SCE_VIF_UPK_V4_5	(0xfU)

#define __VU1_MICRO__	((u_long*)0x11008000U)
#define __VU1_DATA__	((u_long128*)0x1100c000U)


typedef union _sceVifCode{
	u_int d;
	struct{
		u_int imm:16;
		u_int num:8;
		u_int cmd:7;
		u_int itr:1;
	}i;
}sceVifCode;

typedef union _sceVuMicroCode{
	u_long v;
	struct{
		u_int :32;
		u_int :25;
		u_int :1;
		u_int :1;
		u_int t:1;
		u_int d:1;
		u_int m:1;
		u_int e:1;
		u_int i:1;
	}mode;

	struct{
		union{
			u_int v;
			float f;

			struct{
				u_int bc:2;
				u_int OPCODE:4;
				u_int fd:5;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int LOP:7;
			}type0;

			struct{
				u_int OPCODE:6;
				u_int fd:5;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int LOP:7;
			}type1;

			struct{
				u_int bc:2;
				u_int OPCODE:9;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int LOP:7;
			}type2;

			struct{
				u_int OPCODE:11;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int LOP:7;
			}type3;

			struct{
				u_int OPCODE:11;
				u_int fs:5;
				u_int ft:5;
				u_int fsf:2;
				u_int ftf:2;
				u_int LOP:7;
			}type4;

			struct{
				u_int OPCODE:6;
				u_int imm5:5;
				u_int is:5;
				u_int it:5;
				u_int dest:4;
				u_int LOP:7;
			}type5;

			struct{
				u_int OPCODE:6;
				u_int imm15:15;
				u_int dest:4;
				u_int LOP:7;
			}type6;

			struct{
				u_int imm11:11;
				u_int fs:5;
				u_int it:5;
				u_int dest:4;
				u_int LOP:7;
			}type7;

			struct{
				u_int imm15l:11;
				u_int fs:5;
				u_int it:5;
				u_int imm15u:4;
				u_int LOP:7;
			}type8;

			struct{
				u_int imm24:24;
				u_int :1;
				u_int LOP:7;
			}type9;

			struct{
				u_int immf:32;	// float
			}type10;
		}lower;

		union{
			u_int v;

			struct{
				u_int bc:2;
				u_int OPCODE:4;
				u_int fd:5;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type0;

			struct{
				u_int OPCODE:6;
				u_int fd:5;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type1;

			struct{
				u_int bc:2;
				u_int OPCODE:9;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type2;

			struct{
				u_int OPCODE:11;
				u_int fs:5;
				u_int ft:5;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type3;

			struct{
				u_int OPCODE:11;
				u_int fs:5;
				u_int ft:5;
				u_int fsf:2;
				u_int ftf:2;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type4;

			struct{
				u_int OPCODE:6;
				u_int imm5:5;
				u_int is:5;
				u_int it:5;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type5;

			struct{
				u_int OPCODE:6;
				u_int imm15:15;
				u_int dest:4;
				u_int :2;
				u_int t:1;
				u_int d:1;
				u_int m:1;
				u_int e:1;
				u_int i:1;
			}type6;
		}upper;
	}code;
}sceVuMicroCode;

typedef union{
	u_long128 q;
	u_long d[2];
	u_int w[4];
}_qword;



static struct{
	int step;
	union{
		u_long d[2];
		sceGifTag tag;
	}gif;
	u_long data;
}_gif_;

static struct{
	int step;
	int cl;
	int wl;
	int len;
	sceVifCode code;
	u_int data;
}_vif1_;


static size_t _dbgFormatVif1Pkt(char* buf, size_t len, u_int data);
static size_t _dbgFormatGifPkt(char* buf, size_t len, u_long data);
static size_t _dbgFormatGifPktPacked(char* buf, size_t len, u_int reg, u_long datal, u_long datah);
static size_t _dbgFormatGifPktReg(char* buf, size_t len, u_int reg, u_long data);

typedef DBG_CMD (*DBG_GSSYNCPAHT_CALLBACK)(DBG_GSPATH_STATE state);

static DBG_GSSYNCPAHT_CALLBACK s_dbgGsSyncPathCallBack = NULL;
//static DBG_PRINTF_FUNC s_dbgPrintf = scePrintf;
static DBG_PRINTF_FUNC s_dbgPrintf = printf;

static DBG_PKDUMP s_dbgPktDumpLevel = DBG_PKDUMP_NONE;

#define PRINTF	(s_dbgPrintf)

DBG_PRINTF_FUNC dbgSetPrintfFunc(DBG_PRINTF_FUNC func){
	DBG_PRINTF_FUNC old = s_dbgPrintf;
	s_dbgPrintf = func;
	return old;
}

DBG_PKDUMP dbgSetPktDumpLevel(DBG_PKDUMP level){
	DBG_PKDUMP old = s_dbgPktDumpLevel;
	s_dbgPktDumpLevel = level;
	return old;
}

int dbgGsSyncPath(int mode, int timeout){

	int result;
	int bExit = (0 == mode) ? 0 : 1;

	UNUSED_ALWAYS(timeout);

	do{
		DBG_GSPATH_STATE state = DBG_GSPATH_IDLE;
		DBG_CMD cmd = DBG_CMD_EXIT;

		result = 0;

		if(*D1_CHCR & D_CHCR_STR_M){
			result |= 0x1;
			cmd = DBG_CMD_WAIT;
		}

		if(*D2_CHCR & D_CHCR_STR_M){
			result |= 0x2;
			cmd = DBG_CMD_WAIT;
		}

		switch(dbgGetVif1State()){
		case DBG_VIF1_IDLE:
			break;

		case DBG_VIF1_RUN:
		case DBG_VIF1_WAIT_VU1:
		case DBG_VIF1_WAIT_GIF:
			result |= 0x4;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_VIF1_BUSY;
			dbgDumpVif1State();
			break;

		case DBG_VIF1_FBREAK:
		case DBG_VIF1_STOP:
		case DBG_VIF1_ISTALL:
			result |= 0x4;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_VIF1_STALL;
			dbgDumpVif1State();
			break;
		}

		switch(dbgGetVu1State()){
		case DBG_VU1_IDLE:
			break;

		case DBG_VU1_RUN:
			result |= 0x8;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_VU1_BUSY;
			dbgDumpVu1State();
			break;

		case DBG_VU1_FBREAK:
		case DBG_VU1_TSTOP:
		case DBG_VU1_DSTOP:
			result |= 0x8;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_VU1_STALL;
			dbgDumpVu1State();
			break;
		}

		switch(dbgGetGifState()){
		case DBG_GIF_IDLE:
			break;

		case DBG_GIF_PATH1:
		case DBG_GIF_PATH2:
		case DBG_GIF_PATH3:
			result |= 0x10;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_GIF_BUSY;
			dbgDumpGifState();
			break;

		case DBG_GIF_PAUSE:
			result |= 0x10;
			cmd = DBG_CMD_CONTINUE;
			state |= DBG_GSPATH_GIF_STALL;
			dbgDumpGifState();
			break;
		}

		if(NULL != s_dbgGsSyncPathCallBack){
			cmd = s_dbgGsSyncPathCallBack(state);
		}

		switch(cmd){
		case DBG_CMD_CONTINUE:
			sceDevVif1Continue();
			sceDevVu1Continue();
			sceDevGifContinue();
			break;

		case DBG_CMD_EXIT:
			bExit = TRUE;
			break;

		case DBG_CMD_CANCEL:
			sceDevVif1Reset();
			sceDevVu1Reset();
			sceDevGifReset();
			bExit = TRUE;
			break;

		case DBG_CMD_WAIT:
			break;

		default:
			bExit = TRUE;
			break;
		}

	}while(!bExit);

	return result;
}

DBG_VIF1_STATE dbgGetVif1State(void){
	DBG_VIF1_STATE result = DBG_VIF1_IDLE;

	u_int stat;

	stat = *VIF1_STAT;

	switch(stat & VIF1_STAT_VPS_M){
	case 0:
		result = DBG_VIF1_IDLE;
		break;

	case 1:
	case 2:
		result = DBG_VIF1_RUN;
		break;

	case 3:
		if(stat & VIF1_STAT_VEW_M){
			result = DBG_VIF1_WAIT_VU1;

		}else if(stat & VIF1_STAT_VGW_M){
			result = DBG_VIF1_WAIT_GIF;

		}else{
			result = DBG_VIF1_RUN;
		}
	}

	if(stat & VIF1_STAT_VSS_M){
		result = DBG_VIF1_STOP;

	}else if(stat & VIF1_STAT_VFS_M){
		result = DBG_VIF1_FBREAK;

	}else if(stat & VIF1_STAT_VIS_M){
		result = DBG_VIF1_ISTALL;
	}

	return result;
}

DBG_GIF_STATE dbgGetGifState(void){
	DBG_GIF_STATE result = DBG_GIF_IDLE;

	u_int stat = *GIF_STAT;

	switch((stat & GIF_STAT_APATH_M) >> GIF_STAT_APATH_O){
	case 0:
		result = DBG_GIF_IDLE;
		break;

	case 1:
		result = DBG_GIF_PATH1;
		break;

	case 2:
		result = DBG_GIF_PATH2;
		break;

	case 3:
		result = DBG_GIF_PATH3;
		break;
	}

	if(stat & GIF_STAT_PSE_M){
		result = DBG_GIF_PAUSE;
	}

	return result;
}

DBG_VU1_STATE dbgGetVu1State(void){
	DBG_VU1_STATE result;

	register u_int reg;

	__asm__ volatile ("cfc2 %0, $vi29" :"=r"(reg):);

	if(reg & 0x00000200U){
		result = DBG_VU1_DSTOP;		//	d stop

	}else if(reg & 0x00000400U){
		result = DBG_VU1_TSTOP;		//	t stop

	}else if(reg & 0x00000800U){
		result = DBG_VU1_FBREAK;	//	f break

	}else if(reg & 0x00000100U){
		result = DBG_VU1_RUN;		//	run

	}else{
		result = DBG_VU1_IDLE;		//	idle
	}

	return result;
}

void dbgDumpVu1State(void){

	sceDevVu1Pause();

	{
		int i;
		sceDevVu1Cnd cnd;
		sceDevVu1GetCnd(&cnd);

		PRINTF("--------vu1 register--------------------------\n");

		PRINTF("status    : %03x\n", cnd.status);
		PRINTF("mac       : %04x\n", cnd.mac);
		PRINTF("clip      : %06x\n", cnd.clipping);

		PRINTF("VI00-VI07 : %04x %04x %04x %04x %04x %04x %04x %04x\n",
			   cnd.vi[0], cnd.vi[1], cnd.vi[2], cnd.vi[3], cnd.vi[4], cnd.vi[5], cnd.vi[6], cnd.vi[7]);
		PRINTF("VI08-VI15 : %04x %04x %04x %04x %04x %04x %04x %04x\n",
			   cnd.vi[8], cnd.vi[9], cnd.vi[10], cnd.vi[11], cnd.vi[12], cnd.vi[13], cnd.vi[14], cnd.vi[15]);

		PRINTF("R I Q P   : %08x %08x %08x %08x | %g %g %g %g\n", cnd.r, cnd.i, cnd.q, cnd.p, *(float*)&cnd.r, *(float*)&cnd.i, *(float*)&cnd.q, *(float*)&cnd.p);

		for(i = 0; i < 32; i++){
			float* pf = (float*)&cnd.vf[i];
			u_int* pi = (u_int*)&cnd.vf[i];
			PRINTF("VF%02d.xyzw : %08x %08x %08x %08x | %g %g %g %g\n", i, pi[0], pi[1], pi[2], pi[3], pf[0], pf[1], pf[2], pf[3]);
		}
	}

	{
		u_short tpc = sceDevVu1GetTpc();
		{
			char work[256];
			u_int count = 7;
			u_int index = (tpc < 3) ? 0 : (tpc - 3);

			PRINTF("-------- vu1 micro --------------------------\n");
			while(count--){
				u_long code = __VU1_MICRO__[index];
				dbgFormatVu1Micro(work, sizeof(work), code);

				if(index == tpc){
					PRINTF("%04x:%08x%08x >%s\n", index, (u_int)(code >> 32), (u_int)(code), work);

				}else{
					PRINTF("%04x:%08x%08x  %s\n", index, (u_int)(code >> 32), (u_int)(code), work);
				}
				index++;
			}
		}
	}
}

void dbgDumpVif1State(void){

	u_int stat;
	u_int code;
	char const* const mode[] = { "idle", "wait", "decode", "trans" };
	char const* const mode2[] = { "", "-vu1", "-gif", "-vu1,gif" };
	char work[256];

	stat = *VIF1_STAT;

	sceDevVif1Pause(1);

	code = *VIF1_CODE;

	dbgFormatVif1Code(work, sizeof(work), code);

	PRINTF("--------- vif1 status -------------------------\n");
	PRINTF("DMA ch1. addr=%08x qwc=%08x\n", *D1_TADR, *D1_QWC);
	PRINTF("vif1:%s%s code=%08x[%s]\n",
		   mode[stat & 0x3U],
		   mode2[(stat >> 2) & 0x3U],
		   code,
		   work);

	PRINTF("\tmark=%s stop=%s break=%s interrupt=%s tagMiss=%s error=%s\n",
		   stat & 0x0040U ? "detect" : "no",
		   stat & 0x0100U ? "detect" : "no",
		   stat & 0x0200U ? "detect" : "no",
		   stat & 0x0400U ? "detect" : "no",
		   stat & 0x1000U ? "detect" : "no",
		   stat & 0x2000U ? "detect" : "no");

	//M_PACKETDUMP((void*)((*D1_TADR) - 16), (void*)((*D1_TADR) + 16 * 3));
}

void dbgDumpGifState(void){

	u_int stat;
	u_int cnt;
	char const* const apath[] = { "idle", "path1", "path2", "path3" };

	stat = *GIF_STAT;

	sceDevGifPause();

	cnt = *GIF_CNT;

	PRINTF("--------- gif status -------------------------\n");
	if(3 == ((stat >> 10) & 0x3U)){
		PRINTF("DMA ch2. addr=%08x qwc=%08x\n", *D2_TADR, *D2_QWC);

	}else{
		PRINTF("DMA ch1. addr=%08x qwc=%08x\n", *D1_TADR, *D1_QWC);
	}

	PRINTF("gif apath=%s M3R=%s M3P=%s IMT=%s PSE=%s IP3=%s P3Q=%s P2Q=%s P1Q=%s OPH=%s DIR=%s FQC=%d\n",
		   apath[(stat >> 10) & 0x3U],
		   (stat & 0x0001U) ? "disable" : "enable",
		   (stat & 0x0002U) ? "disable" : "enable",
		   (stat & 0x0004U) ? "1" : "0",
		   (stat & 0x0008U) ? "stall" : "normal",
		   (stat & 0x0020U) ? "int" : "no",
		   (stat & 0x0040U) ? "req" : "no",
		   (stat & 0x0080U) ? "req" : "no",
		   (stat & 0x0100U) ? "req" : "no",
		   (stat & 0x0200U) ? "busy" : "idle",
		   (stat & 0x1000U) ? "gs>ee" : "ee>gs",
		   ((stat >> 28) & 0xfU));

	PRINTF("gif tag = %08x-%08x-%08x-%08x\n", *GIF_TAG3, *GIF_TAG2, *GIF_TAG1, *GIF_TAG0);

	if(0x1 == ((stat >> 10) & 0x3U)){
		PRINTF("vuaddr=%04x regcnt=%04x loopcnt=%04x\n", (cnt >> 20) & 0x3ffU, (cnt >> 16) & 0xfU, cnt & 0x7fffU);
	}
}


size_t dbgFormatVif1Code(char* buf, u_int len, u_int code){
	char work[256];

	sceVifCode const c = { code };

	char const* const iBit = c.i.itr ? "i" : "";

	switch(c.i.cmd){
	case SCE_VIF_NOP:
		sprintf(work, "%sNOP", iBit);
		break;

	case SCE_VIF_STCYCL:
		sprintf(work, "%sSTCYCL[wl=%d,cl=%d]", iBit, (c.i.imm >> 8), (c.i.imm & 0xffU));
		break;

	case SCE_VIF_OFFSET:
		sprintf(work, "%sOFFSET[%04x]", iBit, c.i.imm);
		break;

	case SCE_VIF_BASE:
		sprintf(work, "%sBASE[%04x]", iBit, c.i.imm);
		break;

	case SCE_VIF_ITOP:
		sprintf(work, "%sITOP[%04x]", iBit, c.i.imm);
		break;

	case SCE_VIF_STMOD:
		sprintf(work, "%sSTMOD[%d]", iBit, (c.i.imm & 0x3U));
		break;

	case SCE_VIF_MSKPATH3:
		sprintf(work, "%sMSKPATH3[%s]", iBit, (c.i.imm & 0x8000U) ? "disable" : "enable");
		break;

	case SCE_VIF_MARK:
		sprintf(work, "%sMARK[%d]", iBit, c.i.imm);
		break;

	case SCE_VIF_FLUSHE:
		sprintf(work, "%sFLUSHE", iBit);
		break;

	case SCE_VIF_FLUSH:
		sprintf(work, "%sFLUSH", iBit);
		break;

	case SCE_VIF_FLUSHA:
		sprintf(work, "%sFLUSHA", iBit);
		break;

	case SCE_VIF_MSCAL:
		sprintf(work, "%sMSCAL[%08x]", iBit, c.i.imm);
		break;

	case SCE_VIF_MSCALF:
		sprintf(work, "%sMSCALF[%08x]", iBit, c.i.imm);
		break;

	case SCE_VIF_MSCNT:
		sprintf(work, "%sMSCNT", iBit);
		break;

	case SCE_VIF_STMASK:
		sprintf(work, "%sSTMASK", iBit);
		break;

	case SCE_VIF_STROW:
		sprintf(work, "%sSTROW", iBit);
		break;

	case SCE_VIF_STCOL:
		sprintf(work, "%sSTCOL", iBit);
		break;

	case SCE_VIF_MPG:
		sprintf(work, "%sMPG[addr=0x%08x,num=%d]", iBit, c.i.imm, c.i.num);
		break;

	case SCE_VIF_DIRECT:
		sprintf(work, "%sDIRECT[count=%08x]", iBit, c.i.imm);
		break;

	case SCE_VIF_DIRECTHL:
		sprintf(work, "%sDIRECTHL[count=%08x]", iBit, c.i.imm);
		break;

	default:
		if(SCE_VIF_UNPACK == (c.i.cmd & 0x60U)){
			char const* const upk[] = {
				"S_32", "S_16", "S_8", "unknown",
				"V2_32", "V2_16", "V2_8", "unknown",
				"V3_32", "V3_16", "V3_8", "unknown",
				"V4_32", "V4_16", "V4_8", "V4_5"
			};

			sprintf(work, "%sUPK_%s[msk=%d,flg=%d,usn=%d,addr=%08x,num=%d]", iBit, upk[c.i.cmd & 0xfU], (c.i.cmd >> 4) & 1, (c.i.imm >> 15) & 1, (c.i.imm >> 14) & 1, (c.i.imm & 0x3fffU), c.i.num);

		}else{
			sprintf(work, "unknown");
		}
	}

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}


size_t dbgFormatVu1Micro(char* buf, u_int len, u_long code){
	char work[256];
	char work_u[128];
	char work_l[128];

	char const* const dfield[16] = {
		"----", "---w", "--z-", "--zw",
		"-y--", "-y-w", "-yz-", "-yzw",
		"x---", "x--w", "x-z-", "x-zw",
		"xy--", "xy-w", "xyz-", "xyzw"
	};
	char const* const rfield[4] = {
		"x", "y", "z", "w"
	};

	sceVuMicroCode micro;

	micro.v = code;

	sprintf(work_u, "unknown");
	sprintf(work_l, "unknown");

	{	// upper
		u_int op;
		op = (micro.code.upper.v >> 2) & 0xfU;

		switch(op){
		case 0x0U:
		case 0x1U:
		case 0x2U:
		case 0x3U:
		case 0x4U:
		case 0x5U:
		case 0x6U:
		{
			char const* const type0[] = {
				"ADD",
				"SUB",
				"MADD",
				"MSUB",
				"MAX",
				"MINI",
				"MUL",
			};
			sprintf(work_u, "%s%s.%s\tVF%02d, VF%02d, VF%02d",
					type0[op],
					rfield[micro.code.upper.type0.bc],
					dfield[micro.code.upper.type0.dest],
					micro.code.upper.type0.fd,
					micro.code.upper.type0.fs,
					micro.code.upper.type0.ft);
		}
		break;

		case 0x7U:
		case 0x8U:
		case 0x9U:
		{
			char const* const type1[][4] = {
				{
					"MULq.%s\tVF%02d, VF%02d, Q",
					"MAXi.%s\tVF%02d, VF%02d, I",
					"MULi.%s\tVF%02d, VF%02d, I",
					"MINIi.%s\tVF%02d, VF%02d, I"
				},{
					"ADDq.%s\tVF%02d, VF%02d, Q",
					"MADDq.%s\tVF%02d, VF%02d, Q",
					"ADDi.%s\tVF%02d, VF%02d, I",
					"MADDi.%s\tVF%02d, VF%02d, I"
				},{
					"SUBq.%s\tVF%02d, VF%02d, Q",
					"MSUBq.%s\tVF%02d, VF%02d, Q",
					"SUBi.%s\tVF%02d, VF%02d, I",
					"MSUBi.%s\tVF%02d, VF%02d, I"
				}
			};

			u_int bc = micro.code.upper.v & 0x3;
			sprintf(work_u, type1[op - 0x7][bc],
					dfield[micro.code.upper.type0.dest],
					micro.code.upper.type0.fd,
					micro.code.upper.type0.fs);
		}
		break;

		case 0xaU:
		case 0xbU:
		{
			char const* const type1[][4] = {
				{ "ADD", "MADD", "MUL", "MAX" },
				{ "SUB", "MSUB", "OPMSUB", "MINI" }
			};
			u_int bc = micro.code.upper.v & 0x3;
			sprintf(work_u, "%s.%s\tVF%02d, VF%02d, VF%02d",
					type1[op - 0xa][bc],
					dfield[micro.code.upper.type0.dest],
					micro.code.upper.type0.fd,
					micro.code.upper.type0.fs,
					micro.code.upper.type0.ft);
		}
		break;

		case 0xfU:	// special
		{
			op = (micro.code.upper.v >> 6) & 0x1fU;
			switch(op){
			case 0x0U:
			case 0x1U:
			case 0x2U:
			case 0x3U:
			case 0x6U:
			{
				char const* const type2[] = {
					"ADDA",
					"SUBA",
					"MADDA",
					"MSUBA",
					"MULA",
				};
				sprintf(work_u, "%s%s.%s\tACC, VF%02d, VF%02d",
						type2[op],
						rfield[micro.code.upper.type0.bc],
						dfield[micro.code.upper.type0.dest],
						micro.code.upper.type0.fs,
						micro.code.upper.type0.ft);
			}
			break;

			case 0x4U:
			case 0x5U:
			{
				char const* const type3[][4] = {
					{ "ITOF0", "ITOF4", "ITOF12", "ITOF15" },
					{ "FTOI0", "FTOI4", "FTOI12", "FTOI15" }
				};

				u_int bc = micro.code.upper.v & 0x3U;
				sprintf(work_u, "%s.%s\tVF%02d, VF%02d",
						type3[op - 0x4][bc],
						dfield[micro.code.upper.type0.dest],
						micro.code.upper.type0.ft,
						micro.code.upper.type0.fs);
			}
			break;

			case 0x7U:
			case 0x8U:
			case 0x9U:
			{
				char const* const type3[][4] = {
					{
						"MULAq.%s\tACC, VF%02d, Q",
						"ABS.%s\tVF%02d, VF%02d",
						"MULAi.%s\tACC, VF%02d, I",
						"CLIPw.%s\tVF%02d, VF%02d"
					},{
						"ADDAq.%s\tACC, VF%02d, Q",
						"MADDAq.%s\tACC, VF%02d, Q",
						"ADDAi.%s\tACC, VF%02d, I",
						"MADDAi.%s\tACC, VF%02d, I"
					},{
						"SUBAq.%s\tACC, VF%02d, Q",
						"MSUBAq.%s\tACC, VF%02d, Q",
						"SUBAi.%s\tACC, VF%02d, I",
						"MSUBAi.%s\tACC, VF%02d, I"
					}
				};

				u_int bc = micro.code.upper.v & 0x3U;
				if((0x7U == op) && (1U == bc)){
					sprintf(work_u, type3[op - 0x7][bc],
							dfield[micro.code.upper.type0.dest],
							micro.code.upper.type0.ft,
							micro.code.upper.type0.fs);

				}else if((0x7U == op) && (3U == bc)){
					sprintf(work_u, type3[op - 0x7][bc],
							dfield[micro.code.upper.type0.dest],
							micro.code.upper.type0.fs,
							micro.code.upper.type0.ft);

				}else{
					sprintf(work_u, type3[op - 0x7][bc],
							dfield[micro.code.upper.type0.dest],
							micro.code.upper.type0.fs);
				}
			}
			break;

			case 0xaU:
			case 0xbU:
			{
				char const* const type3[][4] = {
					{ "ADDA", "MADDA", "MULA", "" },
					{ "SUBA", "MSUBA", "OPMULA", "" }
				};

				u_int bc = micro.code.upper.v & 0x3;
				if(0x3U == bc){
					if(0xbU == op){
						sprintf(work_u, "NOP\t\t");
					}
					break;

				}else{
					sprintf(work_u, "%s.%s\tACC, VF%02d, VF%02d",
							type3[op - 0xa][bc],
							dfield[micro.code.upper.type0.dest],
							micro.code.upper.type0.fs,
							micro.code.upper.type0.ft);
				}
			}
			break;

			default:
				break;
			}
	  
		}
		break;

		default:
			break;
		}
	}

	if(micro.mode.i){
		sprintf(work_l, "LOI\t%g", micro.code.lower.f);

	}else{	// lower
		u_int op = micro.code.lower.v >> 25;
		switch(op){
		case 0x00U:	// 7 sq.dest	vf[fs], imm11(vi[it])
			sprintf(work_l, "SQ.%s\tVF%02d, 0x%03x(VI%02d)",
					dfield[micro.code.lower.type7.dest],
					micro.code.lower.type7.fs,
					micro.code.lower.type7.imm11,
					micro.code.lower.type7.it);
			break;

		case 0x01U:	// 7 lq.dest	vi[ft], imm11(vi[is])
			sprintf(work_l, "LQ.%s\tVF%02d, 0x%03x(VI%02d)",
					dfield[micro.code.lower.type7.dest],
					micro.code.lower.type7.it,	// ft
					micro.code.lower.type7.imm11,
					micro.code.lower.type7.fs);	// is
			break;

		case 0x04U:	// 7 ilw.dest	vi[it], imm11(vi[is])
			sprintf(work_l, "ILW.%s\tVI%02d, 0x%03x(VI%02d)",
					dfield[micro.code.lower.type7.dest],
					micro.code.lower.type7.it,
					micro.code.lower.type7.imm11,
					micro.code.lower.type7.fs);	// is
			break;

		case 0x05U:	// 7 isw.dest	vi[it], imm11(vi[is])
			sprintf(work_l, "ISW.%s\tVI%02d, 0x%03x(VI%02d)",
					dfield[micro.code.lower.type7.dest],
					micro.code.lower.type7.it,
					micro.code.lower.type7.imm11,
					micro.code.lower.type7.fs);	// is
			break;

		case 0x08U:	// 8 iaddiu	vi[it], vi[is], imm15
			sprintf(work_l, "IADDIU\tVI%02d, VI%02d, 0x%04x",
					micro.code.lower.type8.it,
					micro.code.lower.type8.fs,	// is
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x09U:	// 8 isubiu	vi[it], vi[is], imm15
			sprintf(work_l, "ISUBIU\tVI%02d, VI%02d, 0x%04x",
					micro.code.lower.type8.it,
					micro.code.lower.type8.fs,	// is
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x10U:	// 9 fceq	vi01, imm24
			sprintf(work_l, "FCEQ\tVI01, 0x%06x",
					micro.code.lower.type9.imm24);
			break;

		case 0x11U:	// 9 fcset	imm24
			sprintf(work_l, "FCSET\t0x%06x",
					micro.code.lower.type9.imm24);
			break;

		case 0x12U:	// 9 fcand	vi01, imm24
			sprintf(work_l, "FCAND\tVI01, 0x%06x",
					micro.code.lower.type9.imm24);
			break;

		case 0x13U:	// 9 fcor	vi01, imm24
			sprintf(work_l, "FCOR\tVI01, 0x%06x",
					micro.code.lower.type9.imm24);
			break;

		case 0x14U:	// 8 fseq	vi[it], imm12
			sprintf(work_l, "FSEQ\tVI%02d, 0x%03x",
					micro.code.lower.type8.it,
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x15U:	// 8 fsset	imm12
			sprintf(work_l, "FSEQ\t0x%03x",
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x16U:	// 8 fsand	vi[it], imm12
			sprintf(work_l, "FSAND\tVI%02d, 0x%03x",
					micro.code.lower.type8.it,
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x17U:	// 8 fsor	vi[it], imm12
			sprintf(work_l, "FSOR\tVI%02d, 0x%03x",
					micro.code.lower.type8.it,
					micro.code.lower.type8.imm15l | (micro.code.lower.type8.imm15u << 11));
			break;

		case 0x18U:	// 8 fmeq	vi[it], vi[is]
			sprintf(work_l, "FMEQ\tVI%02d, VI%02d",
					micro.code.lower.type8.it,
					micro.code.lower.type8.fs);	// is
			break;

		case 0x1aU:	// 8 fmand	vi[it], vi[is]
			sprintf(work_l, "FMAND\tVI%02d, VI%02d",
					micro.code.lower.type8.it,
					micro.code.lower.type8.fs);	// is
			break;

		case 0x1bU:	// 8 fmor	vi[it], vi[is]
			sprintf(work_l, "FMOR\tVI%02d, VI%02d",
					micro.code.lower.type8.it,
					micro.code.lower.type8.fs);	// is
			break;

		case 0x1cU:	// 8 fcget	vi[it]
			sprintf(work_l, "FCGET\tVI%02d",
					micro.code.lower.type8.it);
			break;

		case 0x20U:	// 7 b		imm11
			sprintf(work_l, "B\t0x%03x",
					micro.code.lower.type7.imm11);
			break;

		case 0x21U:	// 7 bal	vi[it], imm11
			sprintf(work_l, "BAL\tVI%02d, 0x%03x",
					micro.code.lower.type7.it,
					micro.code.lower.type7.imm11);
			break;

		case 0x24U:	// 7 jr		vi[is]
			sprintf(work_l, "JR\tVI%02d",
					micro.code.lower.type7.fs);	// is
			break;

		case 0x25U:	// 7 jalr	vi[it], vi[is]
			sprintf(work_l, "JALR\tVI%02d, VI%02d",
					micro.code.lower.type7.it,
					micro.code.lower.type7.fs);	// is
			break;

		case 0x28U:	// 7 ibeq	vi[it], vi[is], imm11
			sprintf(work_l, "IBEQ\tVI%02d, VI%02d, 0x%03x",
					micro.code.lower.type7.it,
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x29U:	// 7 ibne	vi[it], vi[is], imm11
			sprintf(work_l, "IBNE\tVI%02d, VI%02d, 0x%03x",
					micro.code.lower.type7.it,
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x2cU:	// 7 ibltz	vi[is], imm11
			sprintf(work_l, "IBLTZ\tVI%02d, 0x%03x",
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x2dU:	// 7 ibgtz	vi[is], imm11
			sprintf(work_l, "IBGTZ\tVI%02d, 0x%03x",
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x2eU:	// 7 iblez	vi[is], imm11
			sprintf(work_l, "IBLEZ\tVI%02d, 0x%03x",
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x2fU:	// 7 ibgez	vi[is], imm11
			sprintf(work_l, "IBGEZ\tVI%02d, 0x%03x",
					micro.code.lower.type7.fs,	// is
					micro.code.lower.type7.imm11);
			break;

		case 0x40U:	// major & special
		{
			op = micro.code.lower.v & 0x3fU;
			switch(op){
			case 0x030U:	// 1 iadd	vi[id], vi[is], vi[it]
				sprintf(work_l, "IADD\tVI%02d, VI%02d, VI%02d",
						micro.code.lower.type1.fd,	// id
						micro.code.lower.type1.fs,	// is
						micro.code.lower.type1.ft);	// it
				break;

			case 0x031U:	// 1 isub	vi[id], vi[is], vi[it]
				sprintf(work_l, "ISUB\tVI%02d, VI%02d, VI%02d",
						micro.code.lower.type1.fd,	// id
						micro.code.lower.type1.fs,	// is
						micro.code.lower.type1.ft);	// it
				break;

			case 0x032U:	// 5 iaddi	vi[it], vi[is], imm5
				sprintf(work_l, "IADDI\tVI%02d, VI%02d, 0x%02x",
						micro.code.lower.type5.it,
						micro.code.lower.type5.is,
						micro.code.lower.type5.imm5);
				break;

			case 0x034U:	// 1 iand	vi[id], vi[is], vi[it]
				sprintf(work_l, "IAND\tVI%02d, VI%02d, VI%02d",
						micro.code.lower.type1.fd,	// id
						micro.code.lower.type1.fs,	// is
						micro.code.lower.type1.ft);	// it
				break;

			case 0x035U:	// 1 ior	vi[id], vi[is], vi[it]
				sprintf(work_l, "IOR\tVI%02d, VI%02d, VI%02d",
						micro.code.lower.type1.fd,	// id
						micro.code.lower.type1.fs,	// is
						micro.code.lower.type1.ft);	// it
				break;

			case 0x03cU:
			case 0x03dU:
			case 0x03eU:
			case 0x03fU:
			{
				op = micro.code.lower.v & 0x7ffU;
				switch(op){
				case 0x33cU:	// 3 move.dest	vf[ft], vf[fs]
					sprintf(work_l, "MOVE.%s\tVF%02d, VF%02d",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,
							micro.code.lower.type3.fs);
					break;

				case 0x33dU:	// 3 mr32.dest	vf[ft], vf[fs]
					sprintf(work_l, "MR32.%s\tVF%02d, VF%02d",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,
							micro.code.lower.type3.fs);
					break;

				case 0x37cU:	// 3 lqi.dest	vf[ft], (vi[is]++)
					sprintf(work_l, "LQI.%s\tVF%02d, (VI%02d++)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,
							micro.code.lower.type3.fs);	// is
					break;

				case 0x37dU:	// 3 sqi.dest	vf[fs], (vi[it]++)
					sprintf(work_l, "SQI.%s\tVF%02d, (VI%02d++)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.fs,
							micro.code.lower.type3.ft);	// it
					break;

				case 0x37eU:	// 3 lqd.dest	vf[ft], (--vi[is])
					sprintf(work_l, "LQD.%s\tVF%02d, (--VI%02d)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,
							micro.code.lower.type3.fs);	// is
					break;

				case 0x37fU:	// 3 sqd.dest	vf[fs], (--vi[it])
					sprintf(work_l, "SQD.%s\tVF%02d, (--VI%02d)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.fs,
							micro.code.lower.type3.ft);	// it
					break;

				case 0x3bcU:	// 4 div	q, vf[fs]fsf, vf[ft]ftf
					sprintf(work_l, "DIV\tQ, VF%02d%s, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf],
							micro.code.lower.type4.ft,
							rfield[micro.code.lower.type4.ftf]);
					break;

				case 0x3bdU:	// 4 sqrt	q, vf[ft]ftf
					sprintf(work_l, "SQRT\tQ, VF%02d%s",
							micro.code.lower.type4.ft,
							rfield[micro.code.lower.type4.ftf]);
					break;

				case 0x3beU:	// 4 rsqrt	q, vf[fs]fsf, vf[ft]ftf
					sprintf(work_l, "RSQRT\tQ, VF%02d%s, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf],
							micro.code.lower.type4.ft,
							rfield[micro.code.lower.type4.ftf]);
					break;

				case 0x3bfU:	// 3 waitq
					sprintf(work_l, "WAITQ");

				case 0x3fcU:	// 4 mtir	vi[it], vf[fs]fsf
					sprintf(work_l, "MTIR\tVI%02d, VF%02d%s",
							micro.code.lower.type4.ft,	// it
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x3fdU:	// 3 mfir.dest	vf[ft], vi[is]
					sprintf(work_l, "MFIR.%s\tVF%02d, VI%02d",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,
							micro.code.lower.type3.fs);	// is
					break;

				case 0x3feU:	// 3 ilwr.dest	vi[it], (vi[is])
					sprintf(work_l, "ILWR.%s\tVI%02d, (VI%02d)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,	// it
							micro.code.lower.type3.fs);	// is
					break;

				case 0x3ffU:	// 3 iswr.dest	vi[it], (vi[is])
					sprintf(work_l, "ISWR.%s\tVI%02d, (VI%02d)",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft,	// it
							micro.code.lower.type3.fs);	// is
					break;

				case 0x43cU:	// 3 rnext.dest	vf[ft], r
					sprintf(work_l, "RNEXT.%s\tVF%02d, R",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft);
					break;

				case 0x43dU:	// 3 rget.dest	vf[ft], r
					sprintf(work_l, "RGET.%s\tVF%02d, R",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft);
					break;

				case 0x43eU:	// 4 rinit	r, vf[fs]fsf
					sprintf(work_l, "RINIT\tR, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x43fU:	// 4 rxor	r, vf[fs]fsf
					sprintf(work_l, "RXOR\tR, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x67cU:	// 3 mfp.dest	vf[ft], p
					sprintf(work_l, "MFP.%s\tVF%02d, P",
							dfield[micro.code.lower.type3.dest],
							micro.code.lower.type3.ft);
					break;

				case 0x6bcU:	// 3 xtop	vi[it]
					sprintf(work_l, "XTOP\tVI%02d",
							micro.code.lower.type3.ft);	// it
					break;

				case 0x6bdU:	// 3 xitop	vi[it]
					sprintf(work_l, "XITOP\tVI%02d",
							micro.code.lower.type3.ft);	// it
					break;

				case 0x6fcU:	// 3 xgkick	vi[is]
					sprintf(work_l, "XGKICK\tVI%02d",
							micro.code.lower.type3.fs);	// is
					break;

				case 0x73cU:	// 3 esadd	p, vf[fs]
					sprintf(work_l, "ESADD\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x73dU:	// 3 ersadd	p, vf[fs]
					sprintf(work_l, "ERSADD\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x73eU:	// 3 eleng	p, vf[fs]
					sprintf(work_l, "ELENG\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x73fU:	// 3 erleng	p, vf[fs]
					sprintf(work_l, "ERLENG\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x77cU:	// 3 eatanxy	p, vf[fs]
					sprintf(work_l, "EATANXY\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x77dU:	// 3 eatanxz	p, vf[fs]
					sprintf(work_l, "EATANXZ\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x77eU:	// 3 esum	p, vf[fs]
					sprintf(work_l, "ESUM\tP, VF%02d",
							micro.code.lower.type3.fs);
					break;

				case 0x7bcU:	// 4 esqrt	p, vf[fs]fsf
					sprintf(work_l, "ESQRT\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x7bdU:	// 4 ersqrt	p, vf[fs]fsf
					sprintf(work_l, "ERSQRT\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x7beU:	// 4 ercpr	p, vf[fs]fsf
					sprintf(work_l, "ERCPR\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x7bfU:	// 3 waitp
					sprintf(work_l, "WAITP");

				case 0x7fcU:	// 4 esin	p, vf[fs]fsf
					sprintf(work_l, "ESIN\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x7fdU:	// 4 eatan	p, vf[fs]fsf
					sprintf(work_l, "EATAN\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				case 0x7feU:	// 4 eexp	p, vf[fs]fsf
					sprintf(work_l, "EEXP\tP, VF%02d%s",
							micro.code.lower.type4.fs,
							rfield[micro.code.lower.type4.fsf]);
					break;

				default:
					break;
				}
			}

			default:
				break;
			}
		}
		break;

		default:
			break;
		}
	}

	sprintf(work, "[%c%c%c%c%c] %s\t%s",
			micro.mode.i ? 'I' : '-',
			micro.mode.e ? 'E' : '-',
			micro.mode.m ? 'M' : '-',
			micro.mode.d ? 'D' : '-',
			micro.mode.t ? 'T' : '-',
			work_u, work_l);

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}

size_t dbgFormatDmaTag(char* buf, u_int len, u_long128 tag){
	char work[256];

	union{
		u_long128 image;
		struct{
			u_int qwc:16;
			u_int pad0:10;
			u_int pce:2;
			u_int id:3;
			u_int irq:1;
			u_int addr:31;
			u_int spr:1;
			u_int opt0:32;
			u_int opt1:32;
		}tag;
	}const dma = { tag };

	switch(dma.tag.id){
	case 0:
		sprintf(work, "REFE[ref=%08x,qwc=%08x,pce=%d,irq=%s]", dma.tag.spr ? 0x70000000U | dma.tag.addr : dma.tag.addr, dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 1:
		sprintf(work, "CNT[qwc=%08x,pce=%d,irq=%s]", dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 2:
		sprintf(work, "NEXT[next=%08x,qwc=%08x,pce=%d,irq=%s]", dma.tag.spr ? 0x70000000U | dma.tag.addr : dma.tag.addr, dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 3:
		sprintf(work, "REF[ref=%08x,qwc=%08x,pce=%d,irq=%s]", dma.tag.spr ? 0x70000000U | dma.tag.addr : dma.tag.addr, dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 4:
		sprintf(work, "REFS[ref=%08x,qwc=%08x,pce=%d,irq=%s]", dma.tag.spr ? 0x70000000U | dma.tag.addr : dma.tag.addr, dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 5:
		sprintf(work, "CALL[call=%08x,qwc=%08x,pce=%d,irq=%s]", dma.tag.spr ? 0x70000000U | dma.tag.addr : dma.tag.addr, dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 6:
		sprintf(work, "RET[qwc=%08x,pce=%d,irq=%s]", dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;

	case 7:
		sprintf(work, "END[qwc=%08x,pce=%d,irq=%s]", dma.tag.qwc, dma.tag.pce, dma.tag.irq ? "true" : "false");
		break;
	}

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}

size_t dbgFormatGifTag(char* buf, u_int len, u_long tag){

	char work[128];

	union{
		u_long image;
		struct{
			u_int nloop:15;
			u_int eop:1;
			u_int pad0:16;
			u_int pad1:14;
			u_int pre:1;
			u_int prim:11;
			u_int flg:2;
			u_int nreg:4;
		}tag;
	}const gif = { tag };

	char const * const _mode[] = {
		"packed",
		"reglist",
		"image",
		"*image*",
	};

	if(0 != gif.tag.pre){
		sprintf(work, "gif[nloop=%d,eop=%d,pre=%d,prim=%03x,flg=%d<%s>,nreg=%d]", gif.tag.nloop, gif.tag.eop, gif.tag.pre, gif.tag.prim, gif.tag.flg, _mode[gif.tag.flg], gif.tag.nreg);
	}else{
		sprintf(work, "gif[nloop=%d,eop=%d,pre=%d,prim=---,flg=%d<%s>,nreg=%d]", gif.tag.nloop, gif.tag.eop, gif.tag.pre, gif.tag.flg, _mode[gif.tag.flg], gif.tag.nreg);
	}

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}


void dbgResetPacketState(void){
	_gif_.step = 0;
	_vif1_.step = 0;
}

void dbgDumpPacketDma(u_long128 const* pkt, u_long128 const* limit, int path, DBG_PKDUMP level){
	_qword const* const tail = (_qword const*)((u_int)limit & ~15);
	_qword const* ptr = (_qword const*)(((u_int)pkt + 15) & ~15);
	_qword const* call0 = NULL;
	_qword const* call1 = NULL;
	_qword const* next = NULL;
	u_int id = 1;		// ‰‰ñƒ_ƒ~[
	u_int count = 0;
	u_int addr = 0;

	char work[256];

	if(DBG_PKDUMP_NONE == level){
		return;
	}

	PRINTF("dbgDumpPacketDma(%08x,%08x,%d,..)\n", (u_int)pkt, (u_int)limit, path);

	while(ptr != tail){
		if(0 == count){

			if((7 == id) || (0 == id)){
				break;
			}

			if(NULL != next){
				if(tail == next){
					break;
				}
				ptr = next;
				next = NULL;
			}
	
			dbgFormatDmaTag(work, sizeof(work), ptr->q);
			PRINTF("%08x : %08x %08x %08x %08x : %s", (u_int)ptr, ptr->w[0], ptr->w[1], ptr->w[2], ptr->w[3], work);
			if((0 == path) && (DBG_PKDUMP_VIFL <= level)){
				_dbgFormatVif1Pkt(work, sizeof(work), ptr->w[2]);
				PRINTF(" %s", work);

				_dbgFormatVif1Pkt(work, sizeof(work), ptr->w[3]);
				PRINTF(" %s", work);
			}
			PRINTF("\n");

			id = (ptr->w[0] >> 28) & 0x7;

			count = (u_int)(ptr->w[0] & 0x0000ffffU);

			addr = ptr->w[1];
			if(0 != (addr >> 31)){
				addr = (addr & 0x0fffffffU) | 0x70000000U;
			}else{
				addr = (addr & 0x0fffffffU) | 0x20000000U;
			}

			ptr++;

			switch(id){
			case 0:
			case 3:
			case 4:
				if(DBG_PKDUMP_DMAL >= level){
					count = (count > 4) ? 4 : count;
				}
				next = ptr;
				ptr = (_qword const*)addr;
				break;

			case 2:
				next = (_qword const*)addr;
				break;

			case 5:
				if(NULL == call0){
					call0 = ptr;
					next = (_qword const*)addr;

				}else if(NULL == call1){
					call1 = ptr;
					next = (_qword const*)addr;

				}else{
					PRINTF("error : call");
				}
				break;

			case 6:
				if(call1){
					next = call1;
					call1 = NULL;

				}else if(call0){
					next = call0;
					call0 = NULL;

				}else{
					PRINTF("error : ret\n");
				}
				break;
			}

		}else{
			if(DBG_PKDUMP_DMA < level){
				PRINTF("%08x : %08x %08x %08x %08x :", (u_int)ptr, ptr->w[0], ptr->w[1], ptr->w[2], ptr->w[3]);
				if(0 == path){
					if(DBG_PKDUMP_VIFL <= level){
						int i;
						for(i = 0; i < 4; i++){
							_dbgFormatVif1Pkt(work, sizeof(work), ptr->w[i]);
							PRINTF(" %s", work);
						}
					}
				}else{
					if(DBG_PKDUMP_GIFL <= level){
						int i;
						for(i = 0; i < 2; i++){
							_dbgFormatGifPkt(work, sizeof(work), ptr->d[i]);
							PRINTF(" %s", work);
						}
					}
				}
				PRINTF("\n");
			}
			count--;
			ptr++;
		}
    }
}

void dbgDumpPacketGif(u_long128 const* pkt, u_long128 const* limit, DBG_PKDUMP level){
	_qword const* const tail = (_qword const*)((u_int)limit & ~15);
	_qword const* const head = (_qword const*)(((u_int)pkt + 15) & ~15);
	_qword const* ptr;
	char work[256];

	if(DBG_PKDUMP_NONE == level){
		return;
	}

	PRINTF("dbgDumpPacketGif(%08x,%08x,..)\n", (u_int)pkt, (u_int)limit);

	for(ptr = head; ptr != tail; ptr++){

		PRINTF("%08x : %08x %08x %08x %08x :", (u_int)ptr, ptr->w[0], ptr->w[1], ptr->w[2], ptr->w[3]);
		if(DBG_PKDUMP_GIFL <= level){
			int i;
			for(i = 0; i < 2; i++){
				_dbgFormatGifPkt(work, sizeof(work), ptr->d[i]);
				PRINTF(" %s", work);
			}
		}
		PRINTF("\n");
	}
}

static size_t _dbgFormatVif1Pkt(char* buf, size_t len, u_int data){
	size_t result;

	if(0 == _vif1_.step){
		result = dbgFormatVif1Code(buf, len, data);

		_vif1_.code.d = data;

		switch(_vif1_.code.i.cmd){
		case SCE_VIF_NOP:
		case SCE_VIF_OFFSET:
		case SCE_VIF_BASE:
		case SCE_VIF_ITOP:
		case SCE_VIF_STMOD:
		case SCE_VIF_MSKPATH3:
		case SCE_VIF_MARK:
		case SCE_VIF_FLUSHE:
		case SCE_VIF_FLUSH:
		case SCE_VIF_FLUSHA:
		case SCE_VIF_MSCAL:
		case SCE_VIF_MSCALF:
		case SCE_VIF_MSCNT:
			_vif1_.len = 1;
			break;

		case SCE_VIF_STCYCL:
			_vif1_.cl = (int)(_vif1_.code.i.imm & 0xffU);
			_vif1_.wl = (int)((_vif1_.code.i.imm >> 8) & 0xffU);
			_vif1_.len = 1;
			break;

		case SCE_VIF_STMASK:
			_vif1_.len = 1 + 1;
			break;

		case SCE_VIF_STROW:
		case SCE_VIF_STCOL:
			_vif1_.len = 1 + 4;
			break;

		case SCE_VIF_MPG:
			{
				int num = (0 == _vif1_.code.i.num) ? 256 : _vif1_.code.i.num;
				_vif1_.len = 1 + num * 2;
			}
			break;

		case SCE_VIF_DIRECT:
		case SCE_VIF_DIRECTHL:
			{
				int imm = (0 == _vif1_.code.i.imm) ? 65536 : _vif1_.code.i.imm;
				_vif1_.len = 1 + imm * 4;
			}
			break;

		default:
			if(SCE_VIF_UNPACK == (_vif1_.code.i.cmd & 0x60U)){
				int cl = _vif1_.cl;
				int wl = _vif1_.wl;
				int vl = _vif1_.code.i.cmd & 0x2U;
				int vn = (_vif1_.code.i.cmd >> 2) & 0x2U;
				int num = (0 == _vif1_.code.i.num) ? 256 : _vif1_.code.i.num;
				int n;

				if(wl <= cl){
					n = num;
				}else{
					int _wl = num % wl;
					n = cl * (num / wl) + ((_wl > cl) ? cl : _wl);
				}

				_vif1_.len = 1 + ((32 >> vl) * (vn + 1) * n + 31) / 32;

			}else{
				_vif1_.len = 1;
			}
			break;
		}

	}else{
		if((SCE_VIF_DIRECT == _vif1_.code.i.cmd) || (SCE_VIF_DIRECTHL == _vif1_.code.i.cmd)){
			if(0 == (_vif1_.step & 1)){
				result = _dbgFormatGifPkt(buf, len, (u_long)data << 32 | (u_long)_vif1_.data);
			}else{
				_vif1_.data = data;
				*buf = '\0';
				result = 0;
			}

		}else{
			*buf = '\0';
			result = 0;
		}
	}

	_vif1_.step++;

	if(_vif1_.step == _vif1_.len){
		_vif1_.step = 0;
	}

	return result;
}

static size_t _dbgFormatGifPkt(char* buf, size_t len, u_long data){

	if(0 == _gif_.step){
		_gif_.gif.d[0] = data;
		_gif_.step++;
		return dbgFormatGifTag(buf, len, data);

	}else if(1 == _gif_.step){
		_gif_.gif.d[1] = data;
		_gif_.step++;

		if(0 == _gif_.gif.tag.NLOOP){
			_gif_.step = 0;
		}

		*buf = '\0';
		return 0;

	}else{
		int offset = _gif_.step - 2;
		_gif_.step++;

		switch((int)_gif_.gif.tag.FLG){
		case 0:		// packed
			if(0 == (offset & 1)){
				_gif_.data = data;
				*buf = '\0';
				return 0;

			}else{
				int nreg = (0 == _gif_.gif.tag.NREG) ? 16 : _gif_.gif.tag.NREG;
				u_int reg = (_gif_.gif.d[1] >> ((offset % nreg) * 4)) & 0xfU;

				if(((offset + 1) / 2) == (_gif_.gif.tag.NLOOP * nreg)){
					_gif_.step = 0;
				}

				return _dbgFormatGifPktPacked(buf, len, reg, _gif_.data, data);
			}
			break;

		case 1:		// reglist
			{
				int nreg = (0 == _gif_.gif.tag.NREG) ? 16 : _gif_.gif.tag.NREG;
				u_int reg = (_gif_.gif.d[1] >> ((offset % nreg) * 4)) & 0xfU;

				if((offset + 1) == (_gif_.gif.tag.NLOOP * nreg)){
					_gif_.step = 0;
				}

				return _dbgFormatGifPktReg(buf, len, reg, data);
			}
			break;

		case 2:		// image
		case 3:
			if(((offset + 1) / 2) == _gif_.gif.tag.NLOOP){
				_gif_.step = 0;
			}
			*buf = '\0';
			return 0;
			break;
		}
	}

	*buf = '\0';
	return 0;
}

static size_t _dbgFormatGifPktPacked(char* buf, size_t len, u_int reg, u_long datal, u_long datah){
	char work[256];

	switch(reg){
	case SCE_GS_PRIM:		// 0x00
	case SCE_GS_TEX0_1:		// 0x06
	case SCE_GS_TEX0_2:		// 0x07
	case SCE_GS_CLAMP_1:	// 0x08
	case SCE_GS_CLAMP_2:	// 0x09
		return _dbgFormatGifPktReg(buf, len, reg, datal);
		break;

	case SCE_GS_RGBAQ:		// 0x01
		{
			union{
				u_long d[2];
				struct{
					u_int r:8;
					u_int pad08:24;
					u_int g:8;
					u_int pad28:24;
					u_int b:8;
					u_int pad48:24;
					u_int a:8;
					u_int pad68:24;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "RGBAQ[%02x,%02x,%02x,%02x]", p.i.r, p.i.g, p.i.b, p.i.a);
		}
		break;

	case SCE_GS_ST:			// 0x02
		{
			union{
				u_long d[2];
				struct{
					float s;
					float t;
					float q;
					u_int pad60;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "ST[%f,%f,%f]", p.i.s, p.i.t, p.i.q);
		}
		break;

	case SCE_GS_UV:			// 0x03
		{
			union{
				u_long d[2];
				struct{
					int u:16;
					int pad10:16;
					int v:16;
					int pad30:16;
					u_int pad40;
					u_int pad60;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "UV[%04x,%04x]", p.i.u, p.i.v);
		}
		break;

	case SCE_GS_XYZF2:		// 0x04
		{
			union{
				u_long d[2];
				struct{
					int x:16;
					int pad10:16;
					int y:16;
					int pad30:16;
					u_int pad40:4;
					u_int z:24;
					u_int pad5c:4;
					u_int pad60:4;
					u_int f:8;
					u_int pad6c:3;
					u_int adc:1;
					u_int pad70:16;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "XYZF2[%04x,%04x,%06x,%02x,%d]", p.i.x, p.i.y, p.i.z, p.i.f, p.i.adc);
		}
		break;

	case SCE_GS_XYZ2:		// 0x05
		{
			union{
				u_long d[2];
				struct{
					int x:16;
					int pad10:16;
					int y:16;
					int pad30:16;
					u_int z:32;
					u_int pad60:15;
					u_int adc:1;
					u_int pad70:16;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "XYZF2[%04x,%04x,%08x,%d]", p.i.x, p.i.y, p.i.z, p.i.adc);
		}
		break;

	case SCE_GS_FOG:		// 0x0a
		{
			union{
				u_long d[2];
				struct{
					u_int pad00;
					u_int pad20;
					u_int pad40;
					u_int pad60:4;
					u_int f:8;
					u_int pad6c:20;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "FOG[%02x]", p.i.f);
		}
		break;

	case SCE_GS_XYZF3:		// 0x0c
		{
			union{
				u_long d[2];
				struct{
					int x:16;
					int pad10:16;
					int y:16;
					int pad30:16;
					u_int pad40:4;
					u_int z:24;
					u_int pad5c:4;
					u_int pad60:4;
					u_int f:8;
					u_int pad6c:20;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "XYZF3[%04x,%04x,%06x,%02x]", p.i.x, p.i.y, p.i.z, p.i.f);
		}
		break;

	case SCE_GS_XYZ3:		// 0x0d
		{
			union{
				u_long d[2];
				struct{
					int x:16;
					int pad10:16;
					int y:16;
					int pad30:16;
					u_int z:32;
					u_int pad60;
				}i;
			}const p = {{ datal, datah }};
			sprintf(work, "XYZ3[%04x,%04x,%08x]", p.i.x, p.i.y, p.i.z);
		}
		break;

	case 0x0e:				// A+D
		return _dbgFormatGifPktReg(buf, len, (u_int)(datah & 0xffU), datal);
		break;

	case 0x0f:				// nop
		sprintf(work, "NOP");
		break;

	case 0x0b:				// reserved
	default:
		sprintf(work, "reserved");
		break;
	}

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}

static size_t _dbgFormatGifPktReg(char* buf, size_t len, u_int reg, u_long data){
	char work[256];

	switch(reg){
	case SCE_GS_PRIM:		// 0x00
		{
			union{
				u_long d;
				sceGsPrim i;
			}const p = { data };
			sprintf(work, "PRIM[PRIM=%d,IIP=%d,TME=%d,FGE=%d,ABE=%d,AA1=%d,FST=%d,CTXT=%d,FIX=%d]", p.i.PRIM, p.i.IIP, p.i.TME, p.i.FGE, p.i.ABE, p.i.AA1, p.i.FST, p.i.CTXT, p.i.FIX);
		}
		break;

	case SCE_GS_RGBAQ:		// 0x01
		{
			union{
				u_long d;
				sceGsRgbaq i;
			}const p = { data };
			sprintf(work, "RGBAQ[%02x,%02x,%02x,%02x,%f]", p.i.R, p.i.G, p.i.B, p.i.A, p.i.Q);
		}
		break;

	case SCE_GS_ST:			// 0x02
		{
			union{
				u_long d;
				sceGsSt i;
			}const p = { data };
			sprintf(work, "ST[%f,%f]", p.i.S, p.i.T);
		}
		break;

	case SCE_GS_UV:			// 0x03
		{
			union{
				u_long d;
				sceGsUv i;
			}const p = { data };
			sprintf(work, "UV[%04x,%04x]", p.i.U, p.i.V);
		}
		break;

	case SCE_GS_XYZF2:		// 0x04
	case SCE_GS_XYZF3:		// 0x0c
		{
			union{
				u_long d;
				sceGsXyzf i;
			}const p = { data };
			sprintf(work, "XYZF%d[%04x,%04x,%06x,%02x]", (SCE_GS_XYZF2 == reg) ? 2 : 3, p.i.X, p.i.Y, p.i.Z, p.i.F);
		}
		break;

	case SCE_GS_XYZ2:		// 0x05
	case SCE_GS_XYZ3:		// 0x0d
		{
			union{
				u_long d;
				sceGsXyz i;
			}const p = { data };
			sprintf(work, "XYZ%d[%04x,%04x,%08x]", (SCE_GS_XYZ2 == reg) ? 2 : 3, p.i.X, p.i.Y, (int)p.i.Z);
		}
		break;

	case SCE_GS_FOG:		// 0x0a
		{
			union{
				u_long d;
				sceGsFog i;
			}const p = { data };
			sprintf(work, "FOG[%02x]", p.i.F);
		}
		break;

	case SCE_GS_XYOFFSET_1:	// 0x18
	case SCE_GS_XYOFFSET_2:	// 0x19
		{
			union{
				u_long d;
				sceGsXyoffset i;
			}const p = { data };
			sprintf(work, "XYOFFSET_%d[%04x,%04x]", 1 + (reg & 1), p.i.OFX, p.i.OFY);
		}
		break;

	case SCE_GS_PRMODECONT:	// 0x1a
		{
			union{
				u_long d;
				sceGsPrmodecont i;
			}const p = { data };
			sprintf(work, "PRMODECONT[%d]", p.i.AC);
		}
		break;

	case SCE_GS_PRMODE:		// 0x1b
		{
			union{
				u_long d;
				sceGsPrmode i;
			}const p = { data };
			sprintf(work, "PRMODE[IIP=%d,TME=%d,FGE=%d,ABE=%d,AA1=%d,FST=%d,CTXT=%d,FIX=%d]", p.i.IIP, p.i.TME, p.i.FGE, p.i.ABE, p.i.AA1, p.i.FST, p.i.CTXT, p.i.FIX);
		}
		break;

	case SCE_GS_TEX0_1:		// 0x06
	case SCE_GS_TEX0_2:		// 0x07
		{
			union{
				u_long d;
				sceGsTex0 i;
			}const p = { data };
			sprintf(work, "TEX0_%d[TBP0=%04x,TBW=%d,PSM=%02x,TW=%d,TH=%d,TCC=%d,TFX=%d,CBP=%04x,CPSM=%02x,CSM=%d,CSA=%d,CLD=%d]", 1 + (reg & 1), p.i.TBP0, p.i.TBW, p.i.PSM, p.i.TW, p.i.TH, p.i.TCC, p.i.TFX, p.i.CBP, p.i.CPSM, p.i.CSM, p.i.CSA, p.i.CLD);
		}
		break;

	case SCE_GS_TEX1_1:		// 0x14
	case SCE_GS_TEX1_2:		// 0x15
		{
			union{
				u_long d;
				sceGsTex1 i;
			}const p = { data };
			sprintf(work, "TEX1_%d[LCM=%d,MXL=%d,MMAG=%d,MMIN=%d,MTBA=%d,L=%d,K=%d]", 1 + (reg & 1), p.i.LCM, p.i.MXL, p.i.MMAG, p.i.MMIN, p.i.MTBA, p.i.L, p.i.K);
		}
		break;

	case SCE_GS_TEX2_1:		// 0x16
	case SCE_GS_TEX2_2:		// 0x17
		{
			union{
				u_long d;
				sceGsTex2 i;
			}const p = { data };
			sprintf(work, "TEX2_%d[PSM=%02x,CBP=%04x,CPSM=%02x,CSM=%d,CSA=%d,CLD=%d]", 1 + (reg & 1), p.i.PSM, p.i.CBP, p.i.CPSM, p.i.CSM, p.i.CSA, p.i.CLD);
		}
		break;

	case SCE_GS_TEXCLUT:	// 0x1c
		{
			union{
				u_long d;
				sceGsTexclut i;
			}const p = { data };
			sprintf(work, "TEXCLUT[CBW=%d,COU=%d,COV=%d]", p.i.CBW, p.i.COU, p.i.COV);
		}
		break;

	case SCE_GS_SCANMSK:	// 0x22
		{
			union{
				u_long d;
				sceGsScanmsk i;
			}const p = { data };
			sprintf(work, "SCANMSK[MSK=%d]", p.i.MSK);
		}
		break;

	case SCE_GS_MIPTBP1_1:	// 0x34
	case SCE_GS_MIPTBP1_2:	// 0x35
		{
			union{
				u_long d;
				sceGsMiptbp1 i;
			}const p = { data };
			sprintf(work, "MIPTBP1_%d[TBP1=%04x,TBW1=%d,TBP2=%04x,TBW2=%d.TBP3=%04x,TBW3=%d]", 1 + (reg & 1), p.i.TBP1, p.i.TBW1, p.i.TBP2, p.i.TBW2, p.i.TBP3, p.i.TBW3);
		}
		break;

	case SCE_GS_MIPTBP2_1:	// 0x36
	case SCE_GS_MIPTBP2_2:	// 0x37
		{
			union{
				u_long d;
				sceGsMiptbp2 i;
			}const p = { data };
			sprintf(work, "MIPTBP2_%d[TBP4=%04x,TBW4=%d,TBP5=%04x,TBW5=%d.TBP6=%04x,TBW6=%d]", 1 + (reg & 1), p.i.TBP4, p.i.TBW4, p.i.TBP5, p.i.TBW5, p.i.TBP6, p.i.TBW6);
		}
		break;

	case SCE_GS_CLAMP_1:	// 0x08
	case SCE_GS_CLAMP_2:	// 0x09
		{
			union{
				u_long d;
				sceGsClamp i;
			}const p = { data };
			sprintf(work, "CLAMP_%d[WMS=%d,WMT=%d,MINU=%03x,MAXU=%03x,MINV=%03x,MAXV=%03x]", 1 + (reg & 1), p.i.WMS, p.i.WMT, p.i.MINU, p.i.MAXU, p.i.MINV, p.i.MAXV);
		}
		break;

	case SCE_GS_TEXA:		// 0x3b
		{
			union{
				u_long d;
				sceGsTexa i;
			}const p = { data };
			sprintf(work, "TEXA[TA0=%d,AEM=%d,TA1=%d]", p.i.TA0, p.i.AEM, p.i.TA1);
		}
		break;

	case SCE_GS_FOGCOL:		// 0x3d
		{
			union{
				u_long d;
				sceGsFogcol i;
			}const p = { data };
			sprintf(work, "FOGCOL[FCR=%d,FCG=%d,FCB=%d]", p.i.FCR, p.i.FCG, p.i.FCB);
		}
		break;

	case SCE_GS_TEXFLUSH:	// 0x3f
		sprintf(work, "TEXFLUSH");
		break;


	case SCE_GS_SCISSOR_1:	// 0x40
	case SCE_GS_SCISSOR_2:	// 0x41
		{
			union{
				u_long d;
				sceGsScissor i;
			}const p = { data };
			sprintf(work, "SCISSOR_%d[SCAX0=%d,SCAX1=%d,SCAY0=%d,SCAY1=%d]", 1 + (reg & 1), p.i.SCAX0, p.i.SCAX1, p.i.SCAY0, p.i.SCAY1);
		}
		break;

	case SCE_GS_ALPHA_1:	// 0x42
	case SCE_GS_ALPHA_2:	// 0x43
		{
			union{
				u_long d;
				sceGsAlpha i;
			}const p = { data };
			sprintf(work, "ALPHA_%d[A=%d,B=%d,C=%d,D=%d,FIX=%02x]", 1 + (reg & 1), p.i.A, p.i.B, p.i.C, p.i.D, p.i.FIX);
		}
		break;

	case SCE_GS_DIMX:		// 0x44
		sprintf(work, "DIMX");
		break;

	case SCE_GS_DTHE:		// 0x45
		{
			union{
				u_long d;
				sceGsDthe i;
			}const p = { data };
			sprintf(work, "DTHE[%d]", p.i.DTHE);
		}
		break;

	case SCE_GS_COLCLAMP:	// 0x46
		{
			union{
				u_long d;
				sceGsColclamp i;
			}const p = { data };
			sprintf(work, "COLCLAMP[%d]", p.i.CLAMP);
		}
		break;

	case SCE_GS_TEST_1:		// 0x47
	case SCE_GS_TEST_2:		// 0x48
		{
			union{
				u_long d;
				sceGsTest i;
			}const p = { data };
			if(0 != p.i.ZTE){
				sprintf(work, "TEST_%d[ATE=%d,ATST=%d,AREF=%d,AFAIL=%d,DATE=%d,DATM=%d,ZTST=%d]", 1 + (reg & 1), p.i.ATE, p.i.ATST, p.i.AREF, p.i.AFAIL, p.i.DATE, p.i.DATM, p.i.ZTST);
			}else{
				sprintf(work, "TEST_%d[ATE=%d,ATST=%d,AREF=%d,AFAIL=%d,DATE=%d,DATM=%d,***ZTE=0***,ZTST=%d]", 1 + (reg & 1), p.i.ATE, p.i.ATST, p.i.AREF, p.i.AFAIL, p.i.DATE, p.i.DATM, p.i.ZTST);
			}
		}
		break;

	case SCE_GS_PABE:		// 0x49
		{
			union{
				u_long d;
				sceGsPabe i;
			}const p = { data };
			sprintf(work, "PABE[%d]", p.i.PABE);
		}
		break;

	case SCE_GS_FBA_1:		// 0x4a
	case SCE_GS_FBA_2:		// 0x4b
		{
			union{
				u_long d;
				sceGsFba i;
			}const p = { data };
			sprintf(work, "FBA_%d[%d]", 1 + (reg & 1), p.i.FBA);
		}
		break;


	case SCE_GS_FRAME_1:	// 0x4c
	case SCE_GS_FRAME_2:	// 0x4d
		{
			union{
				u_long d;
				sceGsFrame i;
			}const p = { data };
			sprintf(work, "FRAME_%d[FBP=%03x,FBW=%d,PSM=%x,FBMSK=%08x]", 1 + (reg & 1), p.i.FBP, p.i.FBW, p.i.PSM, (int)p.i.FBMSK);
		}
		break;

	case SCE_GS_ZBUF_1:		// 0x4e
	case SCE_GS_ZBUF_2:		// 0x4f
		{
			union{
				u_long d;
				sceGsZbuf i;
			}const p = { data };
			sprintf(work, "ZBUF_%d[ZBP=%03x,PSM=%x,ZMSK=%d]", 1 + (reg & 1), p.i.ZBP, p.i.PSM, p.i.ZMSK);
		}
		break;

	case SCE_GS_BITBLTBUF:	// 0x50
		{
			union{
				u_long d;
				sceGsBitbltbuf i;
			}const p = { data };
			sprintf(work, "BITBLTBUF[SBP=%04x,SBW=%d,SPSM=%02x,DBP=%04x,DBW=%d,DPSM=%02x]", p.i.SBP, p.i.SBW, p.i.SPSM, p.i.DBP, p.i.DBW, p.i.DPSM);
		}
		break;

	case SCE_GS_TRXPOS:		// 0x51
		{
			union{
				u_long d;
				sceGsTrxpos i;
			}const p = { data };
			sprintf(work, "TRXPOS[SSAX=%d,SSAY=%d,DSAX=%d,DSAY=%d,DIR=%d]", p.i.SSAX, p.i.SSAY, p.i.DSAX, p.i.DSAY, p.i.DIR);
		}
		break;

	case SCE_GS_TRXREG:		// 0x52
		{
			union{
				u_long d;
				sceGsTrxreg i;
			}const p = { data };
			sprintf(work, "TRXREG[RRW=%d,RRH=%d]", p.i.RRW, p.i.RRH);
		}
		break;

	case SCE_GS_TRXDIR:		// 0x53
		{
			union{
				u_long d;
				sceGsTrxdir i;
			}const p = { data };
			sprintf(work, "TRXDIR[%d]", p.i.XDR);
		}
		break;

	case SCE_GS_HWREG:		// 0x54
		sprintf(work, "HWREG");
		break;

	case SCE_GS_SIGNAL:		// 0x60
		{
			union{
				u_long d;
				sceGsSignal i;
			}const p = { data };
			sprintf(work, "SIGNAL[ID=%08x,IDMSK=%08x]", p.i.ID, p.i.IDMSK);
		}
		break;

	case SCE_GS_FINISH:		// 0x61
		sprintf(work, "FINISH");
		break;

	case SCE_GS_LABEL:		// 0x62
		{
			union{
				u_long d;
				sceGsLabel i;
			}const p = { data };
			sprintf(work, "LABEL[ID=%08x,IDMSK=%08x]", p.i.ID, p.i.IDMSK);
		}
		break;

	case SCE_GS_NOP:		// 0x7f
		sprintf(work, "NOP");
		break;

	default:
		sprintf(work, "unknown");
		break;
	}

	if(NULL != buf){
		strncpy(buf, work, len);
		buf[len - 1] = '\0';
	}

	return strlen(work);
}

void dbgGsSendPktVif1(u_long128 const* pkt){
	u_int addr;

	dbgDumpPacketDma(pkt, NULL, 0, s_dbgPktDumpLevel);

	if(7 == ((u_int)pkt >> 28)){
		addr = ((u_int)pkt & 0x00003fffU) | 0x80000000U;

	}else{
		FlushCache(0);
		addr = (u_int)pkt & 0x0fffffffU;
	}

	*D1_QWC = 0;
	*D1_TADR = addr;
	*D1_CHCR = D_CHCR_STR_M | D_CHCR_TTE_M | (1 << D_CHCR_MOD_O) | D_CHCR_DIR_M;
}

void dbgGsSendPktGif(u_long128 const* pkt){
	u_int addr;

	dbgDumpPacketDma(pkt, NULL, 1, s_dbgPktDumpLevel);

	if(7 == ((u_int)pkt >> 28)){
		addr = ((u_int)pkt & 0x00003fffU) | 0x80000000U;

	}else{
		FlushCache(0);
		addr = (u_int)pkt & 0x0fffffffU;
	}

	*D2_QWC = 0;
	*D2_TADR = addr;
	*D2_CHCR = D_CHCR_STR_M | (1 << D_CHCR_MOD_O);
}

void dbgGsSendPktGifN(u_long128 const* pkt, u_int qwc){

	u_int addr;

	dbgDumpPacketGif(pkt, pkt + qwc, s_dbgPktDumpLevel);

	if(7 == ((u_int)pkt >> 28)){
		addr = ((u_int)pkt & 0x00003fffU) | 0x80000000U;

	}else{
		FlushCache(0);
		addr = (u_int)pkt & 0x0fffffffU;
	}

	*D2_QWC = qwc;
	*D2_MADR = addr;
	*D2_CHCR = D_CHCR_STR_M;
}

/*[eof]*/
