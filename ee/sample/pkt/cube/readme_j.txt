[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

3�����|���S����\��������T���v��

<�T���v���̉��>
	���̃v���O�����͗����̂𓧎��ϊ���p���ĕ\�����A�R���g���[����
	��� x�� y�� z�����ꂼ��Ɨ��ɉ�]������T���v���v���O�����ł��B
	�܂��J������x�� y�����S�ɓ��������Ƃ��ł��܂��B

<�t�@�C��>
	main.c

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
	EB-2000��GS�ɃA���`�G�C���A�V���O�̃o�O������܂��B
	���̂��߈ȉ���3�s�̃R�[�h��ǉ����܂����B

	/* GS�̃o�O�̂��߈ȉ���3�̃p�P�b�g��ǉ��œ]�� */
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODECONT(0));
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODE(0, 0, 0, 0, 0, 0, 0, 0));
	sceVif1PkAddGsData(&packet, SCE_GS_SET_PRMODECONT(1));
