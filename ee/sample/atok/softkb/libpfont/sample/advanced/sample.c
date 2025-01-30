/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>
#include <eekernel.h>

#include "sample.h"

#include <libdma.h>
#include <libgraph.h>
#include <sifdev.h>
#include <libpfont.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "debug.h"
#include "libpkdbg.h"
#include "mfifo.h"

typedef struct{
	int ode;
	int vsync_sema;
	int vstart_handler;
	int vend_handler;

	int dsync_sema;
	int gs_handler;

	int exit_sema;
	int exit_count;

	int idle_thread;

	////////////////////////////////////////////////////////// �������� font �֘A

	u_long128* font_data;
	int fd;

	sceVu0FMATRIX font_screen;
	sceVu0FMATRIX font_matrix;
	sceVu0FVECTOR font_locate;
	int font_putx;		// ���s���̂��߂ɍ��[���o���Ă���
	int font_calc;		// �������W�v�Z���[�h�t���O
	int font_index;		// �������W�v�Z�ΏۃC���f�b�N�X
	sceVu0FVECTOR* font_pos;	// �������W�o�͔z��

	////////////////////////////////////////////////////////// �����܂� font �֘A

#if defined(__MFIFO__)
	void* mfifo_work;
#endif

#if defined(__PKTDBG__)
	int pkd_level;
#endif

	u_int frame;
	sceGsDBuff db;


}SAMPLE_WORK;

static SAMPLE_WORK _sw;

static char _sample_text[4096];

static u_long128 _idle_stack[IDLE_STACK_SIZE];
static void _idle_thread(void* arg);

static int _vstart_handler(int ch);
static int _vend_handler(int ch);
static int _gs_handler(int c0);
static void _dma1_kick(u_long128 const* packet);

// �t�H���g�R�[���o�b�N����
static int _sampleFontFilter(int fd, scePFontControl* pCtrl);


int appInit(void){

#if 1
	TRACE("spr size = 0x%04X\n", sizeof(SampleStr));
	ASSERT(0x4000 >= sizeof(SampleStr));
#endif

	ChangeThreadPriority(GetThreadId(), MAIN_THREAD_PRIO);

	// ���[�N������
	{
		memset(GetApp(), 0, sizeof(SampleStr));

		memset(&_sw, 0, sizeof(_sw));
		_sw.vsync_sema = -1;
		_sw.vstart_handler = -1;
		_sw.vend_handler = -1;

		_sw.dsync_sema = -1;
		_sw.gs_handler = -1;

		_sw.idle_thread = -1;

		_sw.exit_sema = -1;
		_sw.exit_count = 0;

		_sw.font_data = NULL;
		_sw.fd = -1;

#if defined(__MFIFO__)
		_sw.mfifo_work = NULL;
#endif

#if defined(__PKTDBG__)
		_sw.pkd_level = DBG_PKDUMP_NONE;
#endif
	}

	// �p�P�b�g������
	{
		xVif1PkInit(GetPkt(), GetApp()->pktbuf[0]);

		// Cycle Stealing on!
		DPUT_D_CTRL((DGET_D_CTRL() & ~D_CTRL_RCYC_M) | D_CTRL_RELE_M);

#if defined(__MFIFO__)
		_sw.mfifo_work = _mfifo_alloc(16);
		if(NULL == _sw.mfifo_work){
			return -1;
		}
		_mfifo_init(SCE_DMA_VIF1, _sw.mfifo_work, 1 << 16);
#endif
	}

	////////////////////////////////////////////////////////// �������� font �֘A

	// �t�H���g�f�[�^�ǂݍ��݃��[�N�m��
	_sw.font_data = (u_long128*)memalign(16, FONT_DATASIZE);
	if(NULL == _sw.font_data){
		//WARN("\n");
		return -1;
	}

	// �T���v���e�L�X�g�f�[�^�ǂݍ���
	{
		int fd = sceOpen("host0:utf8.txt", SCE_RDONLY);
		int len;
		if(0 > fd){
			WARN("sceOpen(\"host0:utf8.txt\", SCE_RDONLY) failed\n");
			return -1;
		}
		len = sceRead(fd, _sample_text, sizeof(_sample_text) - 1);
		_sample_text[len] = '\0';
		sceClose(fd);
	}

	// �T���v���t�H���g�f�[�^�ǂݍ���
	{
		int fd = sceOpen("host0:sample.pf", SCE_RDONLY);
		if(0 > fd){
			WARN("sceOpen(\"host0:test.pf\", SCE_RDONLY) failed\n");
			return -1;
		}
		sceRead(fd, _sw.font_data, FONT_DATASIZE);
		sceClose(fd);
	}

	// �t�H���g������
	{
		int csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
		int fsize = scePFontCalcCacheSize(1) - csize;
		int num = (sizeof(GetApp()->font_cache) - fsize) / csize;

		// �t�H���g�Ǘ��\���̏������ݒ�
		_sw.fd = scePFontInit(num, GetApp()->font_cache);
		ASSERT(sizeof(GetApp()->font_cache) >= scePFontCalcCacheSize(num));

		// �t�H���g�C���[�W�p�̃e�N�X�`���̈��ݒ�
		scePFontSetTexMem(_sw.fd, FONT_GSMEM * 64, FONT_GSMEMSIZE * 64, CLUT_GSMEM * 64);

		// �t�H���g�f�[�^�����т���
		if(0 != scePFontAttachData(_sw.fd, _sw.font_data)){
			WARN("scePFontAttachData(sw.fd, sw.font_data) failed\n");
			return -1;
		}

		// �Ǝ������̂��߂̃t�B���^�[���Z�b�g
		scePFontSetFilter(_sw.fd, _sampleFontFilter);
	}

	// �\������������
	{
		// �X�N���[�����e�p�}�g���N�X�ݒ�
		sceVu0UnitMatrix(_sw.font_screen);
		//sceVu0RotMatrixZ(_sw.font_screen, _sw.font_screen, 0.0f * 3.14f / 180.0f);
		{
			sceVu0FMATRIX m;
			sceVu0UnitMatrix(m);
			m[1][1] = 0.5f;
			sceVu0MulMatrix(_sw.font_screen, m, _sw.font_screen);
		}
		_sw.font_screen[3][0] = 2048.0f - SCREEN_WIDTH / 2;
		_sw.font_screen[3][1] = 2048.0f - SCREEN_HEIGHT / 2;

		// �����ό`�p�}�g���N�X�ݒ�
		sceVu0UnitMatrix(_sw.font_matrix);

		// �t�H���g�`����W�ݒ�
		_sw.font_locate[0] = 0.0f;
		_sw.font_locate[1] = 0.0f;
		_sw.font_locate[2] = 0.0f;
		_sw.font_locate[3] = 0.0f;
	}

	////////////////////////////////////////////////////////// �����܂� font �֘A

	// v�����Z�}�t�H�쐬
	{
		struct SemaParam sp;
		memset(&sp, 0, sizeof(sp));
		sp.maxCount = 1;
		sp.initCount = 0;
		sp.option = 0;
		_sw.vsync_sema = CreateSema(&sp);
		if(0 > _sw.vsync_sema){
			return -1;
		}
	}

	// VSYNC���荞�ݐݒ�
	{
		_sw.vstart_handler = AddIntcHandler(INTC_VBLANK_S, _vstart_handler, 0);
		EnableIntc(INTC_VBLANK_S);

		_sw.vend_handler = AddIntcHandler(INTC_VBLANK_E, _vend_handler, 0);
		EnableIntc(INTC_VBLANK_E);
	}

	// �`�擯���Z�}�t�H�쐬
	{
		struct SemaParam sp;
		memset(&sp, 0, sizeof(sp));
		sp.maxCount = 1;
		sp.initCount = 0;
		sp.option = 0;
		_sw.dsync_sema = CreateSema(&sp);
		if(0 > _sw.dsync_sema){
			return -1;
		}
	}

	// GS���荞�ݐݒ�
	{
		_sw.gs_handler = AddIntcHandler(INTC_GS, _gs_handler, 0);
		EnableIntc(INTC_GS);
		DPUT_GS_CSR(GS_CSR_FINISH_M);
		DPUT_GS_IMR(~GS_IMR_FINISHMSK_M);
	}

	// �X���b�h�E�o����
	{
		struct SemaParam sp;
		memset(&sp, 0, sizeof(sp));
		sp.maxCount = 255;	// �X���b�h�ő吔
		sp.initCount = 0;
		sp.option = 0;
		_sw.exit_sema = CreateSema(&sp);
		if(0 > _sw.exit_sema){
			return -1;
		}
	}

	// IDLE�X���b�h���쐬
	{
		struct ThreadParam tp;
		memset(&tp, 0, sizeof(tp));
		tp.entry = _idle_thread;
		tp.stack = _idle_stack;
		tp.stackSize = sizeof(_idle_stack);
		tp.gpReg = &_gp;
		tp.initPriority = IDLE_THREAD_PRIO;
		_sw.idle_thread = CreateThread(&tp);
		if(0 > _sw.idle_thread){
			return -1;
		}
		StartThread(_sw.idle_thread, NULL);
		_sw.exit_count++;
	}

	return 0;
}

// �A�v���P�[�V�����I���A�J������
void appExit(void){

	GetApp()->exit_flag = 1;

	// �e�X���b�h�̏I���҂�
	while(0 < _sw.exit_count--){
		// �^�C���A�E�g�����ꂽ�ق�����������
		WaitSema(_sw.exit_sema);
	}

	// ���������n���B
	if(0 <= _sw.vstart_handler){
		DisableIntc(INTC_VBLANK_S);
		RemoveIntcHandler(INTC_VBLANK_S, _sw.vstart_handler);
		_sw.vstart_handler = -1;
	}

	if(0 <= _sw.vend_handler){
		DisableIntc(INTC_VBLANK_E);
		RemoveIntcHandler(INTC_VBLANK_E, _sw.vend_handler);
		_sw.vend_handler = -1;
	}

	if(0 <= _sw.vsync_sema){
		DeleteSema(_sw.vsync_sema);
		_sw.vsync_sema = -1;
	}

	if(0 <= _sw.gs_handler){
		DisableIntc(INTC_GS);
		RemoveIntcHandler(INTC_GS, _sw.gs_handler);
		_sw.gs_handler = -1;
	}

	if(0 <= _sw.dsync_sema){
		DeleteSema(_sw.dsync_sema);
		_sw.dsync_sema = -1;
	}

	if(0 <= _sw.idle_thread){
		TerminateThread(_sw.idle_thread);
		DeleteThread(_sw.idle_thread);
		_sw.idle_thread = -1;
	}

	if(0 <= _sw.exit_sema){
		DeleteSema(_sw.exit_sema);
		_sw.exit_sema = -1;
	}

	if(0 <= _sw.fd){
		scePFontRelease(_sw.fd);
		_sw.fd = -1;
	}

	if(NULL != _sw.font_data){
		free(_sw.font_data);
		_sw.font_data = NULL;
	}

#if defined(__MFIFO__)
	if(NULL != _sw.mfifo_work){
	   _mfifo_close();
	   _mfifo_free(_sw.mfifo_work);
	   _sw.mfifo_work = NULL;
	}
#endif
}

int appRun(int argc, char const* const argv[]){

	sceGsDBuff* pDB;

	UNUSED_ALWAYS(argc);
	UNUSED_ALWAYS(argv);

	pDB = (sceGsDBuff*)GET_UNCACHED_ADR(&_sw.db);
	sceGsSetDefDBuff(pDB, SCE_GS_PSMCT32, SCREEN_WIDTH, SCREEN_HEIGHT, SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_NOCLEAR);

	_sw.frame = 0;

#if 0
	// �����ʒu���v�Z�̃e�X�g
	{
		int i;
		char const str[] = "abcd";
		sceVu0FVECTOR pos[128];
		sampleCalcChrPos(str, 0, 0, pos);
		for(i = 0; i < ((int)sizeof(str) - 1); i++){
			printf("%f %f\n", pos[i][0], pos[i][1]);
		}
	}
#endif

#if defined(__MFIFO__)
	_mfifo_start(1, 1);
#endif

	// ���C�����[�v
	for(;;){
		// ���O�̕`��I���҂�
		sampleDrawSync();

#if defined(__MFIFO__)
		_mfifo_stop();
#endif

		// VSync�҂�
		if(0 > WaitSema(_sw.vsync_sema)){
			break;
		}

		// �_�u���o�b�t�@�ؑւ�
		{
			int side = _sw.frame & 1;
			TRACE("frame=%d ode=%d\n", _sw.frame, _sw.ode);

			sceGsSetHalfOffset(side ? &pDB->draw1 : &pDB->draw0, 2048, 2048, !_sw.ode);
			sceGsSwapDBuff(pDB, side);
			_sw.frame++;
		}

		// �I���`�F�b�N
		if(0 != GetApp()->exit_flag){
			break;
		}

#if defined(__MFIFO__)
		_mfifo_start(1, 1);
#endif


#if 1
		// 10�b��ɋ����I��
		if(60 * 10 < _sw.frame){
			WARN("time up!\n");
			GetApp()->exit_flag = 1;
		}
#endif

		// �w�i�h��Ԃ�
		sampleDrawSolidRect(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0x007080a0U);

		// �����o��
		samplePuts(_sample_text, 24.0f, 24.0f);


		// ���[�U�[��`�����o�̓e�X�g
		//jokeKPuts("Qapla\'", 320.0f, 24.0f);

#if 1
		// ���ו\��
		{
			sampleDrawSolidRect(600, 616, 0, SCREEN_HEIGHT, 0x000000ffU);

			{
				sceGifTag* giftag = (0 == (_sw.frame & 1)) ? &pDB->giftag0 : &pDB->giftag1;
				u_int qwc = giftag->NLOOP + 1;
				xVif1PkRef(GetPkt(), (u_long128*)giftag, qwc, 0, SCE_VIF1_SET_DIRECT(qwc, 0), 0);
				xVif1PkCnt(GetPkt(), 0);
				sampleDrawKick();
			}

			sampleDrawSolidRect(600, 616, 0, SCREEN_HEIGHT, 0x0000ff00U);
		}
#endif

	}

	return 0;
}

////////////////////////////////////////////////////////// �������� font �֘A

// �t�H���g�R�[���o�b�N����
static int _sampleFontFilter(int fd, scePFontControl* pCtrl){
	int stat = -1;
	int c;

	while(0 < (c = pCtrl->Getc(fd))){
		// �P�����擾

		if(0 != _sw.font_calc){
			scePFontGetLocate(fd, &_sw.font_pos[_sw.font_index]);
		}

		if(0x20 > c){
			// �R���g���[���R�[�h
			
			switch(c){
			case '\n':	// ���s����
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] = (float)_sw.font_putx;// �ŏ���X���W
					v[1] += 26.0f;	// �K���ȍs��
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			case '\t':	// �^�u
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] += 32.0f;	// �K���ȃ^�u��
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			default:
				break;
			}

		}else{
			// �ʏ핶���`�揈��
			if(-1 == pCtrl->Putc(fd, c)){
				// �p�P�b�g�̈悪�s���������ߎ擾����������߂��Ĕ�����
				pCtrl->Ungetc(fd);
				return stat;
			}
			stat = 1;
		}

		if(0 != _sw.font_calc){
			_sw.font_index++;
		}
	}

	// �����I��
	return 0;
}

// �t�H���g�o��
void samplePuts(char const* str, int x, int y){
	{
		_sw.font_calc = 0;
		_sw.font_putx = x;
		_sw.font_locate[0] = (float)x;
		_sw.font_locate[1] = (float)y + 26.0f;
	}

	// �t�H���g�̍��W��}�g���N�X��^����
	scePFontSetScreenMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_screen);
	scePFontSetFontMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_matrix);
	scePFontSetLocate(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_locate);

	// �t�H���g��`�悷�邽�߂̊���ݒ�
	{
		union{
			u_long d[2];
			u_long128 tag;
		}const gif = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};
		xVif1PkCnt(GetPkt(), 0);
		xVif1PkOpenDirectCode(GetPkt(), 0);
		xVif1PkOpenGifTag(GetPkt(), gif.tag);

		xVif1PkAddGsAD(GetPkt(), SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
		xVif1PkAddGsAD(GetPkt(), SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));

		xVif1PkCloseGifTag(GetPkt());
		xVif1PkCloseDirectCode(GetPkt());
	}

	// �p�P�b�g�\�z
	{
		int result;

		// scePFontPuts �̒������ɐݒ肵��filter���Ă΂��

		// ���������J�n
		if(1 == (result = scePFontPuts(_sw.fd, GetPkt(), PKTBUF_SIZE - 1, str))){
			do{
				// �������p�P�b�g���o��
				xVif1PkCnt(GetPkt(), 0);
				sampleDrawKick();

				// �p������
			}while(1 == (result = scePFontPutsContinue(_sw.fd, GetPkt(), PKTBUF_SIZE - 1)));
		}
	}
	xVif1PkCnt(GetPkt(), 0);

	// �`��
	sampleDrawKick();
}

// �t�H���g���o�͂����ꍇ�̊e�����̈ʒu��Ԃ�
// pPosList �ɂ͕��������̗v�f�����K�v
void sampleCalcChrPos(char const* str, int x, int y, sceVu0FVECTOR* pPosList){
	{
		_sw.font_pos = pPosList;
		_sw.font_calc = 1;
		_sw.font_putx = x;
		_sw.font_locate[0] = (float)x;
		_sw.font_locate[1] = (float)y + 26.0f;
	}

	// �t�H���g�̍��W��}�g���N�X��^����
	scePFontSetScreenMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_screen);
	scePFontSetFontMatrix(_sw.fd, (sceVu0FMATRIX const*)&_sw.font_matrix);
	scePFontSetLocate(_sw.fd, (sceVu0FVECTOR const*)&_sw.font_locate);

	// ���W�v�Z
	scePFontCalcRect(_sw.fd, str, NULL, NULL);
}

////////////////////////////////////////////////////////// �����܂� font �֘A

// �`��I���҂�
void sampleDrawSync(void){

#if !defined(__MFIFO__) && defined(__PKDBG__)
	dbgGsSyncPath(0, 0);
#endif

	// �Z�}�t�H�N���A
	//PollSema(_dw.dsync_sema);

	// FINISH��ł�
	{
		xVif1PkCnt(GetPkt(), 0);
		xVif1PkOpenDirectCode(GetPkt(), 0);
		xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL);
		xVif1PkAddGsAD(GetPkt(), SCE_GS_FINISH, 0);
		xVif1PkCloseGifTag(GetPkt());
		xVif1PkCloseDirectCode(GetPkt());
		sampleDrawKick();
	}

	// FINISH �����������̂�҂�
	WaitSema(_sw.dsync_sema);

	return;
}

// �`��J�n
void sampleDrawKick(void){
	u_int qwc = xVif1PkSize(GetPkt());

#if defined(__DEBUG__)
	ASSERT((0 != qwc) && (PKTBUF_SIZE >= qwc));
#endif

#if defined(__MFIFO__)
	xVif1PkTerminate(GetPkt());

#if defined(__PKDBG__)
	dbgDumpPacketDma(GetPkt()->pBase, &GetPkt()->pBase[qwc], 0, _sw.pkd_level);
#endif

	_mfifo_kick(GetPkt()->pBase, qwc, 0);

#else
	xVif1PkTerminateR(GetPkt());

#if defined(__PKDBG__)
	dbgDumpPacketDma(GetPkt()->pBase, &GetPkt()->pBase[qwc], 0, _sw.pkd_level);
#endif

	_dma1_kick(GetPkt()->pBase);
#endif

	xVif1PkInit(GetPkt(), (GetPkt()->pBase == GetApp()->pktbuf[0]) ? GetApp()->pktbuf[1] : GetApp()->pktbuf[0]);
}

// ����DMA
static void _dma1_kick(u_long128 const* packet){
	sceDmaChan volatile* dc = sceDmaGetChan(SCE_DMA_VIF1);
	sceDmaSync((sceDmaChan*)(u_int)dc, 0, 0);
	if(7 == ((u_int)packet >> 28)){
		packet = (u_long128 const*)(((u_int)packet & 0x000003ff0U) | 0x80000000U);
	}
	dc->tadr = (sceDmaTag*)(u_int)packet;
	dc->qwc = 0;
	{
		tD_CHCR	const chcr = { 1, 0, 1, 0, 1, 0, 1, 0, 0 };
		dc->chcr = chcr;
	}
}

// ��`�`��
void sampleDrawSolidRect(int left, int right, int top, int bottom, u_int rgba){

	xVif1PkCnt(GetPkt(), 0);
	xVif1PkOpenDirectCode(GetPkt(), 0);
	xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 0, 0, 0, 0, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_RGBAQ, (u_long)rgba);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4, (SCREEN_TOP + top) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4, (SCREEN_TOP + top) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + left) << 4, (SCREEN_TOP + bottom) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + right) << 4, (SCREEN_TOP + bottom) << 4, 0));
	xVif1PkCloseGifTag(GetPkt());
	xVif1PkCloseDirectCode(GetPkt());
	sampleDrawKick();

	return;
}

// �S��ʂ�Z���N���A
void sampleClearZBuffer(u_int z){
	xVif1PkCnt(GetPkt(), 0);
	xVif1PkOpenDirectCode(GetPkt(), 0);
	xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEST_1, SCE_GS_SET_TEST(1, 0, 0, 2, 0, 0, 1, 1));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 0, 0, 0, 0, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ(SCREEN_LEFT << 4, SCREEN_TOP << 4, z));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ(SCREEN_RIGHT << 4, SCREEN_TOP << 4, z));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ(SCREEN_LEFT << 4, SCREEN_BOTTOM << 4, z));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ(SCREEN_RIGHT << 4, SCREEN_BOTTOM << 4, z));
	xVif1PkCloseGifTag(GetPkt());
	xVif1PkCloseDirectCode(GetPkt());
	sampleDrawKick();

	return;
}

void sampleSetTexture(u_int tbp, u_int tbw, u_int tpsm, u_int tw, u_int th, u_int cbp, u_int cpsm){

	xVif1PkCnt(GetPkt(), 0);
	xVif1PkOpenDirectCode(GetPkt(), 0);
	xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEXFLUSH, 0);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEX0_1, SCE_GS_SET_TEX0(tbp, tbw, tpsm, tw, th, 1, 0, cbp, cpsm, 0, 0, 1));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEX1_1, SCE_GS_SET_TEX1(0, 0, 0, 0, 0, 0, 0));
	xVif1PkCloseGifTag(GetPkt());
	xVif1PkCloseDirectCode(GetPkt());
	sampleDrawKick();

	return;
}

void sampleDrawSprite(int u, int v, int w, int h, int x, int y){
	xVif1PkCnt(GetPkt(), 0);
	xVif1PkOpenDirectCode(GetPkt(), 0);
	xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEST_1, SCE_GS_SET_TEST(1, 0, 0, 1, 0, 0, 1, 1));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 0, 0, 1, 0, 0));

	xVif1PkAddGsAD(GetPkt(), SCE_GS_RGBAQ, 0x80808080UL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV(u << 4, v << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x) << 4, (SCREEN_TOP + y) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV((u + w) << 4, (v + h) << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x + w) << 4, (SCREEN_TOP + y + h) << 4, 0));
	xVif1PkCloseGifTag(GetPkt());
	xVif1PkCloseDirectCode(GetPkt());
	sampleDrawKick();

	return;
}

void sampleDrawSpriteEx(int u, int v, int uw, int uh, int x, int y, int w, int h){
	xVif1PkCnt(GetPkt(), 0);
	xVif1PkOpenDirectCode(GetPkt(), 0);
	xVif1PkOpenGifTag2(GetPkt(), SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_TEST_1, SCE_GS_SET_TEST(1, 0, 0, 1, 0, 0, 1, 1));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 0, 0, 1, 0, 0));

	xVif1PkAddGsAD(GetPkt(), SCE_GS_RGBAQ, 0x80808080UL);
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV(u << 4, v << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x) << 4, (SCREEN_TOP + y) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV((u + uw) << 4, v << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x + w) << 4, (SCREEN_TOP + y) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV(u << 4, (v + uh) << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x) << 4, (SCREEN_TOP + y + h) << 4, 0));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_UV, SCE_GS_SET_UV((u + uw) << 4, (v + uh) << 4));
	xVif1PkAddGsAD(GetPkt(), SCE_GS_XYZ2, SCE_GS_SET_XYZ((SCREEN_LEFT + x + w) << 4, (SCREEN_TOP + y + h) << 4, 0));
	xVif1PkCloseGifTag(GetPkt());
	xVif1PkCloseDirectCode(GetPkt());
	sampleDrawKick();

	return;
}

//
static void _idle_thread(void* arg){

	UNUSED_ALWAYS(arg);

	for(;;){
		if(0 != GetApp()->exit_flag){
			break;
		}

		// nop T-T
	}

	_sw.idle_thread = -1;

	SignalSema(_sw.exit_sema);

	ExitDeleteThread();

	return;
}

// VSYNC�����ׂ̈�VBLANK�̊Ԃ����Z�}�t�H���V�O�i���ɂ���
static int _vstart_handler(int ch){
	UNUSED_ALWAYS(ch);

	_sw.ode = (0 == (DGET_GS_CSR() & GS_CSR_FIELD_M)) ? 0 : 1;

	iPollSema(_sw.vsync_sema);
	iSignalSema(_sw.vsync_sema);
	ExitHandler();
	return 0;
}

static int _vend_handler(int ch){
	UNUSED_ALWAYS(ch);
	iPollSema(_sw.vsync_sema);
	ExitHandler();
	return 0;
}

// �`��I������̂��߂�FINISH���荞�݂��g�p
static int _gs_handler(int c0){
	UNUSED_ALWAYS(c0);

	if(0 != (DGET_GS_CSR() & GS_CSR_FINISH_M)){
		DPUT_GS_CSR(GS_CSR_FINISH_M);
		iPollSema(_sw.dsync_sema);
		iSignalSema(_sw.dsync_sema);
	}

	ExitHandler();
	return 0;
}

// ���[�U�[��`�����o�͂̃e�X�g
void jokeKPuts(char const* str, int x, int y){
	struct{
		u_int uc;
		char const* ut;
		char const* al;
	} const kmap[] = {

		{ 0xf8d0U, "\xef\xa3\x90", "a" },
		{ 0xf8d1U, "\xef\xa3\x91", "b" },
		{ 0xf8d2U, "\xef\xa3\x92", "ch" },
		{ 0xf8d3U, "\xef\xa3\x93", "D" },
		{ 0xf8d4U, "\xef\xa3\x94", "e" },
		{ 0xf8d5U, "\xef\xa3\x95", "gh" },
		{ 0xf8d6U, "\xef\xa3\x96", "H" },
		{ 0xf8d7U, "\xef\xa3\x97", "I" },
		{ 0xf8d8U, "\xef\xa3\x98", "j" },
		{ 0xf8d9U, "\xef\xa3\x99", "l" },
		{ 0xf8daU, "\xef\xa3\x9a", "m" },
		{ 0xf8dbU, "\xef\xa3\x9b", "n" },
		{ 0xf8dcU, "\xef\xa3\x9c", "ng" },
		{ 0xf8ddU, "\xef\xa3\x9d", "o" },
		{ 0xf8deU, "\xef\xa3\x9e", "p" },
		{ 0xf8dfU, "\xef\xa3\x9f", "q" },
		{ 0xf8e0U, "\xef\xa3\xa0", "Q" },
		{ 0xf8e1U, "\xef\xa3\xa1", "r" },
		{ 0xf8e2U, "\xef\xa3\xa2", "S" },
		{ 0xf8e3U, "\xef\xa3\xa3", "t" },
		{ 0xf8e4U, "\xef\xa3\xa4", "tlh" },
		{ 0xf8e5U, "\xef\xa3\xa5", "u" },
		{ 0xf8e6U, "\xef\xa3\xa6", "v" },
		{ 0xf8e7U, "\xef\xa3\xa7", "w" },
		{ 0xf8e8U, "\xef\xa3\xa8", "y" },
		{ 0xf8e9U, "\xef\xa3\xa9", "\'" },

		{ 0xf8f0U, "\xef\xa3\xb0", "0" },
		{ 0xf8f1U, "\xef\xa3\xb1", "1" },
		{ 0xf8f2U, "\xef\xa3\xb2", "2" },
		{ 0xf8f3U, "\xef\xa3\xb3", "3" },
		{ 0xf8f4U, "\xef\xa3\xb4", "4" },
		{ 0xf8f5U, "\xef\xa3\xb5", "5" },
		{ 0xf8f6U, "\xef\xa3\xb6", "6" },
		{ 0xf8f7U, "\xef\xa3\xb7", "7" },
		{ 0xf8f8U, "\xef\xa3\xb8", "8" },
		{ 0xf8f9U, "\xef\xa3\xb9", "9" },
	};
	char output[1024];
	char* ptr = output;

	while('\0' != *str){
		size_t len = 0;
		int mat = 0;
		int i;
		for(i = 0; i < (int)bulkof(kmap); i++){
			size_t n = strlen(kmap[i].al);
			if(0 == strncmp(kmap[i].al, str, n)){
				if(len < n){
					len = n;
					mat = i;
				}
			}
		}

		if(0 == len){
			*ptr++ = *str++;

		}else{
			strcpy(ptr, kmap[mat].ut);
			ptr += strlen(kmap[mat].ut);
			str += len;
		}
	}
	*ptr = '\0';

	samplePuts(output, x, y);
}

// [eof]
