[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                          �T���v���̍\��
========================================================================

�T���v���f�B���N�g���\��
------------------------------------------------------------------------
sce/iop/sample/
����hello
����kernel
�b  ����module
��  ����thread
����sif
�b  ����sifcmd
��  ����sifrpc
����spu2
    ����autodma
    ����seqplay
    ����stream
    ����voice


�T���v���ꗗ
------------------------------------------------------------------------
�T���v���R�[�h�ɂ͈ȉ��̂��̂�����܂��B

hello:
	hello		"hello !" ��\������T���v��
kernel:
	kernel/module	�풓���C�u�������W���[���̍쐬���@�T���v��
	kernel/thread	�X���b�h�̐����ƋN���A�X���b�h�̃v���C�I���e�B��
			����A�X���b�h�Ԃ̓������s���T���v��
sif:
	sif/sifcmd	SIF CMD �v���g�R���̃T���v��
	sif/sifrpc	SIF RPC �v���g�R���̃T���v��
spu2:
	spu2/autodma	�b��T�E���h���C�u�����ilibspu2�j ��AutoDMA�]��
			�@�\���g���āA�X�g���[�gPCM���͂ɂ�锭�����s��
			�T���v��
	spu2/seqplay	�b��T�E���h���C�u�����ilibspu2, libsnd2�j��
			�g���āAIOP���MIDI�V�[�P���X�̍Đ����s���T���v��
	spu2/stream	�b��T�E���h���C�u�����ilibspu2�j���g���āAIOP���
			SPU2�̃{�C�X�ɂ��X�g���[���Đ����s���T���v��
	spu2/voice	�b��T�E���h���C�u�����ilibspu2�j���g���āAIOP���
			SPU2�̃{�C�X�������s���T���v��


�T���v���R���p�C���̑O�ɍs���Ă�������
------------------------------------------------------------------------
sce/iop/lib �f�B���N�g���Ɉړ�����

 % make

�Ǝ��s���Ă��������B
�R���p�C���̃p�X�Ȃǂ��ݒ肳��܂��B


