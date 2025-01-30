/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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

#ifndef __LIBFEP_H__
#define __LIBFEP_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <libusbkb.h>

#include "mtfifo.h"
#include <shareddef.h>

#define	IS_SJIS_KANJI(_c)		((((_c)>=0x81) && ((_c)<=0x9F)) || (((_c)>=0xE0) && ((_c)<=0xFC)))

#define HENKAN_SIZE			(256)		// �ϊ����̕�����ő�l
#define KAKUTEI_SIZE		(256)		// �m�蕶����    �ő�l
#define CANDLIST_SIZE		(1024)		// ���ꗗ������ő�l

#define CONV_ON				(1)			// �ϊ���
#define CONV_OFF			(0)			// �ϊ����łȂ�


#define FEP_EDIT_DELETE		(0)			// �����폜
#define FEP_EDIT_ADD		(1)			// �����ǉ�
#define FEP_EDIT_INSERT		(2)			// �����}��

#define FEP_EDIT_NG			(0)			// �����ǉ�NG
#define FEP_EDIT_OK			(1)			// �����ǉ�OK
#define FEP_EDIT_OVER		(2)			// �w�肵���o�C�g�����I�[�o�[����


// ���̓��[�h
typedef enum
{
	// FEP ON
	FEPON_HIRA = 0,			// �Ђ炪��
	FEPON_HAN_KATAKANA,		// �J�^�J�i(���p)
	FEPON_ZEN_KATAKANA,		// �J�^�J�i(�S�p)
	FEPON_HAN_ALPHABET,		// ���ϊ�  (���p)
	FEPON_ZEN_ALPHABET,		// ���ϊ�  (�S�p)

	// FEP OFF
	FEPOFF,

	#if 1//SKB_EX FixMode
	FEPON_HIRA_FIX        ,			// �Ђ炪��
	FEPON_HAN_KATAKANA_FIX,		// �J�^�J�i(���p)
	FEPON_ZEN_KATAKANA_FIX,		// �J�^�J�i(�S�p)
	FEPON_HAN_ALPHABET_FIX,		// ���ϊ�  (���p)
	FEPON_ZEN_ALPHABET_FIX,		// ���ϊ�  (�S�p)
	#endif

	FEPMODE_MAX,

} fepAtokInputMode_m;


// �L�[�o�C���h�e�[�u���̒�`�Ɏg����l�̒�`
enum {
	RAW_CODE          = 0x4000, // �o�̓L���[�ɐ��R�[�h��Ԃ��Ƃ��Ɏg���t���O

	EX_ATOK_EDIT_NULL = 0x8000, // �g���p�R�[�h���A0x8000�ȏ�Ƃ��Ē�`
	                            // EX_ATOK_�Ŏn�܂�R�}���h�́A����sceAtokEditConv()�ɓn�����R�}���h�ł͂Ȃ�
	                            // �������̃v���~�e�B�u��g�ݍ��킹�������R�}���h�ł��B�����R�}���h�̔ԍ��̒l
	                            // �͈̔͂�0x8000�ȏ�ɂ��邱�ƂŁAbit 15���g���ċ�ʂł���悤�ɂ��Ă��܂��B

	// sceAtokEditConv()�֐�1��̌Ăяo���Ŏ����ł��Ȃ��@�\�́A
	// �K���Ȗ��O�����ēK�X�������������܂��B
	EX_ATOK_EDIT_KANJIOFF,      // �������̓��[�hOFF
	EX_ATOK_EDIT_KANALOCK,      // �J�i���b�N�؂�ւ�

	// ���͕����̐؂�ւ�
	EX_ATOK_DSPHIRAGANA,        // �Ђ炪�ȓ���
	EX_ATOK_DSPZENKANA,         // �S�p�J�^�J�i����
	EX_ATOK_DSPZENRAW,          // �S�p���ϊ�����
	EX_ATOK_DSPHANKANA,         // ���p�J�^�J�i����
	EX_ATOK_DSPHANRAW,          // ���p���ϊ�����

	EX_ATOK_DSPSWEISU,          // �p���؂�ւ�
	EX_ATOK_DSPSWKANA,          // ���Ȑ؂�ւ�
	                            // NOTICE: ���p�J�i�ɂȂ�\������

	// �Œ���̓��[�h�̐ݒ�
	EX_ATOK_FIXOFF,             // �Œ���̓��[�hOFF
	EX_ATOK_FIXHIRAGANA,        // �Œ���͂Ђ炪��
	EX_ATOK_FIXZENKANA,         // �Œ���͑S�p�J�^�J�i
	EX_ATOK_FIXZENRAW,          // �Œ���͑S�p���ϊ�
	EX_ATOK_FIXHANKANA,         // �Œ���͔��p�J�^�J�i
	EX_ATOK_FIXHANRAW,          // �Œ���͔��p���ϊ�

	// 
	EX_ATOK_FIX_ZENHIRA_HANRAW,          // �Œ���͂Ђ炪��<-->�Œ���͔��p���ϊ�
	EX_ATOK_FIX_ZENHIRA_ZENRAW,          // �Œ���͂Ђ炪��<-->�Œ���͑S�p���ϊ�
	EX_ATOK_FIX_ZENHIRA_ZENKANA_HANKANA, // �Œ���͂Ђ炪��-->�Œ���͑S�p�J�^�J�i-->�Œ���͔��p�J�^�J�i

	EX_ATOK_CHANGE_DSPKANA,     // �Ђ炪��-->�S�p�J�^�J�i-->���p�J�^�J�i
	EX_ATOK_CHANGE_DSPRAW,      // �S�p���ϊ�-->���p���ϊ�

	// ��⃊�X�g�̑I��
	EX_ATOK_EDIT_CANDNEXT,      // �����
	EX_ATOK_EDIT_CANDPREV,      // �O���
	EX_ATOK_EDIT_CANDHEAD,      // ���擪
	EX_ATOK_EDIT_CANDTAIL,      // ��▖��
	EX_ATOK_EDIT_CANDGRPNEXT,   // �����Q
	EX_ATOK_EDIT_CANDGRPPREV,   // �O���Q

	EX_ATOK_EDIT_CLSEND_CAND,   // �Ō���
	EX_ATOK_EDIT_CLSTOP_CAND,   // �擪���

	// ���ߒP�ʂ̃J�[�\���ړ�
	EX_ATOK_EDIT_REVCLSLEFT,    // ���ߍ��z��
	EX_ATOK_EDIT_REVCLSRIGHT,   // ���߉E�z��
	EX_ATOK_EDIT_CLSLEFT,       // ���ߍ�
	EX_ATOK_EDIT_CLSRIGHT,      // ���߉E
	EX_ATOK_EDIT_CLSTOP,        // ���ߐ擪
	EX_ATOK_EDIT_CLSEND,        // ���ߖ���
//	EX_ATOK_EDIT_FOCUSLEFTEX,   // �t�H�[�J�X��
//	EX_ATOK_EDIT_FOCUSRIGHTEX,  // �t�H�[�J�X�E

	EX_ATOK_EDIT_POSTRAWHALF,   // ���p�p���ւ̕�����ϊ�

	EX_ATOK_EDIT_KAKUTEISPACE,    // �m���X�y�[�X�}��
	EX_ATOK_EDIT_KAKUTEIHANSPACE, // �m��㔼�p�X�y�[�X�}��
	EX_ATOK_EDIT_CURSTAY,         // �J�[�\���ړ������i�_�~�[�j

	EX_ATOK_EDIT_CAND1,         // ���ꗗ�\���̂Ƃ��A[0]�`[9]�̃L�[���g����
	EX_ATOK_EDIT_CAND2,         // �_�C���N�g�Ɍ���I�������ꍇ�̓����`
	EX_ATOK_EDIT_CAND3,         // 
	EX_ATOK_EDIT_CAND4,         // 
	EX_ATOK_EDIT_CAND5,         // 
	EX_ATOK_EDIT_CAND6,         // 
	EX_ATOK_EDIT_CAND7,         // 
	EX_ATOK_EDIT_CAND8,         // 
	EX_ATOK_EDIT_CAND9,         // 
	EX_ATOK_EDIT_CAND0,         // 

};



// �V�t�g��Ԃ��w�肷�邽�߂ɃL�[�R�[�h�ɕt������t���O�̒l�ł��B
#define SHIFT (0x10000000U)
#define CTRL  (0x20000000U)
#define ALT   (0x40000000U)

// �ЂƂЂƂ̃L�[�X�g���[�N�ɑ΂��铮����`����e�[�u���̍\���v�f�ł��B
typedef struct {
	u_long  code;    // �L�[�R�[�h
	u_short func[6]; //  0: SCE_ATOK_ISTATE_BEFOREINPUT   ������
	                 //  1: SCE_ATOK_ISTATE_BEFORECONVERT �ϊ��O
	                 //  2: SCE_ATOK_ISTATE_CONVERTING    �ϊ���
	                 //     SCE_ATOK_ISTATE_CANDEMPTY     ���؂�(���Ȃ�)
	                 //  3: SCE_ATOK_ISTATE_POSTCONVKANA  ���ꗗ
	                 //  4: SCE_ATOK_ISTATE_MOVECLAUSEGAP ���ߋ�؂蒼��
	                 //  5: SCE_ATOK_ISTATE_POSTCONVKANA  ��ϊ�(�J�^�J�i)
	                 //     SCE_ATOK_ISTATE_POSTCONVHALF  ��ϊ�(���p)
	                 //     SCE_ATOK_ISTATE_POSTCONVRAW   ��ϊ�(���ϊ�)
} FEP_KEYBIND;


// �ϊ����ꂽ������
typedef struct {
	u_int   Char;     // �ϊ����ݕ����R�[�h UCS4(UCS2)
	u_char  Flag;     // �t���O
	                  //  bit 0: 
	                  //  bit 1: 
	                  //  bit 2: 
	                  //  bit 3: 
	                  //  bit 4: 
	                  //  bit 5: 
	                  //  bit 6: 
	                  //  bit 7: 
	u_short RawCode;  // ���EUSB�L�[�R�[�h
} COOKED_CHAR;


// FEP
typedef struct
{
	u_char ucUseAtok;  // ATOK
	                   //  0: �g��Ȃ�
	                   //  1: �g��

	char   *pAtokErx;  // ERX�p�o�b�t�@

	int iInsertMode;   // �}�����[�h
	                   //  0: INSERT
	                   //  1: ADD

	int nCaretPos;     // �L�����b�g�̑��݂���o�C�g�ʒu
	int nTextLen;      // ���͍ςݕ�����̃o�C�g��

//	int szText[2048];       // �m�蕶����
	int szInsertText[2048]; // �}��������

	int mode;               // sceAtokGetKanjiState()�̒l
	                        // ATOK��ON/OFF
	int state;              // sceAtokGetInputState()�̒l
	                        // ATOK�G���W���̕������͏��
	int stateCandLeftEnd;   // sceAtokGetInputState()�̒l
	                        // ATOK�G���W���̕������͏��
	                        // �擪(����)���߂ɃW�����v�����ꗗ
	int led;          // �񓯊�ATOK���C�����Ǘ����Ă��邩�ȃ��b�N���
	int KanDspType;   // ATOK�G���W���ɐݒ肵�����͕�����

	int Keybind;      // �L�[�o�C���h�̐ݒ�
	                  //  0: ATOK��
	                  //  1: IME��
	                  //  2: VJE��

	int InitialInput; // �������̓��[�h
	                  //  0: ���[�}������
	                  //  1: ���ȓ���

	int SpaceInput;   // �X�y�[�X���̓��[�h
	                  //  0: ��ɔ��p
	                  //  1: ��ɑS�p
	                  //  2: ���̓��[�h�ɂ��

	int TenkeyInput;  // �e���L�[���̓��[�h
	                  //  0: ��ɔ��p
	                  //  1: ��ɑS�p
	                  //  2: ���̓��[�h�ɂ��

	int Font1, Font2; // �t�H���g�ݒ�
	FIFO qKeyInput;            // �L�[���̓L���[
	u_int auiKinQueueBuf[512]; // �L�[���̓L���[�̃o�b�t�@

	FIFO qFepOuput;            // ATOK�o�̓L���[
	u_int qOutBuf[512];        // ATOK�o�̓L���[�̃o�b�t�@

	int usePadChangeMode; // �p�b�h���g���ă��[�h���ς�������ǂ����̃t���O


	int FocusTop;      // �J�[�\��������o�C�g�ʒu
	int FocusCls;      // �t�H�[�J�X�����镶�߂̔ԍ�
	int nClusters;     // �ϊ���������̕��ߐ�

	int nCandidates;   // �ϊ���␔
	int nCandOffset;   // ���݂̌��ꗗ�̕\���J�n�I�t�Z�b�g
	int nCurrentCand;  // ���݂̕ϊ����J�[�\���ʒu

	char szConv[HENKAN_SIZE];           // �ϊ���������o�b�t�@
	char CandBuf[CANDLIST_SIZE];        // ��⃊�X�g�o�b�t�@
	char aszClusters[110][HENKAN_SIZE]; // �ϊ����̕��߃o�b�t�@

	int  cntF10;      // F10�p��ԃJ�E���^
	int  cntFix;      // ���ϊ�
	int  cntFixShift; // ���ϊ�+SHIFT
	int  cntFixCtrl;  // ���ϊ�+CTRL

	int  nIsConv;     // �ϊ�����
	                  //  0: �ϊ����łȂ�
	                  //  1: �ϊ���
} fep_t;

/*
typedef struct
{
	int		iRegion;				// ���[�W����
	int		iKeyBindType;			// �L�[�o�C���h�^�C�v
	int		iKeybdType;				// �L�[�{�[�h�^�C�v
	u_int	*puiUCS4;				// ������ւ̃|�C���^
	u_int	uiMaxTextLen;			// �ő啶����

	// �A�v������
	void (*atokON)(void);						// ATOK ON
	void (*atokOFF)(sceAtokContext *pAtok);		// ATOK OFF
	void (*ok)(void);							// ok
	void (*cancel)(void);						// �L�����Z��
	void (*changeMode)(int mode);				// ���̓��[�h�ύX
	int  (*changeText)(int changeType, const u_int *szChar, u_short ctrlCode);	// �����`�F�b�N
	u_char (*showHideSoftKB)(void);				// �\�t�g�E�F�A�L�[�{�[�h
												// 0: ��\��
												// 1: �\��
	void (*closeSoftKB)(void);					// �\�t�g�E�F�A�L�[�{�[�h����
	void (*openSoftKB)(void);					// �\�t�g�E�F�A�L�[�{�[�h�J��

	// �V�X�e��
	void (*dprintf)(const char *msg, ...);

	// ��
	void (*se)(int soundNo);

	// �������Ǘ�
	void* (*malign)(size_t align, size_t size);		// = memalign
	void  (*mfree)(void *p);						// = free

	// �d���Ǘ�
	void (*powerLock)(void);
	void (*powerUnlock)(void);

	int (*getStrWidthSJIS)(const char *sjis, float fontratio);		// �߂�l : 16�{���ꂽ�l

	// �`��֘A
	int  (*readTex)(textureInfo_t *pTex, const char *filename);		// �e�N�X�`���ǂݍ���
	void (*deleteTex)(textureInfo_t *pTex);							// �e�N�X�`���j��
	void (*loadTex)(sceVif1Packet *pk, textureInfo_t *pTex);		// �e�N�X�`�����[�h
	void (*drawTex)(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex);	// �e�N�X�`���`��

} fepImport_t;
*/




int  Fep_Init(gImport_t *pImport,
			 int iUseAtok, int iPriority,
			 const char *pszSysDic, const char *pszUserDic, const char *pszSysconfDir, const char *pszConfig);
int  Fep_Exit(void);
int  Fep_Run(void);
void Fep_PostSoftKey(const u_int *pUcs4, u_char Flag, u_short ctrlCode);
void Fep_PostKeys(USBKBDATA_t *pData);

void Fep_GetConvertChars(char *sjis);
int Fep_GetClusterNum(void);
int Fep_GetNowCluster(void);
char *Fep_GetClusterChars(int index);


u_int Fep_GetCandListMax(void);
u_int Fep_GetCandListOffset(void);
u_int Fep_GetCurrentCand(void);
u_int Fep_GetCandListMaxWidth(float mainChars, float subChars);
char *Fep_GetCandStart(void);
char *Fep_GetCandListItem(char *pStartPos, char *pItem, char *pSubItem);
const int Fep_CandListNum(void);
void Fep_SetText(const u_int *pUCS4);

void Fep_SetKakuteiPart(void);
void Fep_SetKakuteiAll(void);
int Fep_GetNowAtokInput(void);
int Fep_GetNowConv(void);
void Fep_SetKanjiCancel(void);
void Fep_ConvStrings(char *str_sjis);


u_int Fep_ShowCandList(void);
u_int Fep_Changing(void);
int  Fep_AtokGetInputState(void);
int  Fep_GetCaretPos(void);
int  Fep_IsIdle(void);
const u_int *Fep_GetInsertChars(void);
int  Fep_SetInputMode(fepAtokInputMode_m mode);
int  Fep_Cmd(sceAtokContext *pAtok, u_int f);
void Fep_SetData2Fifo(void);


#if 1//SKB_EX Insert 
void Fep_SetInsertMode(int Mode);  //  �����̑}��/�㏑���[�h��ݒ肷��
int  Fep_GetInsertMode(void);      //  �����̑}��/�㏑���[�h���l������
#endif

#if 1//SKB_EX Power
int  Fep_GetAtokSemaID(void);      // Atok���쎑���̃Z�}�t�H�h�c�擾
#endif

#if 1//SKB_EX_Sound 
void Fep_se_play(int sound_No);
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__LIBFEP_H__
