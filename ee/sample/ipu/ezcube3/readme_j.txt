[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

�����̈��k�f�[�^���f�R�[�h����T���v��
<�T���v���̉��>
	�{�T���v���� sample �f�B���N�g���̒��� ipu/ezcube �̃T���v����
	�g���������̂ł��B
	�����̈��k���ꂽ�f�[�^�� IPU��1��1�f�R�[�h���A�e�N�X�`���o�b
	�t�@��DMA�]�����A�e�N�X�`���Ƃ��ĕ\�����Ă��܂��B
	�܂�IPU�����DMA�I�����荞�݂�p���āA1�̃f�[�^�̃f�R�[�h��
	�I����������Ɏ��̃f�R�[�h���s���Ă���A�����ǂ�IPU�𗘗p����
	���܂��B
	
<�t�@�C��>
	main.c
	ldimage.c
	ezcube.c
	ezcube.h
	ezmovie.h
	ez.dat
	rugby.dat
	movie3.dat
	456.dat
	1_6.dat
	wada.dat

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
