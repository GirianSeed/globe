[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

libnet���C�u������p�����Audp�p�P�b�g����������
�T�[�o�E�N���C�A���g�̃f�[�^�������s���T���v��


<�T���v���̉��>
        ���̃v���O�����́Alinux��œ��삷��T�[�o�[�v���O������
        �{�[���̋O���v�Z�E�Փ˂Ȃǂ̕������Z���s���A���̌��ʂ�
        �e"PlayStation 2"�N���C�A���g�v���O�����Ŏ�M���ĕ`�悷����̂ł�

[ ���� ]
	���̃T���v���ł� USB �C�[�T�A�_�v�^���g�p����ꍇ�ɁAan986.irx ��
	�K�v�ł��B
	an986.irx �ɂ��ẮA�^�C�g�����ł̎g�p���֎~�Ƃ��Ă��邽��
	�i�ڍׂ� 2000/11/22 �t���� SCE-NET �́u�Z�p���v���Q�Ƃ��������j
	�����[�X�p�b�P�[�W�isce/iop/modules ���j�ɂ͊܂܂�Ă���܂���B
	SCE-NET �́u�_�E�����[�h�v->�u���̑����ʃc�[���v����_�E�����[�h
	���Ă��������B

<�t�@�C��>

/usr/local/sce/ee/sample/inet/ball_game/ee:

        buff0.dsm       �F�g���v���o�b�t�@�̓]����ԂO��ݒ肷��p�P�b�g
        buff1.dsm       �F�g���v���o�b�t�@�̓]����ԂP��ݒ肷��p�P�b�g
        buff2.dsm       �F�g���v���o�b�t�@�̓]����ԂQ��ݒ肷��p�P�b�g
        grid.dsm        �F�n�ʂ̃��f��
        m_opt.vsm       �F�`��p�}�C�N���R�[�h �œK��
        main.c          �F���C���֐�
        mat.dsm         �F�n�ʂ̃e�N�X�`��
        mathfunc.c      �F�e�퐔�l���Z�֐�
        mathfunc.h      �F�e��p�����[�^���`
        matrix.dsm      �F�}�g���N�X�p�p�P�b�g�̃e���v���[�g
        no[1-4].dsm     �F�P�Ԃ���S�Ԃ̃e�N�X�`��
        packet.dsm      �Fpath1/path2�ɗ����p�P�b�g�f�[�^
        phys.c          �F�{�[���̈ʒu���v�Z���p�P�b�g���ɃZ�b�g����֐�
        sphere.dsm      �F�{�[���̃��f��
        main.h      	�F�T���v���̃p�����[�^��v���g�^�C�v�錾

/usr/local/sce/ee/sample/inet/ball_game/gamed:
        gamed.c         : �T�[�o�v���O�������C���֐�
        gamed.h         : �T�[�o�v���O�����w�b�_�t�@�C��
        ntohf.c         : �G���f�B�A���ϊ��p���[�e�B���e�B�֐�
        ntohf.h         : ���w�b�_�t�@�C��
        physics.c       : �O���v�Z�E�Փ˓��̕����v�Z����
        physics.h       : ���w�b�_�t�@�C��

/usr/local/sce/ee/sample/inet/ball_game/common:
        common.h        : �T�[�o�A�N���C�A���g�ŋ��ʂ̃w�b�_�t�@�C��

<�N�����@>
        $ make                          : �R���p�C��

�܂��T�[�o�v���O�����ł��� gamed ���N�����܂��Bgamed ��linux���
���삵�܂��B

        $ cd gamed; gamed               : linux��ŃT�[�o�v���O���������s

���ɁA�ʂ�terminal�ŃN���C�A���g�v���O�������N�����܂��B

        $ cd ee
        $ dsedb -r run main.elf <saddr>
                                        : �N���C�A���g�̎��s�B
                                          saddr �� gamed ���N�����Ă���
					  �T�[�o
        
�ȉ��A������̃N���C�A���g�ŃN���C�A���g�v���O���������s���閈�Ƀ{�[����
�ǉ�����܂��B

<�R���g���[���̑�����@>
        START �{�^�� : ���N���C�A���g�̃{�[���̍ė���

<���l>
	�{�v���O�����ł͐ڑ����Ƃ��āAUSB�C�[�T�l�b�g�AHDD �C�[�T�l�b�g
	��z�肵�Ă��܂��B
	�{�v���O�������̐擪�ɂ���ȉ��� #define ��؂�ւ��邱�Ƃɂ����
	�ڑ�����ύX���邱�Ƃ��\�ł��B
	
	#define USB_ETHERNET				: USB�C�[�T�l�b�g
	#define HDD_ETHERNET				: HDD�C�[�T�l�b�g

	IP�A�h���X�̕ύX�� PPP ���g�p����ꍇ�́A�e�ڑ����p�̐ݒ薼 
	"Combination?"(? �͐��l) �������ݒ�t�@�C����K�؂ɕύX���ĉ������B
	�Ȃ��A���L�����ꂼ��ɑΉ�����ݒ薼����ѐݒ�t�@�C���ł��B

	USB�C�[�T�l�b�g          Combination4         net003.cnf
	HDD�C�[�T�l�b�g          Combination6         net005.cnf

	�܂��{�v���O�����ł́A/usr/local/sce/conf/net �ȉ��ɂ��镽����
	�ݒ�t�@�C�����Q�Ƃ��Ă��܂��B����͖{�v���O�������J���T���v���p
	�r�ł��邽�߂ŁA�����̃^�C�g�����ł͌̕��������ꂽ�ݒ�t�@�C��
	��p����K�v�����鎖�ɒ��ӂ��ĉ������B
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

        [�N���C�A���g]
        1. �e�탂�W���[���̃��[�h�Ainet�̏�����
        2. �����[�g�|�[�g�ւ̐ڑ�
        3. �T�[�o�ւ̃|�W�V�������N�G�X�g
        4. start�������ꂽ�ꍇ�A�|�W�V�������Z�b�g��v�����T�[�o��
        5. �T�[�o����e�N���C�A���g�̃{�[���̏����󂯎��A
           ���ꂼ���`��B

        [�T�[�o]
        1. ���������A�N���C�A���g����̐ڑ���҂B
        2. �ڑ�����閈�ɃX���b�h���N�����āA�e�N���C�A���g�����
           �ʒu��񃊃N�G�X�g�ɔ�����B
        3. �ʒu��񃊃N�G�X�g����������A�N���C�A���g�Ɍ��݂�
           �ʒu���𑗐M����B
        4. �ڑ����ꂽ�S�N���C�A���g�Ƀ��N�G�X�g�𑗐M���閈�ɁA
        �@ �P�ʎ��Ԃ�i�߂Ĉʒu�̍Čv�Z���s���B


