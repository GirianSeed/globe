[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
                  Copyright (C) 2001 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libeenet ���C�u������p�����Ahttp�v���g�R���ɂ��
�����[�g�t�@�C�����擾����T���v��


<�T���v���̉��>
        ���̃v���O�����́Alibeenet ���C�u������p���āA
        �����[�g��http�|�[�g�ɐڑ����A
        �t�@�C���_�E�����[�h���s���T���v���v���O�����ł��B

<�t�@�C��>
        http.c        : ���C���v���O����

<�N�����@>
        $ make                          : �R���p�C��
        $ dsedb -r run http.elf [saddr] : ���s

        saddr��WWW�T�[�o�A�h���X�ł�(www.scei.co.jp��)�B
        ���s����ƁAHTTP�𗘗p���ăt�@�C�� http://<saddr>/index.html ��
        �擾���s�Ȃ��A���[�J���ɃZ�[�u���܂��B

<�R���g���[���̑�����@>
        �Ȃ�

<���l>
	�{�v���O�����ł͐ڑ����Ƃ��āADHCP(USB�C�[�T�l�b�g)�A
	DHCP(HDD�C�[�T�l�b�g)�APPPoE(USB�C�[�T�l�b�g)�APPPoE(HDD�C�[�T
	�l�b�g)�APPP(���f��)��z�肵�Ă��܂��B
	�{�v���O�������̐擪�ɂ���ȉ��� #define ��؂�ւ��邱�Ƃɂ����
	�ڑ�����ύX���邱�Ƃ��\�ł��B

	#define HDD_ETHERNET			: DHCP(HDD�C�[�T�l�b�g)
	#define HDD_ETHERNET_WITH_PPPOE		: PPPoE(HDD�C�[�T�l�b�g)
	#define USB_ETHERNET			: DHCP(USB�C�[�T�l�b�g)
	#define USB_ETHERNET_WITH_PPPOE		: PPPoE(USB�C�[�T�l�b�g)
	#define MODEM				: PPP(���f��)

	PPP(���f��)���g�p����ꍇ�́Aiopmodules.h �� NULL ������ɂȂ�
	�Ă���ȉ��� #define 

	#define USE_CONF_NAME		""	: net.db ���̐ݒ薼
	#define IOP_MOD_MODEMDRV        ""	: ���W���[���p�X��
	#define MODEMDRV_ARG            ""	: ����

	���APPP�ڑ�����g�p���郂�f���ɍ��킹�ĕύX���Ă��������B
	USB ���f�����g�p����ꍇ�́A��L�Ŏw�肷�郂�W���[���ȊO��
	USBD ���W���[�������[�h����K�v������܂��B���̑��A�g�p���郂
	�f���̎�ނɉ����Ēǉ��̃��W���[���̃��[�h���K�v�ȏꍇ�́A�K�X
	http.c ��ύX���Ă��������B

	IP�A�h���X�̕ύX��PPPoE �̃��[�U���ύX���s�Ȃ�����PPP ���g�p����
	�ꍇ�́A�e�ڑ����p�̐ݒ薼 "Combination?"(? �͐��l) �������ݒ�
	�t�@�C����K�؂ɕύX���ĉ������B
	�Ȃ��A���L�����ꂼ��ɑΉ�����ݒ薼����ѐݒ�t�@�C���ł��B

	DHCP(HDD�C�[�T�l�b�g)    Combination6         net005.cnf
	PPPoE(HDD�C�[�T�l�b�g)   Combination7         net006.cnf
	DHCP(USB�C�[�T�l�b�g)    Combination4         net003.cnf
	PPPoE(USB�C�[�T�l�b�g)   Combination5         net004.cnf
	PPP(���f��)		 Combination2	      net001.cnf

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

        1. libeenet �̏������A�e�탂�W���[���̃��[�h
        2. �����[�g�|�[�g�ւ̐ڑ�
        3. GET���b�Z�[�W�̑��M
        4. �f�[�^�̎�M
	5. �e�탂�W���[���̃A�����[�h
	6. libeenet �̏I������
