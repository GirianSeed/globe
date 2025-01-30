[SCEI CONFIDENTIAL DOCUMENT]
DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
                  Copyright (C) 1999 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

                                                          �T���v���̍\��
========================================================================

�T���v���f�B���N�g���\��
------------------------------------------------------------------------
sce/ee/sample/
����basic3d
��  ����core
��  ����vu0
��  ����vu1
����deci2
��  ����system
����device
��  ����file
��  ����pad
����graphics
��  ����capture
��  ����clip_vu0
��  ����clip_vu1
��  ����clut
��  ����clut_csa
��  ����jointm
�b  �b  ����intr_key
�b  �b      ����mkdata	
�b  �b      ����runtime
��  ����mipmap
��  ����refmap	
��  ����tex_swap
����ipu
��  ����ezcube
��  ����ezcube3	
��  ����ezmovie
����kernel
��  ����thread
����mpeg
��  ����ezmpeg
����pc
��  ����dcache
����pkt
��  ����balls
��  ����cube
��  ����mfifo
����sif
��  ����sifcmd
��  ����sifrp
����spu2
��  ����rautodma
��  ����rseqplay
��  ����rstream
��  ����rvoice
����vu0
��  ����tballs
��  ����tdiff
��  ����tfog
��  ����tmip
��  ����tskin
��  ����tsquare
����vu1
�@  ����blow
�@  ����bspline
�@  ����deform
�@  ����hako
�@  ����iga

�T���v���ꗗ
------------------------------------------------------------------------
�T���v���R�[�h�ɂ͈ȉ��̂��̂�����܂��B

basic3d:
        3D�̃I�u�W�F�N�g��\������܂ł́A��{�I�Ȏ葱���𗝉����邽�߂�
        �T���v��
        basic3d/core    cpu �̃R�A�̖��߂݂̂ŋL�q���ꂽ�A���S���Y��
                        ����p�T���v��
        basic3d/vu0     core �̃v���O���������ƂɃ��C���̏����� VU0 
                        �}�N�����߂ɒu���������T���v��
        basic3d/vu1     core�Avu0 �ōs���Ă��鏈�����A����� VU1 
                        �}�C�N�����߂Ŏ��������T���v��

deci2:
	deci2/system    DECI2 �v���g�R����p�����z�X�g��� system() ��
			���s����T���v��

device:
        device/file     �z�X�g�}�V���̃t�@�C������T���v��
        device/pad      �R���g���[���ǂݍ��݃T���v��

graphics:
        graphics/capture        �t���[���o�b�t�@���e���z�X�gPC��
                                �t�@�C���ɃZ�[�u����T���v��
	graphics/clip_vu0	Clipping �T���v��(VU0) 
	graphics/clip_vu1	Clipping �T���v��(VU1)
        graphics/clut           4bit/8bit�J���[�̃e�N�X�`����\������
				�T���v��
	graphics/clut_csa	4bit �e�N�X�`��CLUT�̈ꎞ�o�b�t�@�ւ�
				���[�h��csa/cld�t�B�[���h�ɂ���Đ��䂷��
				�T���v��
	graphics/jointm/intr_key/mkdata
				��]�����}�ɂ��L�[�t���[���}�g���N�X
				�A�j���[�V�����̃f�[�^�쐬�T���v��
	graphics/jointm/intr_key/runtime
				��]�����}�ɂ��L�[�t���[���}�g���N�X
				�A�j���[�V�����̃����^�C���T���v��
        graphics/mipmap         mipmap���s���T���v��
	graphics/refmap		�X�y�L�����[�̂���Ɠx�v�Z�A���t���N
				�V�����}�b�s���O���s���T���v��
	graphics/tex_swap	�e�N�X�`�������ւ��Ȃ���`����s��
				�T���v��

ipu:
	ipu/ezcube	3�����|���S���� IPU ��p���ăf�R�[�h����
			�e�N�X�`���𒣂�����T���v��
	ipu/ezcube3	�����̈��k�f�[�^���f�R�[�h����T���v��
        ipu/ezmovie     IPU ���g�p��������̊ȈՍĐ��T���v��

kernerl:
	kernel/thread	�}���`�X���b�h��p���ăI�u�W�F�N�g�̕`���
			�s�Ȃ��T���v��

mpeg:
	mpeg/ezmpeg	IPU ���g�p���� MPEG2(MPEG1���܂�)�X�g���[��
			�Đ��T���v��

pc:
	pc/dcache	Performance Counter ��p���� D Cache miss ��
			�J�E���g����T���v��

pkt:
        pkt/balls       �X�v���C�g�`��T���v��
        pkt/cube        �|���S�����f���`��T���v��
	pkt/mfifo	MFIFO��p���ăI�u�W�F�N�g�̕`����s���T���v��

sif:
	sif/sifcmd	SIF CMD �v���g�R���̃T���v��
	sif/sifrpc	SIF RPC �v���g�R���̃T���v��

spu2:
	spu2/rautodma	AutoDMA �]���@�\���g���āA�X�g���[�gPCM ���͂�
			��锭�����s�Ȃ��T���v��
	spu2/rseqplay	MIDI�V�[�P���X�Đ����s�Ȃ��T���v��
	spu2/rstream	�{�C�X�ɂ��X�g���[���Đ����s�Ȃ��T���v��
	spu2/rvoice	�{�C�X�̔������s�Ȃ��T���v��

vu0:
        VU0���g�p���ăW�I���g���������s�Ȃ��T���v��
        vu0/tballs      �R�����̋��̂����삷��T���v��
        vu0/tdiff       �|���S���̃N���b�v���s�Ȃ��T���v��
        vu0/tfog        �����Ȃ��_�����ƃI�u�W�F�N�g�N���b�v���s�Ȃ�
                        �T���v��
        vu0/tmip        MiMe�̐��`���}��MIPMODEL���s�Ȃ��T���v��
        vu0/tskin       �P�̃��f�����}�g���N�X���g�p���ĕ����\������
                        �T���v��
        vu0/tsquare     �C�����C���A�Z���u�����g�p����VU0�}�N���̃T���v��

vu1:
        VU1���g�p���ăW�I���g���������s�Ȃ��T���v��
        �}�C�N���R�[�h�̋L�q�ɂ�dvpasm�`�����g�p
        vu1/blow        �p�[�e�B�N���`��T���v��
        vu1/bspline     VU1 B-Spline�Ȗʕ`��T���v��
        vu1/deform      �Q�`���ԃT���v��
        vu1/hako        VU1�����ϊ��T���v��
        vu1/iga         VU1�����ϊ��A�����v�Z�T���v��

