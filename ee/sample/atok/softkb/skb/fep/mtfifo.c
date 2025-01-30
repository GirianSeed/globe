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

//=====================================================================================
// �X���b�h������FIFO�p�C�v
//
// ���̃��W���[���́A�X���b�h�Z�[�t�ȒP����FIFO�p�C�v�̎����ł��B
// �����O�o�b�t�@��1�̃Z�}�t�H�ɂ���ē������䂵�܂��B
//
// �o�b�t�@���t��/�G���v�e�B�̂Ƃ��A
// ���M���܂��͎�M���̂ǂ��炪�X���b�h�҂���Ԃɓ��邩��2��ނ̎g����������܂��B
//               �Z�}�t�H�҂�   ����
// FifoPostData()    �Ȃ�      FIFO�Ƀf�[�^��u��
//                             �o�b�t�@�󂫗e�ʂ�����Ȃ������ꍇ�̓G���[�I��
// FifoRecvData()    ����      FIFO����f�[�^�𓾂�
//                             �w��o�C�g�������f�[�^��������܂ŃE�F�C�g
// FifoSendData()    ����      FIFO�Ƀf�[�^��u��
//                             �w��o�C�g�������f�[�^���u����悤�ɂȂ�܂ŃE�F�C�g
// FifoPollData()    �Ȃ�      FIFO����f�[�^�𓾂�
//                             �o�b�t�@���Ɏw��o�C�g�������f�[�^���Ȃ������ꍇ�G���[�I��
//=====================================================================================
//	Send��             FIFO              Recv��
//
//	pData           +----------+
//	                | nQueHead |
//	                |          |
//	                +----------+
//	                | nInBytes |->Recv��
//	                |          |
//	                +----------+ 
//	       Send�� ->|          |
//	                |          |
//	                |          |
//	pData+nBufSize	+----------+
//
//	Send��Wait������FIFO�ɑ��荞�߂�����́AnBufsize-nInByte >= ���M�v���T�C�Y
//	Recv��Wait������FIFO�ɑ��荞�߂�����́AnInByte >= ��M�v���T�C�Y



#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include "mtfifo.h"


static void CheckWaitRequest(FIFO *pFifo);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FifoCreate												ref: mtfifo.h
//
//	�}���`�X���b�hFIFO�L���[�̍쐬
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//	void *pBuf  : �L���[�o�b�t�@�̃|�C���^
//	int nBufSize : �L���[�o�b�t�@�̃T�C�Y
//
//	�߂�l : int : 0:����/-1:���s
int FifoCreate(FIFO *pFifo, void *pBuf, int nBufSize)
{
	struct SemaParam sp;
	// FIFO�L���[�̂��߂̃Z�}�t�H���쐬
	sp.maxCount  = 1;							// �Z�}�t�H�ő�l
	sp.initCount = 1;							// �Z�}�t�H�����l
	sp.option    = 0;							// �Z�}�t�H�t�����
	pFifo->FifoSema   = CreateSema(&sp);		// MFIFO�̔r���A�N�Z�X�p�̃Z�}�t�H����
	
	if(pFifo->FifoSema<0)
	{
		// �Z�}�t�H�����Ɏ��s�����̂ŃG���[�I��
		return(pFifo->FifoSema);
	}
	WaitSema(pFifo->FifoSema);

	pFifo->pData    = pBuf;						// �L���[�o�b�t�@�̃A�h���X��ݒ�
	pFifo->nBufSize = nBufSize;					// �L���[�ɓo�^�\�ȃG�������g�̍ő��
	pFifo->nQueHead = 0;						// �擪�C���f�N�X�ԍ���������
	pFifo->nInBytes = 0;						// ���݃L���[���ɂ���G�������g��
	pFifo->nSendReqSize = 0;					// Recive����Wait�����̏����̏����l
	pFifo->nRecvReqSize = 0;					// Send����Wait�����̏����̏����l
	
	sp.initCount = 0;
	pFifo->SendSema = CreateSema(&sp);
	if(pFifo->SendSema<0)
	{
		DeleteSema(pFifo->FifoSema);
		return(pFifo->SendSema);
	}
	pFifo->RecvSema = CreateSema(&sp);
	if(pFifo->RecvSema < 0)
	{
		DeleteSema(pFifo->FifoSema);
		DeleteSema(pFifo->SendSema);
		return(pFifo->RecvSema);
	}
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoDelete												ref: mtfifo.h
//
//	�}���`�X���b�hFIFO�L���[�����
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
void FifoDelete(FIFO *pFifo)
{
	DeleteSema(pFifo->FifoSema);						// �Z�}�t�H���
	DeleteSema(pFifo->SendSema);
	DeleteSema(pFifo->RecvSema);
	return;
}


//============================================================================
//	FifoFlush												ref: mtfifo.h
//
//	FIFO�p�C�v���t���b�V������
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
void FifoFlush(FIFO *pFifo)
{
	int ret=-1;

	ret = WaitSema(pFifo->FifoSema);
	if(ret < 0)
	{
		// MFIFO������������ԂŌĂяo���ꂽ�B
		return;		
	}
	pFifo->nQueHead = 0;						// �擪�C���f�N�X�ԍ���������
	pFifo->nInBytes = 0;						// ���݃L���[���ɂ���G�������g����������
	pFifo->nSendReqSize = 0;
	pFifo->nRecvReqSize = 0;

	SignalSema(pFifo->FifoSema);
	return;
}


//============================================================================
//	FifoGetBytes											ref: mtfifo.h
//
//	FIFO�L���[���ɂ��܂��Ă���f�[�^�o�C�g��
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//
//	�߂�l : u_int : �L���[���̗v�f��
u_int FifoGetBytes(FIFO *pFifo)
{
	u_int r;
	WaitSema(pFifo->FifoSema);
	r = pFifo->nInBytes;
	SignalSema(pFifo->FifoSema);
	return r;					// �L���[���̗v�f����߂�l�Ƃ��ĕԂ�
}


//============================================================================
//	FifoGetBytes											ref: mtfifo.h
//
//	FIFO�L���[�ɒl��ǉ�
//	�Z�}�t�H�҂��Ȃ��BFifoRecvData()�ƃy�A�Ŏg�p
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//	const void *pData : FIFO�L���[�ɓn�������f�[�^�̃|�C���^
//	int nSize : FIFO�L���[�ɓn�������f�[�^�̃T�C�Y
//
//	�߂�l : int : 0:����/1:�G���[(�L���[����ꂻ��)
int FifoPostData(FIFO *pFifo, const void *pData, int nSize)
{
	int t, m, n;

	if(nSize == 0)
	{
		return(0);
	}

	WaitSema(pFifo->FifoSema);

	m = pFifo->nBufSize;						// �ő�v�f���𓾂�
	n = pFifo->nInBytes;						// ����FIFO�p�C�v���ɂ���f�[�^�o�C�g���𓾂�
	if((m - n) < nSize)
	{
		// �L���[�̋󂫗e�ʂ����傫���Ƃ��A�G���[�I��
		SignalSema(pFifo->FifoSema);
		return(1);
	}

	t = pFifo->nQueHead + n;					// ���[�C���f�N�X�𓾂�
	if(t > m)
	{
		t -= m;
	}

	// ���b�v�A���E���h���Ȃ��ŃR�s�[�ł���o�C�g�����v�Z
	n = m - t;
	if(nSize <= n)
	{
		// ���b�v�A���E���h�Ȃ�
		memcpy((char*)&pFifo->pData[t], pData, (size_t)nSize);
	}
	else
	{
		// ���b�v�A���E���h����
		memcpy((char*)&pFifo->pData[t], pData, (size_t)n);
		memcpy((char*)pFifo->pData, &((const char *)pData)[n], (size_t)(nSize - n));
	}
	pFifo->nInBytes += nSize;					// FIFO�p�C�v���̗v�f���𑝂₷
	
	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);

	return(0);
}


//============================================================================
//	FifoRecvData											ref: mtfifo.h
//
//	�L���[�l����l���擾
//	�Z�}�t�H�҂�����BFifoPostData()�ƃy�A�Ŏg�p
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//	const void *pData : FIFO�L���[�ɓn�������f�[�^�̃|�C���^
//	int nSize : FIFO�L���[�ɓn�������f�[�^�̃T�C�Y
//	int fPeek : 
//
//	�߂�l : u_int : 0:����/1:�G���[(�L���[����ꂻ��)
u_int FifoRecvData(FIFO *pFifo, void *pData, int nSize, int fPeek)
{
	int h, m, n;
	int bWait = FALSE;

	if(nSize == 0)
	{
		return(0);
	}
	
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;		// �ő�v�f���𓾂�
	n = pFifo->nInBytes;

	if(nSize > m)
	{
		// �L���[�̍ő�e�ʂ����傫���Ƃ��A�G���[�I��
		SignalSema(pFifo->FifoSema);
		return(1);
	}

	// FIFO�L���[���Ƀf�[�^�����߂���܂Ń��[�v
	if( n < nSize)
	{
		pFifo->nRecvReqSize = nSize;
		bWait = TRUE;
	}
	SignalSema(pFifo->FifoSema);

	if(bWait==TRUE)
	{
	  WaitSema(pFifo->RecvSema);
	}
	WaitSema(pFifo->FifoSema);

	// ���b�v�A���E���h���Ȃ��ŃR�s�[�ł���o�C�g�����v�Z
	h = pFifo->nQueHead;						// �擪�C���f�N�X�𓾂�
	n = m - h;
	if(nSize <= n)
	{
		// ���b�v�A���E���h�Ȃ�
		memcpy(pData, (char*)&pFifo->pData[h], (size_t)nSize);
		if(!fPeek)
		{
			pFifo->nQueHead = (h + nSize) % m;
			pFifo->nInBytes -= nSize;			// FIFO�p�C�v���̗v�f�������炷
		}
	}
	else
	{
		// ���b�v�A���E���h����
		memcpy(pData,  (char*)&pFifo->pData[h], (size_t)n);
		memcpy(&((char *)pData)[n], (char*)pFifo->pData, (size_t)(nSize - n));
		if(!fPeek)
		{
			pFifo->nQueHead = nSize - n;
			pFifo->nInBytes -= nSize;			// FIFO�p�C�v���̗v�f�������炷
		}
	}

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoSendData											ref: mtfifo.h
//
//	FIFO�L���[�ɒl��ǉ�
//	�Z�}�t�H�҂�����BFifoPollData()�ƃy�A�Ŏg�p
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//	const void *pData : FIFO�L���[�ɓn�������f�[�^�̃|�C���^
//	int nSize : FIFO�L���[�ɓn�������f�[�^�̃T�C�Y
//
//	�߂�l : int : 0:����/1:�G���[(�L���[����ꂻ��)
int FifoSendData(FIFO *pFifo, const void *pData, int nSize)
{
	int t, m ,n;
	int bWait = FALSE;

	if(nSize == 0)
	{
		return(0);
	}
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;

	if( m < nSize)
	{
		// �L���[�̍ő�e�ʂ����傫���Ƃ��A�G���[�I��
		SignalSema(pFifo->FifoSema);
		return(1);
	}
	n = pFifo->nInBytes;

	// FIFO�L���[���Ƀf�[�^�����܂��Ă��Ȃ��ꍇ�́AWAIT����B
	if(( m  - n) < nSize)
	{
		pFifo->nSendReqSize = nSize;
		bWait = TRUE;
	}

	SignalSema(pFifo->FifoSema);

	if(bWait==TRUE)
	{
	  WaitSema(pFifo->SendSema);
	}
	WaitSema(pFifo->FifoSema);

	t = pFifo->nQueHead + n;			// ���[�C���f�N�X�𓾂�

	if(t > m)
	{
		t -= m;
	}

	// ���b�v�A���E���h���Ȃ��ŃR�s�[�ł���o�C�g�����v�Z
	n = m - t;
	if(nSize <= n)
	{
		// ���b�v�A���E���h�Ȃ�
		memcpy((char*)&pFifo->pData[t], pData, (size_t)nSize);
	}
	else
	{
		// ���b�v�A���E���h����
		memcpy((char*)&pFifo->pData[t], pData, (size_t)n);
		memcpy((char*)pFifo->pData, &((const char *)pData)[n], (size_t)(nSize - n));
	}
	pFifo->nInBytes += nSize;					// FIFO�p�C�v���̗v�f���𑝂₷

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoPollData											ref: mtfifo.h
//
//	�L���[�l����l���擾
//	�Z�}�t�H�҂��Ȃ��BFifoSendData()�ƃy�A�Ŏg�p
//
//	FIFO *pFifo : FIFO�\���̂̃|�C���^
//	const void *pData : FIFO�L���[�ɓn�������f�[�^�̃|�C���^
//	int nSize : FIFO�L���[�ɓn�������f�[�^�̃T�C�Y
//	int fPeek : 
//
//	�߂�l : u_int : 0:����/1:�G���[(�L���[����ꂻ��)
u_int FifoPollData(FIFO *pFifo, void *pData, int nSize, int fPeek)
{
	int h, m, n;
	
	if(nSize == 0)
	{
		return(0);
	}
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;
	n = pFifo->nInBytes;
	if(n < nSize)
	{
		// �L���[���̃f�[�^�ʂ����傫���Ƃ��A�G���[�I��
		SignalSema(pFifo->FifoSema);
		return(0);
	}

	// ���b�v�A���E���h���Ȃ��ŃR�s�[�ł���o�C�g�����v�Z
	h = pFifo->nQueHead;		// �擪�C���f�N�X�𓾂�

	n = m - h;
	if(nSize <= n)
	{
		// ���b�v�A���E���h�Ȃ�
		memcpy(pData,(char*)  &pFifo->pData[h], (size_t)nSize);
		if(!fPeek)
		{
			pFifo->nQueHead = (h + nSize) % m;
			pFifo->nInBytes -= nSize;			// FIFO�p�C�v���̗v�f�������炷
		}
	}
	else
	{
		// ���b�v�A���E���h����
		memcpy(pData, (char*)  &pFifo->pData[h], (size_t)n);
		memcpy(&((char *)pData)[n], (char*)pFifo->pData, (size_t)(nSize - n));
		if(!fPeek)
		{
			pFifo->nQueHead = nSize - n;
			pFifo->nInBytes -= nSize;			// FIFO�p�C�v���̗v�f�������炷
		}
	}

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(1);
}

//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	CheckWaitRequest										ref: mtfifo.h
//
//	���M�A��M�v���T�C�Y����������Ă��邩�ǂ����𔻒f���A
//	�������ꂽ�ꍇ�́AWAIT��Ԃ��J������B
static void CheckWaitRequest(FIFO *pFifo)
{
	if(pFifo->nSendReqSize && ((pFifo->nBufSize - pFifo->nInBytes) >= pFifo->nSendReqSize))
	{
		SignalSema(pFifo->SendSema);
		pFifo->nSendReqSize = 0;
	}
	if(pFifo->nRecvReqSize && ((pFifo->nInBytes >= pFifo->nRecvReqSize)))
	{
		SignalSema(pFifo->RecvSema);
		pFifo->nRecvReqSize = 0;
	}
}

