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

// FIXME: NTSC/PAL�ŕύX���ׂ��l
#define REPEAT_GUARD  (30)
#define REAT_INTERVAL (4)

volatile u_long PadVSyncCounter; // VSYNC�J�E���g�p

// �R���g���[���p�̃��[�N
typedef struct tagCONTROLLER {
	u_short key;      // �L�[�̉����������(���_��)
	u_short oldkey;   // 1���[�v�O�̃L�[���(���_��)
	u_short down;     // �L�[�̉��������G�b�W(���_��)
	u_short up;       // �L�[�̉����グ�G�b�W(���_��)
	u_short rep;      // �L�[�̃��s�[�g(���_��)

	u_long ulLastTime; // �Ō�ɃL�[��]����������(VSYNC�J�E���g����)
	int repcount1[9];  // ���s�[�g�J�E���^1
	int repcount2[9];  // ���s�[�g�J�E���^2

	u_char pressure[16];  // �����������͂̋���

	u_char RawBuf[32];    // �R���g���[������̎�M�o�b�t�@
	u_char Id;            // �R���g���[�����[�hID
	int    State;         // �F���t�F�C�Y
} CONTROLLER;

static CONTROLLER g_Ctrl[2];

// ���s�[�g�̌n����`
static const u_short g_RepKeyTbl[9] = {
	SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright,  // �����L�[
	SCE_PADRup   ,   // [��]�{�^��
	SCE_PADRdown ,   // [�~]�{�^��
	SCE_PADRleft ,   // [��]�{�^��
	SCE_PADRright,   // [��]�{�^��
	SCE_PADL1,       // [L1]�{�^��
	SCE_PADL2,       // [L2]�{�^��
	SCE_PADR1,       // [R1]�{�^��
	SCE_PADR2        // [R2]�{�^��
};


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	PadGetKey													ref: pad.h
//
//	�{�^�����̍X�V(���[�v�擪��1��Ă�)
//
//	int cno : �R���g���[���ԍ�
//
//	�߂�l: u_long : �R���g���[�����(�������/�����n�ߏ��/�������/���s�[�g���)
u_long PadGetKey(int cno)
{
	// ���s�[�g�����o����L�[�̃��X�g
	int	i, t, w, state;
	CONTROLLER *pCtrl;

	pCtrl = &g_Ctrl[cno];
	t = PadVSyncCounter;
	w = (t - pCtrl->ulLastTime);
	pCtrl->ulLastTime = t;

	// �p�b�h�f�[�^�̎擾
	pCtrl->oldkey = pCtrl->key;  // �O��̃L�[�����������
	pCtrl->key    = 0;           // ����̃L�[���

	// �R���g���[���̏�ԃ`�F�b�N
	state = scePadGetState(cno, 0);
	pCtrl->State = state;           // ��Ԃ�ۑ�
	if(state==scePadStateDiscon) {
		// �R���g���[����������Ă����Ƃ�
		pCtrl->RawBuf[0] = 0x01;    // �o�b�t�@����
		pCtrl->Id        = 0x00;    // ID�͖���
	} else if(state==scePadStateStable || state==scePadStateFindCTP1) {
		// �R���g���[�����R�}���h�����s�\�ȏ��
		u_char id;

		// �R���g���[������̃f�[�^��ǂݍ���
		scePadRead(0, 0, pCtrl->RawBuf);
		if(pCtrl->RawBuf[0]!=0x00) {
			// �R���g���[���Ƃ̒ʐM�Ɏ��s���Ă����Ƃ�
			;
		} else {
			// �R���g���[���Ƃ̒ʐM����
			id = pCtrl->RawBuf[1];
			pCtrl->Id = id;

			// �R���g���[������̎�M�f�[�^����
			switch(id) {
				case 0x41:  // �W���R���g���[���̏ꍇ
				case 0x73:  // DUALSHOCK�̃A�i���O���[�h�̏ꍇ
					pCtrl->key = ~((pCtrl->RawBuf[2]<<8) | pCtrl->RawBuf[3]); // ���݂̃L�[���
					break;
			}
		}
	}

	// �L�[�̃T�[�r�X����(�ǂݎ�����f�[�^�����H)
	pCtrl->down = (pCtrl->oldkey ^ 0xFFFF) & pCtrl->key;    // �����n�߃G�b�a�̏���ݒ�
	pCtrl->up   = (pCtrl->key    ^ 0xFFFF) & pCtrl->oldkey; // �L�[�𗣂����G�b�a�̏���ݒ�

	// �{�^���̃��s�[�g
	pCtrl->rep = pCtrl->down;
	for(i=0; i<9; i++) {
		if((pCtrl->oldkey & g_RepKeyTbl[i]) && (pCtrl->key & g_RepKeyTbl[i])) {
			// �O��ƍ�����L�[������������Ԃ������Ƃ�
			if(pCtrl->repcount1[i]) {
				// �K�[�h�^�C�����Ԓ��̂Ƃ�
				pCtrl->repcount1[i] -= w;
				if(pCtrl->repcount1[i]<0) {
					pCtrl->repcount1[i] = 0;
				}
			}
			if(pCtrl->repcount1[i]==0) {
				// ���s�[�g��
				pCtrl->repcount2[i] -= w;
				if(pCtrl->repcount2[i]<=0) {
					pCtrl->rep |= pCtrl->key & g_RepKeyTbl[i];
					pCtrl->repcount2[i] += REAT_INTERVAL;	// ���s�[�g�^�C��
				}
			}
		} else {
			// �O��ƍ���ŃL�[��Ԃ��ς���Ă����Ƃ�
			pCtrl->repcount1[i] = REPEAT_GUARD; // ���s�[�g�܂ł̃K�[�h�^�C����ݒ�
			pCtrl->repcount2[i] = 0;            // 
		}
	}

	return (((u_long)pCtrl->up<<48) | ((u_long)pCtrl->down<<32) | ((u_long)pCtrl->key<<16) | ((u_long)pCtrl->rep));
}
