[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

3�����|���S����IPU��p���ăf�R�[�h�����e�N�X�`���𒣂����T���v��

<�T���v���̉��>
	sample �f�B���N�g���̒��� pkt/cube �̃T���v����
	ipu/ezmovie �̃T���v�����e�N�X�`���Ƃ��Ē��������̂ł��B
	����Ɋւ��Ă�cube�̃T���v�����A����Ɋւ��Ă�ezmovie��
	�T���v�����Q�Ƃ��Ă��������B
	�{�T���v���ł�IPU�Ńf�R�[�h�����f�[�^���e�N�X�`���o�b�t�@��
	DMA�]�����A�e�N�X�`���Ƃ��ĕ\�����Ă��܂��B
		
<�t�@�C��>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	ez.dat
	ezmovie.h

<�N�����@>
	% make		�F�R���p�C��
	% make run	�F���s

	�R���p�C����A�ȉ��̕��@�ł����s�\
	% dsedb
	> run main.elf

<�R���g���[���̑�����@>
	��,���{�^��	�Fx����]
	��,���{�^��	�Fy����]
	L1,L2�{�^��	�Fz����]
	R1,R2�{�^��	�F�����̂�z�������̈ړ�
	��,�~�{�^��	�F�J������x����]
	��,���{�^��	�F�J������y����]
	start�{�^��	�F���Z�b�g

<���l>
	EB-2000, EB-2000S�ɂ�GS�ɃA���`�G�C���A�V���O�̃o�O������܂��B
        ���̂��߈ȉ���3�s�̃R�[�h��ǉ����܂����B

        /* GS�̃o�O�̂��߈ȉ���3�̃p�P�b�g��ǉ��œ]�� */
        sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(0));
        sceVif1PkAddGsAD(&packet, SCE_GS_PRMODE,
                         SCE_GS_SET_PRMODE(0,0,0,0,0,0,0,0));
        sceVif1PkAddGsAD(&packet, SCE_GS_PRMODECONT, SCE_GS_SET_PRMODECONT(1));

