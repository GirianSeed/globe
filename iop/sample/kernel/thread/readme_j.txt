[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

�}���`�X���b�h�T���v��

<�T���v���̉��>

�X���b�h�̐����ƋN���A�X���b�h�̃v���C�I���e�B�̑���A�X���b�h�Ԃ̓�������
��{�I�ȃT���v���ł��B

<�t�@�C��>

	createth.c		�X���b�h�̐����ƋN�����@�̊�{�I�ȃT���v��
	sleepth.c		SleepThread()/WakeupThread() �̃T���v��
	eventth.c		SetEventFlag()/WaitEventFlag() �̃T���v��
	startthargs.c		�X���b�h�̋N���T���v��

<�N�����@>

	% make					�F�R���p�C��

    createth �̎��s

	�܂��A�ʂ̃E�C���h�E��
	% dsicons 
	�����s���Ă����܂��B

	% dsreset 0 2; dsistart createth.irx

	dsicons �����s�����E�C���h�E�� 6 �̃X���b�h�̋N���̗l�q��
	�\������܂��B

	�ȉ��̕��@�ł����s�\
	% dsidb
	> reset 0 2 ; mstart createth.irx


    sleepth �̎��s

	�܂��A�ʂ̃E�C���h�E��
	% dsicons 
	�����s���Ă����܂��B

	% dsreset 0 2; dsistart sleepth.irx

	dsicons �����s�����E�C���h�E�� '0..5,up,down > ' �Ƃ����v�����v�g
	���\������܂��B
	���̎��A�v�����v�g��\�����ē��͑҂��ɂȂ��Ă���R�}���h����
	�X���b�h�ƁA���̑��� 6 �̃X���b�h���N�����Ă��܂��B
	up ����͂��āARETURN �L�[����͂���ƁA�v�����v�g��\������
	���͑҂��ɂȂ��Ă���X���b�h�̃v���C�I���e�B���A���� 6 ��
	�X���b�h���������Ȃ�܂��B

	down ����͂��āARETURN �L�[����͂���ƁA�v�����v�g��\������
	���͑҂��ɂȂ��Ă���X���b�h�̃v���C�I���e�B���A���� 6 ��
	�X���b�h�����Ⴍ�Ȃ�܂��B

	0 ���� 5 �̐�������͂��āARETURN �L�[����͂���ƑΉ�����
	�X���b�h���N��(WakeupThread)�����܂��B

	�ȉ��̕��@�ł����s�\
	% dsidb
	> reset 0 2 ; mstart sleepth.irx


    eventth �̎��s

	�܂��A�ʂ̃E�C���h�E��
	% dsicons 
	�����s���Ă����܂��B

	% dsreset 0 2; dsistart eventth.irx

	dsicons �����s�����E�C���h�E�� '0..5,a,up,down > ' �Ƃ���
	�v�����v�g���\������܂��B
	���̎��A�v�����v�g��\�����ē��͑҂��ɂȂ��Ă���R�}���h����
	�X���b�h�ƁA���̑��� 6 �̃X���b�h���N�����Ă��܂��B
	up ����͂��āARETURN �L�[����͂���ƁA�v�����v�g��\������
	���͑҂��ɂȂ��Ă���X���b�h�̃v���C�I���e�B���A���� 6 ��
	�X���b�h���������Ȃ�܂�

	down ����͂��āARETURN �L�[����͂���ƁA�v�����v�g��\������
	���͑҂��ɂȂ��Ă���X���b�h�̃v���C�I���e�B���A���� 6 ��
	�X���b�h�����Ⴍ�Ȃ�܂��B

	0 ���� 5 �̐�������͂��āARETURN �L�[����͂���ƃC�x���g�t���O��
	�Ή�����r�b�g�𗧂Ă܂��B

	a ����͂��āARETURN �L�[����͂���ƃC�x���g�t���O�̑S���̃r�b�g
	�𗧂Ă܂��B

	�ȉ��̕��@�ł����s�\
	% dsidb
	> reset 0 2 ; mstart eventth.irx


    startthargs �̎��s

	�܂��A�ʂ̃E�C���h�E��
	% dsicons 
	�����s���Ă����܂��B

	% dsreset 0 2; dsistart startthargs.irx

	dsicons �����s�����E�C���h�E�� �X���b�h�̋N�������̗l�q��
	�\������܂��B

	�ȉ��̕��@�ł����s�\
	% dsidb
	> reset 0 2 ; mstart startthargs.irx
