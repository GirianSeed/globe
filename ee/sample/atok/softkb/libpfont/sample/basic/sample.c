/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
 */
/* 
 *                libpfont Library Sample Program
 *
 *                           - basic -
 *
 *                          Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                             sample.c
 *                      main function of basic.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Jan,14,2002      gens
 */

#include <string.h>
#include <malloc.h>

#include <eetypes.h>
#include <sifdev.h>

#include <libdma.h>
#include <libgraph.h>
#include <libpfont.h>

#include "sample.h"

typedef struct
{
	int				ode;			/* ��ʕ`�擯�� */
	u_int			frame;			/* �t���[���� */
	sceGsDBuff		db;				/* �_�u���o�b�t�@ */

	/*** �������� font �֘A ***/
	u_long128*		font_data;		/* �f�[�^�i.pf �t�@�C���j */
	int				fd;				/* ���ʎq */

	sceVu0FMATRIX	font_screen;	/* �X�N���[�����e�p�}�g���N�X */
	sceVu0FMATRIX	font_matrix;	/* �ό`�p�}�g���N�X */
	sceVu0FVECTOR	font_locate;	/* �`��J�n�ʒu */

	sceVu0FVECTOR	font_color;		/* �F */
	float			font_pitch;		/* ���� */
	int				font_putx;		/* ���s���̂��߂ɍ��[���o���Ă��� */

	/*** �����܂� font �֘A ***/

} SAMPLE_WORK;

static SAMPLE_WORK		_sw;
static char				_sample_text[4096];

static sceVif1Packet	_Packet;
static sceVif1Packet*	_pPacket;
static u_long128		_font_cache[64];	/* 1k bytes */
static u_long128*		_pktbuf[2];
static int				_exit_flag = 0;

/* �X�v���C�g�pGIF�^�O */
union
{
	u_long d[2];
	u_long128 tag;
} const s_gif = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};

/* �f�t�H���g�e�L�X�g */
static char	c_readtext[] = "host0:utf8.txt";

/* �t�H���g�f�[�^ */
static char	c_readfont[] = "host0:sample.pf";

/* �t�B���^�[�֐� */
static int _sampleFontFilter(int fd, scePFontControl* pCtrl);

static void sampleDrawKick(void);
static void sampleClear(sceGsDBuff* pDB);
static void samplePuts(char const* str, int x, int y);

/* ���܂��F �l�p��`�悷�� */
static void sampleDrawSolidRect(
	int left, int right, int top, int bottom, u_int rgba);

int appInit(void)
{
	/* ���[�N������ */
	_pPacket		= &_Packet;

	_pktbuf[0]		= (u_long128*)(SCRATCH_PAD_RAM);
	_pktbuf[1]		= (u_long128*)(SCRATCH_PAD_RAM + PKTBUF_SIZE);

	memset(&_sw, 0, sizeof(_sw));
	memset(_pPacket, 0, sizeof(sceVif1Packet));
	memset(_font_cache, 0, sizeof(u_long128) * 64);
	memset(_pktbuf[0], 0, sizeof(u_long128) * PKTBUF_SIZE * 2);

	_sw.font_data	= NULL;
	_sw.fd			= -1;

	/* �p�P�b�g������ */
	sceVif1PkInit(_pPacket, _pktbuf[0]);

	/*** �������� font �֘A ***/
	/* �t�H���g�f�[�^�ǂݍ��݃��[�N�m�� */
	{
		int		filed;		/* �t�@�C�� */

		if(NULL == (_sw.font_data = (u_long128*)memalign(16, FONT_DATASIZE)))
		{
			scePrintf("Memory alloc faild.\n");
			return -1;
		}
		/* �T���v���t�H���g�f�[�^�ǂݍ��� */
		if(0 > (filed = sceOpen(c_readfont, SCE_RDONLY)))
		{
			scePrintf("sceOpen(\"%s\", SCE_RDONLY) failed (%d)\n",
				c_readfont, filed);
			return -1;
		}
		sceRead(filed, _sw.font_data, FONT_DATASIZE);
		sceClose(filed);
	}
	/* �t�H���g������ */
	{
		int		csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
		int		fsize = scePFontCalcCacheSize(1) - csize;
		int		num   = (sizeof(_font_cache) - fsize) / csize;

		/* �t�H���g�Ǘ��\���̏������ݒ� */
		_sw.fd	= scePFontInit(num, _font_cache);

		if(sizeof(_font_cache) < scePFontCalcCacheSize(num))
		{
			scePrintf("font cache size faild.\n");
			return -1;
		}
		/* �t�H���g�C���[�W�p�̃e�N�X�`���̈��ݒ� */
		scePFontSetTexMem(
			_sw.fd, FONT_GSMEM * 64, FONT_GSMEMSIZE * 64, CLUT_GSMEM * 64);

		/* �t�H���g�f�[�^�����т��� */
		if(0 != scePFontAttachData(_sw.fd, _sw.font_data))
		{
			scePrintf("scePFontAttachData(sw.fd, sw.font_data) failed.\n");
			return -1;
		}
		/* �Ǝ������̂��߂̃t�B���^�[���Z�b�g */
		scePFontSetFilter(_sw.fd, _sampleFontFilter);

		/* �t�H���g�f�[�^������ */
		{
			_sw.font_color[0]	= 0xff;		/* R */
			_sw.font_color[1]	= 0xff;		/* G */
			_sw.font_color[2]	= 0xff;		/* B */
			_sw.font_color[3]	= 0xff;		/* A */

			_sw.font_pitch	= 2.5f;			/* ���� */
		}
	}
	/* �\������������ */
	{
		/* �X�N���[�����e�p�}�g���N�X�ݒ� */
		sceVu0UnitMatrix(_sw.font_screen);
		{
			sceVu0FMATRIX	m;

			sceVu0UnitMatrix(m);

			m[1][1] = 0.5f;			/* �c�T�C�Y�𔼕��ɂ��� */
			sceVu0MulMatrix(_sw.font_screen, m, _sw.font_screen);
		}
		_sw.font_screen[3][0]	= 2048.0f - SCREEN_WIDTH / 2;
		_sw.font_screen[3][1]	= 2048.0f - SCREEN_HEIGHT / 2;

		/* �����ό`�p�}�g���N�X�ݒ� */
		sceVu0UnitMatrix(_sw.font_matrix);
	}
	/*** �����܂� font �֘A ***/

	return 0;
}

/* �A�v���P�[�V�����I���A������� */
void appExit(void)
{
	if(0 <= _sw.fd)
	{
		scePFontRelease(_sw.fd);
		_sw.fd	= -1;
	}
	if(NULL != _sw.font_data)
	{
		free(_sw.font_data);
		_sw.font_data	= NULL;
	}
}

int appRun(int argc, char* argv[])
{
	sceGsDBuff*		pDB;

	/* �T���v���e�L�X�g�f�[�^�ǂݍ��� */
	{
		char*	filename = c_readtext;
		int		filed;
		int		len;

		if(1 < argc)
		{
			/* �p�����^���͂��������Ȃ�΁A������t�@�C�����Ƃ݂Ȃ� */
			/* �i��: run main.elf host0:myutf8.txt�j */
			filename	= argv[1];
		}
		if(0 > (filed = sceOpen(filename, SCE_RDONLY)))
		{
			scePrintf("sceOpen(\"%s\", SCE_RDONLY) failed (%d)\n",
				filename, filed);
			return filed;
		}
		len		= sceRead(filed, _sample_text, sizeof(_sample_text) - 1);
		_sample_text[len]	= '\0';

		sceClose(filed);
	}
	FlushCache(WRITEBACK_DCACHE);

	pDB	= (sceGsDBuff*)(((u_int)(&_sw.db) & 0x0fffffffU) | 0x20000000U);

	/* �_�u���o�b�t�@�w��i��ʂ��N���A�������Ȃ��ꍇ�� SCE_GS_NOCLEAR�j */
	sceGsSetDefDBuff(pDB, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT,
		SCE_GS_ZNOUSE, SCE_GS_PSMZ24, SCE_GS_CLEAR);

	_sw.frame 	= 0;
	_exit_flag	= 0;

	/* �O��`�����N���A���� */
	sampleClear(pDB);

	/* ���C�����[�v */
	for(;;)
	{
		/* VSync�҂� */
		_sw.ode = ! sceGsSyncV(0);

		/* �_�u���o�b�t�@�ؑւ� */
		{
			int		side = _sw.frame & 1;

//			scePrintf("frame=%d ode=%d side = %d\n", _sw.frame, _sw.ode, side);

			/* �C���^�[���X�`�揈�� */
			sceGsSetHalfOffset(side ? &pDB->draw1 : &pDB->draw0,
				2048, 2048, _sw.ode);

			sceGsSwapDBuff(pDB, side);

			/* �t���[�����C���N�������g */
			_sw.frame++;
		}
		/* �I���`�F�b�N */
		if(0 != _exit_flag)
		{
			/* �I�����`�����N���A���� */
			sampleClear(pDB);

			break;
		}
		/* 5 �b��ɋ����I�������� */
		if(60 * 5 < _sw.frame)
		{
			scePrintf("time up!\n");
			_exit_flag	= 1;
		}
		/* �΃E�B���h�E */
		sampleDrawSolidRect(100, SCREEN_WIDTH - 100, 50, SCREEN_HEIGHT -50,
			0xff446644U);

		/* �����o�� */
		samplePuts(_sample_text, 100 + 10, 50 + 5);
	}
	return 0;
}

/*** �������� font �֘A ***/
/* �t�H���g�R�[���o�b�N���� */
static int _sampleFontFilter(int fd, scePFontControl* pCtrl)
{
	int		stat = -1;
	int		c;

	while(0 < (c = pCtrl->Getc(fd)))
	{
		/* �P�����擾 */
		if(0x20 > c)
		{
			/* �R���g���[���R�[�h */
			switch(c)
			{
				case '\n':	/* ���s���� */
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);

					v[0]	= (float)_sw.font_putx;		/* �ŏ���X���W */
					v[1]	+= 26.0f;					/* �K���ȍs�� */
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);

					break;
				}
				case '\t':	/* �^�u */
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);

					v[0]	+= 32.0f;					/* �K���ȃ^�u�� */
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);

					break;
				}
				case 0x0d:	/* Windows files ���s�폜 */
					break;
				default:;
			}
		}
		else
		{
#ifdef _DEBUG // {
			/* �������o�^����Ă��邩�`�F�b�N����B */
			{
				scePFontGlyph	glyph;

				if(-1 == scePFontGetGlyph(fd, c, &glyph))
				{
					/* �������o�^����Ă��Ȃ� */
					scePrintf("This is not support font ! (0x%08x)\n", c);
					_exit_flag	= 1;
				}
			}
#endif // }
			/* �ʏ핶���`�揈�� */
			if(-1 == pCtrl->Putc(fd, c))
			{
				/* �p�P�b�g�̈悪�s���������ߎ擾����������߂��Ĕ����� */
				pCtrl->Ungetc(fd);

				return stat;
			}
			stat	= 1;
		}
	}
	return 0;
}

/* �t�H���g�o�� */
static void samplePuts(char const* str, int x, int y)
{
	/* �t�H���g�f�[�^������ */
	{
		_sw.font_putx	= x;
		_sw.font_locate[0]	= (float)x;
		_sw.font_locate[1]	= (float)(y * 2) + 26.0f;
	}
	/* �t�H���g�̍��W��}�g���N�X��^���� */
	scePFontFlush(_sw.fd);
	scePFontSetScreenMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_screen);
	scePFontSetFontMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_matrix);

	/* �ʒu */
	scePFontSetLocate(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_locate);
	/* �F */
	scePFontSetColor(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_color);
	/* ���� */
	scePFontSetPitch(_sw.fd, _sw.font_pitch);

	/* �t�H���g��`�悷�邽�߂̊���ݒ� */
	{
		sceVif1PkCnt(_pPacket, 0);
		sceVif1PkOpenDirectCode(_pPacket, 0);
		sceVif1PkOpenGifTag(_pPacket, s_gif.tag);

		/* ������ */
		sceVif1PkAddGsAD(_pPacket,
			SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
		sceVif1PkAddGsAD(_pPacket,
			SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));

		sceVif1PkCloseGifTag(_pPacket);
		sceVif1PkCloseDirectCode(_pPacket);
	}
	/* ���������J�n */
	/* scePFontPuts �̒������ɐݒ肵��filter���Ă΂�� */
	if(1 == scePFontPuts(_sw.fd, _pPacket, PKTBUF_SIZE - 1, str))
	{
		do
		{
			/* �������p�P�b�g���o�� */
			sampleDrawKick();

			/* �p������ */
		} while(1 == scePFontPutsContinue(_sw.fd, _pPacket, PKTBUF_SIZE - 1));
	}
	/* �`�� */
	sampleDrawKick();

	return;
}

/*** �����܂� font �֘A ***/

/* �`��J�n */
static void sampleDrawKick(void)
{
	sceDmaChan*		dc = sceDmaGetChan(SCE_DMA_VIF1);
	u_int			addr = (u_int)_pPacket->pBase;

	/* END �^�O�̒ǉ� */
	sceVif1PkEnd(_pPacket, 0);

	/* �^�[�~�l�C�g */
	sceVif1PkTerminate(_pPacket);

	/* DMA �]���������� */
	dc->chcr.TTE	= 1;

	/* DMA �]�������҂� */
	sceDmaSync(dc, 0, 0);

	/* DMA �]���J�n */
	{
		/* SPR��̃A�h���X�̏ꍇDMA�ɐݒ肷��A�h���X�� */
		/* 31bit�ڂ�1�ɂ���K�v������̂ŕϊ�����B */
		/* Runtime Library Release 2.5 �ȍ~�͊֐������ōs���̂ŕK�v�Ȃ����� */
		if((addr >> 28 ) == 7 )
		{
			addr	&= 0x0fffffff;
			addr	|= D_MADR_SPR_M;
		}
		/* �����܂� */
	}
	sceDmaSend(dc, (void*)addr);

	/* ������ */
	sceVif1PkInit(_pPacket,
		(_pPacket->pBase == _pktbuf[0]) ? _pktbuf[1] : _pktbuf[0]);

	/* �f�[�^�]���̏I���҂� */
	sceGsSyncPath(0, 0);

	return;
}

/* �`��N���A */
static void sampleClear(sceGsDBuff* pDB)
{
	int		side = _sw.frame & 1;

	/* �I�����`�����N���A���� */
	sceGsSwapDBuff(pDB, side);
    sceGsSyncPath(0, 0);
	sceGsSyncV(0);
	sceGsSwapDBuff(pDB, side ^ 1);
    sceGsSyncPath(0, 0);
	sceGsSyncV(0);

	return;
}

/* ��`�`�� */
static void sampleDrawSolidRect(
	int left, int right, int top, int bottom, u_int rgba)
{
	sceVif1PkCnt(_pPacket, 0);
	sceVif1PkOpenDirectCode(_pPacket, 0);
	sceVif1PkOpenGifTag(_pPacket, s_gif.tag);

	/* �g���C�A���O���X�g���b�v */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_PRIM,
		SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 0, 0, 0, 0, 0));

	/* �F */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_TEST_1,
		SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_RGBAQ, (u_long)rgba);

	/* ���_ */
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4,
		(SCREEN_TOP + top) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4,
		(SCREEN_TOP + top) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4,
		(SCREEN_TOP + bottom) << 4, 0));
	sceVif1PkAddGsAD(_pPacket, SCE_GS_XYZ2,
		SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4,
		(SCREEN_TOP + bottom) << 4, 0));

	sceVif1PkCloseGifTag(_pPacket);
	sceVif1PkCloseDirectCode(_pPacket);

	/* �`��J�n */
	sampleDrawKick();

	return;
}

/* [eof] */
