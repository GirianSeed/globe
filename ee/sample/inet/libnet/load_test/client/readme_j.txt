[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libnet���C�u�������TCP��p���āA
�N���C�A���g-�T�[�o�ԂŃp�P�b�g�𑗎�M����T���v��
(�N���C�A���g�v���O����)


<�T���v���̉��>
	���̃v���O������ee����libnet��p���ăT�[�o��TCP�R�l�N�V������
	�m�����A�w��T�C�Y�̃f�[�^�𑗎�M����T���v���v���O�����ł��B
	�f�[�^�T�C�Y�Ɠ����N���X���b�h���������Ŏw��ł��܂��B
	�Ή�����v���O���������炩���߃T�[�o���ŋN������Ă���K�v��
	����܂��B

[ ���� ]
	���̃T���v���ł� USB �C�[�T�A�_�v�^���g�p����ꍇ�ɁAan986.irx ��
	�K�v�ł��B
	an986.irx �ɂ��ẮA�^�C�g�����ł̎g�p���֎~�Ƃ��Ă��邽��
	�i�ڍׂ� 2000/11/22 �t���� SCE-NET �́u�Z�p���v���Q�Ƃ��������j
	�����[�X�p�b�P�[�W�isce/iop/modules ���j�ɂ͊܂܂�Ă���܂���B
	SCE-NET �́u�_�E�����[�h�v->�u���̑����ʃc�[���v����_�E�����[�h
	���Ă��������B

<�t�@�C��>
        main.c        : ���C���v���O����


<�N�����@>
�܂��A�T�[�o�v���O�������T�[�o��ŋN�����܂��B
�T�[�o�v���O�����́Alinux��܂���ee��œ���2��ނ̃v���O����������܂��B
�e�X�̋N�����@�́A
/usr/local/sce/ee/sample/inet/load_test/linux_daemon/readme_j.txt
/usr/local/sce/ee/sample/inet/load_test/daemon/readme_j.txt
�����ꂼ��䗗�������B

�T�[�o���̏������ł�����A�N���C�A���g�v���O�������N�����܂��B

        $ make                                  : �R���p�C��
        $ dsedb					: dsedb���N��
        > run main.elf <saddr> <th_num> <pktsize>
                                                : main.elf�̎��s�B
        <saddr>: �T�[�o�A�h���X
        <th_num>: �����Ɋm������R�l�N�V������
        <pktsize>: ����M����f�[�^�T�C�Y

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

        1. ���[�J�X���b�h���N������
        2. �ʐM�̂��߂̏��������s�Ȃ�
        3. th_num�̒ʐM�X���b�h���N������
	�e�ʐM�X���b�h�ł�
        4. �T�[�o�ɑ΂���TCP�R�l�N�V�������m������
        5. ����M����f�[�^�T�C�Y�𑗂�
        6. �T�[�o��pktsize�o�C�g�̃f�[�^�𑗐M����
        7. �T�[�o����f�[�^����M����
        8. �f�[�^�����ꂩ�`�F�b�N����
        9. 1500�񑗎�M���J��Ԃ��ƏI��
