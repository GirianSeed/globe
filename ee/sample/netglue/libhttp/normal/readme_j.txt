[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libhttp �𗘗p���� HTTP �T�[�o�ɐڑ�����T���v���B

<�T���v���̉��>
	�{�v���O������ libhttp �𗘗p���� HTTP �T�[�o��
	�ڑ�����T���v���ł��B
	libhttp �� HTTP �ڑ��̍ۂɃu���b�L���O�A�m���u���b�L���O��
	�I���ł��܂����A�{�T���v���̓m���u���b�L���O�Őڑ�������̂ł��B

	�{�T���v���́Alibhttp �� inet ��Ŏ������邽�߁A
	netglue ���C�u�����Ƃ��� netglue_insck.a �𗘗p���Ă��܂��B
	inet �ȊO�̃X�^�b�N�Ŗ{�T���v�������s����ɂ́A���̃X�^�b�N�p��
	netglue ���C�u�������ʓr�K�v�ɂȂ�܂��B

[ ���� ]
	���̃T���v���ł� USB �C�[�T�A�_�v�^���g�p����ꍇ�ɁAan986.irx ��
	�K�v�ł��B
	an986.irx �ɂ��ẮA�^�C�g�����ł̎g�p���֎~�Ƃ��Ă��邽��
	�i�ڍׂ� 2000/11/22 �t���� SCE-NET �́u�Z�p���v���Q�Ƃ��������j
	�����[�X�p�b�P�[�W�isce/iop/modules ���j�ɂ͊܂܂�Ă���܂���B
	SCE-NET �́u�_�E�����[�h�v->�u���̑����ʃc�[���v����_�E�����[�h
	���Ă��������B

<�t�@�C��>
	http_test.c        : ���C���v���O����

<�N�����@>
	$ make                          : �R���p�C��

	$ dsedb -r run http_test.elf command <URL> [post-data/file] : ���s

	�w�肳�ꂽ URL �ɑ΂��� HTTP �ڑ����s�Ȃ��AHTTP ���X�|���X��
	�\�����܂��B

	command �� HTTP ���\�b�h���w�肷����̂ŁA�ȉ���3���w��ł��܂��B
		-get	GET ���\�b�h
		-head	HEAD ���\�b�h
		-post	POST ���\�b�h
	POST ���\�b�h�̏ꍇ�́Apost-data �Ƃ��� POST ����f�[�^��
	�w��ł��܂��B�܂� GET ���\�b�h�̏ꍇ�͉����̃{�f�B����
	����M����t�@�C�������w�肷�邱�Ƃ��ł��܂��B


<�R���g���[���̑�����@>
        �Ȃ�

<���l>
	�{�v���O�����ł͐ڑ����Ƃ��āAUSB�C�[�T�l�b�g�AHDD �C�[�T�l�b�g�A
	PPPoE(USB�C�[�T�l�b�g)�APPPoE(HDD�C�[�T�l�b�g)��z�肵�Ă��܂��B
	�{�v���O�������̐擪�ɂ���ȉ��� #define ��؂�ւ��邱�Ƃɂ����
	�ڑ�����ύX���邱�Ƃ��\�ł��B

	#define USB_ETHERNET			: USB�C�[�T�l�b�g
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USB�C�[�T�l�b�g)
	#define HDD_ETHERNET			: HDD�C�[�T�l�b�g
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDD�C�[�T�l�b�g)

	IP�A�h���X�̕ύX��PPPoE �̃��[�U���ύX���s�Ȃ�����PPP ���g�p����
	�ꍇ�́A�e�ڑ����p�̐ݒ薼 "Combination?"(? �͐��l) �������ݒ�
	�t�@�C����K�؂ɕύX���ĉ������B
	�Ȃ��A���L�����ꂼ��ɑΉ�����ݒ薼����ѐݒ�t�@�C���ł��B

	USB�C�[�T�l�b�g          Combination4         net003.cnf
	PPPoE(USB�C�[�T�l�b�g)   Combination5         net004.cnf
	HDD�C�[�T�l�b�g          Combination6         net005.cnf
	PPPoE(HDD�C�[�T�l�b�g)   Combination7         net006.cnf

	�܂��{�v���O�����ł́A/usr/local/sce/conf/net �ȉ��ɂ��镽���̐ݒ�
	�t�@�C�����Q�Ƃ��Ă��܂��B����͖{�v���O�������J���T���v���p�r��
	���邽�߂ŁA�����̃^�C�g�����ł͌̕��������ꂽ�ݒ�t�@�C����
	�p����K�v�����鎖�ɒ��ӂ��ĉ������B
	�̕��������ꂽ�t�@�C���̗��p���@�ɂ��ẮA�ݒ�A�v���P�[�V����
	�T���v�����Q�Ƃ��ĉ������B

	���݂̐ڑ����ݒ�t�@�C���ł́A��̐ݒ�t�@�C���ɂ͕K�������
	�C���^�t�F�[�X���w�肳��Ȃ����߁A�{�v���O�����ł������̃f�o�C�X��
	�����ɗ��p���邱�Ƃ͑z�肵�Ă��܂���B
	�{�v���O�����́A�ړI�̃f�o�C�X��������ڑ�����Ă���󋵂�
	���삵�܂��B
	����f�o�C�X�̕����ڑ��Ȃǂ��l�����Ă��܂���̂ł����Ӊ������B

<�����T��>
	�����̊T���́A�ȉ��̒ʂ�ł��B

	1. inet �𗘗p���邽�߂̊e�탂�W���[���̃��[�h�Ainet �̏�����
	2. sceHTTPSetOption() �Ńm���u���b�L���O���w�肷��
	3. �ړI�� URL �ɑ΂��� HTTP �ڑ����AHTTP ���X�|���X���擾����
	4. HTTP ���X�|���X��\������
