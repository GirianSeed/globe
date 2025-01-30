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

// USB�L�[�{�[�h���C�u�������n���h�����O�����ԃ}�V��

#include <eekernel.h>
#include <libusbkb.h>
#include <stdio.h>
#include <string.h>
#include "kbd.h"

// �t�F�C�Y�Ǘ��̂��߂̏�Ԃ��`
enum {
	KB_GETINFO,
	KB_GETINFO_SYNC,
	KB_NEW_LEDSTAT_SYNC,
	KB_NEW_LEDMODE_SYNC,
	KB_READ,
	KB_READSYNC
};

// �O���[�o���ϐ�
static USBKBINFO_t g_kbInfo;				// USB�L�[�{�[�h�̐ڑ����
static USBKBDATA_t g_kbData;				// USB�L�[�{�[�h���瓾�����
static int g_kbStat;						// default not connected
static int g_kbPhase;						// �t�F�C�Y�Ǘ�
static int g_kbLED;							// LED�Ǘ�

static int g_activeKb;						// �A�N�e�B�u�ȃL�[�{�[�h�ԍ�
                                            // �����Ȃ��邱�ƂɑΉ� ��܂�

static int g_ignore_count;					// �L�[�{�[�h���C�u�����ɗ��܂���
                                            // ���b�Z�[�W������ǂ݂��Ď̂Ă邽��

static int g_changeKb;						// �L�[�{�[�h��2������Ă�������
											// �A�N�e�B�u�łȂ��L�[�{�[�h����ł��ꂽ������
											// �A�N�e�B�u�ɂȂ������ɑ��M���Ȃ��ׂ̃t���O

static struct{
	int region;
	int kbType;
	int repeatWait;
	int repeatSpeed;
}KbdCnf;


// �L�[�{�[�h���C�u����������
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

	// USB�L�[�{�[�h���C�u�����̏�����
	res = sceUsbKbInit(&dmy);
	if(res==USBKB_NG) {
		// USB�L�[�{�[�h���C�u�����̏������Ɏ��s
		printf("Initialize error\n");
	}
	// LED�����ݒ�
	sceUsbKbSetLEDStatus(g_activeKb, g_kbLED);

	return(0);
}

// �L�[�{�[�h���C�u�����I��
void Kbd_Exit(void)
{
	sceUsbKbEnd();
}


// �L�[�{�[�h�̐ڑ��Ď�����уf�[�^�̈������܂ł��s���X�e�[�g�}�V���֐�
// �߂�l
//		0:  �V�K�f�[�^�Ȃ�
//		1:  �V�K�f�[�^����
//		-1: �G���[
int Kbd_Main(USBKBDATA_t *pKbdData)
{
	int ret, result;
	int i;

	switch(g_kbPhase) {
		case KB_GETINFO:

			// �񓯊��ŃL�[�{�[�h�ڑ������擾
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
				
				// �L�[�{�[�h�ڑ����̎擾�������������ǂ����|�[�����O
				ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
				//	printf("USBKB get info%d\n", ret);
				if(ret==USBKB_EXEC) {
					// �񓯊��֐����܂����s��
					break;
				}
				if(result!=USBKB_OK) {
					// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
					printf("Error%d : sceUsbKbSync\n", result);
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
			
				if( (!g_kbInfo.status[0]) && (!g_kbInfo.status[1]) )
				{
					// �L�[�{�[�h�����炸
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
			
				// �A�N�e�B�u�L�[�{�[�h���X�V
				{
					if( g_kbInfo.status[g_activeKb] )
					{
						// ���̂܂�
						new_kb_detected = 1;			// ��񔲂��Ă܂������ꂽ��
					}
					else if( g_kbInfo.status[1-g_activeKb] )
					{
						new_kb_detected = 1;
				
						// g_activeKb �؂�ւ�
						g_activeKb = 1-g_activeKb;
				
						printf( "g_activeKb = %d\n", g_activeKb );
					}
					else
					{
						// ���肦�Ȃ�
					}
				}
				
				// �L�[�{�[�h���V���ɍ����ꂽ
				if( new_kb_detected )
				{
					g_changeKb = 1;
					Kbd_SetConfig();
				
					// LED�̓_�����[�h�̐ݒ�
					//	printf("USBKB LED mode\n");
				
					ret = sceUsbKbSetLEDMode(g_activeKb, USBKB_LED_MODE_MANUAL);	// LED����̓}�j���A����
					if(ret!=USBKB_OK) {
						// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
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

			// LED�_�����[�h�̐ݒ肪�����������ǂ����|�[�����O
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// �񓯊��֐����܂����s��
				break;
			}
			if(result!=USBKB_OK) {
				// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
				printf("Error%d : sceUsbKbSync\n", result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}

			g_kbStat  = g_kbInfo.status[g_activeKb];

			// �񓯊���LED�X�e�[�^�X��ݒ�
			g_kbLED |= USBKB_LED_NUM_LOCK;
			ret = sceUsbKbSetLEDStatus(g_activeKb, USBKB_LED_NUM_LOCK);
			if(ret!=USBKB_OK) {
				// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
				printf("Error%d : sceUsbKbSetLEDStatus\n", ret);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}
			g_kbPhase = KB_NEW_LEDSTAT_SYNC;
			break;

		case KB_NEW_LEDSTAT_SYNC:

			// LED�X�e�[�^�X�������������ǂ����|�[�����O
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// �񓯊��֐����܂����s��
				break;
			}
			if(result!=USBKB_OK) {
				// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
				printf("Error%d : sceUsbKbSync\n", result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}
			g_kbPhase = KB_READ;
			break;

		case KB_READ:

			if(g_changeKb)
			{
				// �L�[�{�[�h
				KbdIgnoreSeveralMessage();
				g_changeKb = 0;
			}
			else
			{
				// �L�[�{�[�h�f�[�^�̃��[�h
				ret = sceUsbKbRead(g_activeKb, &g_kbData);
				if(ret!=USBKB_OK) {
					// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
					printf("Error%d : sceUsbKbRead\n", ret);
					g_kbPhase = KB_GETINFO;
					return(-1);
				}
				g_kbPhase = KB_READSYNC;
			}
			break;

		case KB_READSYNC:

			// �L�[�{�[�h�f�[�^�̃��[�h�������������ǂ����|�[�����O
			ret = sceUsbKbSync(USBKB_NO_WAIT, &result);
			if(ret==USBKB_EXEC) {
				// �񓯊��֐����܂����s��
				break;
			}
			if(result!=USBKB_OK) {
				// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
				printf("Error%d : sceUsbKbSync\n",result);
				g_kbPhase = KB_GETINFO;
				return(-1);
			}

			g_kbPhase = KB_READ;
			if(pKbdData) {
				g_kbData.led = g_kbLED;
				memcpy(pKbdData, &g_kbData, sizeof(USBKBDATA_t));
			}

			// ���܂��Ă������b�Z�[�W�𖳎�����
			if( g_ignore_count > 0 )
			{
				g_ignore_count --;
				if( g_ignore_count < 0 ){ g_ignore_count = 0; }
				return 0;
			}

			// �L�[�R�[�h���X�L��������LED����
			{
				u_char led;

				led = g_kbLED;
				for(i=0; i<g_kbData.len; i++) {
					if(g_kbData.keycode[i]==USBKEYC_SCROLL_LOCK) {
						// [ScrollLock]�L�[�������ꂽ�Ƃ�
						led ^= USBKB_LED_SCROLL_LOCK;
					} else if(g_kbData.keycode[i]==USBKEYC_NUM_LOCK) {
						// [NumLock]�L�[�������ꂽ�Ƃ�
						led ^= USBKB_LED_NUM_LOCK;
					} else if(g_kbData.keycode[i]==USBKEYC_CAPS_LOCK) {
						if( KbdCnf.region == 0 )// ���{����
						{
							// [CapsLock]�L�[�������ꂽ�Ƃ�
							if((g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL)) &&
							  !(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN))) {
								// [CTRL]�L�[���v���X�A[SHIFT],[ALT],[Windows]�L�[�������[�X����Ă����Ƃ�
	
								// �J�i���b�N��Ԃ𔽓]������
								led ^= USBKB_LED_KANA;
							} else if(//(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) &&	// SHIFT�L�[�������Ă�OK?
									  !(g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN))) {
	
								// [SHIFT]�L�[���v���X�A[CTRL],[ALT],[Windows]�L�[�������[�X����Ă����Ƃ�
								// CapsLock��Ԃ𔽓]������
								led ^= USBKB_LED_CAPS_LOCK;
							}
						}
						else
						{
							if(//(g_kbData.mkey & (USBKB_MKEY_L_SHIFT | USBKB_MKEY_R_SHIFT)) &&	// SHIFT�L�[�������Ă�OK?
									  !(g_kbData.mkey & (USBKB_MKEY_L_CTRL | USBKB_MKEY_R_CTRL | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT | USBKB_MKEY_L_WIN | USBKB_MKEY_R_WIN | USBKB_MKEY_L_ALT | USBKB_MKEY_R_ALT))) {
								// [SHIFT]�L�[���v���X�A[CTRL],[ALT],[Windows]�L�[�������[�X����Ă����Ƃ�
								// CapsLock��Ԃ𔽓]������
								led ^= USBKB_LED_CAPS_LOCK;
							}
						}
					}
				}
				if(led!=g_kbLED) {
					g_kbLED = led;
					ret = sceUsbKbSetLEDStatus(g_activeKb, led);
					if(ret!=USBKB_OK) {
						// USB�L�[�{�[�h���C�u��������G���[���Ԃ��ꂽ
						printf("Error%d : sceUsbKbSetLEDStatus\n", ret);
						g_kbPhase = KB_GETINFO;
						return(-1);
					}
					g_kbPhase = KB_NEW_LEDSTAT_SYNC;
				}
			}
			return(1);		// g_kbData�L��
	}
	return(0);			// sync ��
}

// NumLock��Ԃ��H
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
	// 10 ��� sceUsbKbRead �� ��������
	// UsbKb�̃L���[�ɂ��܂������b�Z�[�W�𖳎����邽��
	g_ignore_count = 10;
}


// KbdMain �𒷂��ԌĂ΂Ȃ��Ȃ��Ă��ǂ��^�C�~���O���ǂ������`�F�b�N
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
	sceUsbKbSetCodeType(g_activeKb, USBKB_CODETYPE_RAW);		// ASCII�R�[�h�ɕϊ����Ȃ�
	sceUsbKbSetArrangement(g_activeKb, KbdCnf.kbType );	// �z��ݒ�
}

