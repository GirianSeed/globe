[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
	                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                          All Rights Reserved

	�`�s�n�j���C�u���� �T���v���v���O����


< �T���v���̉�� >

	���̃v���O�����́A�`�s�n�j���C�u����(libatok)�̎g�p���������
	�T���v���v���O�����ł��B

	�`�s�n�j���C�u������������Ƃ��āA
	"PlayStation 2"��p �n�[�h�f�B�X�N�h���C�u���j�b�g���A�ڑ�����Ă���A
	�V�X�e���\�t�g�E�F�A�̃C���X�g�[�����s���Ă���K�v������܂��B

	�܂��A�ȉ��̃p�b�P�[�W���v���O�������Ŏg�p���Ă��܂��̂� SCE-NET ���
	���ꂼ��_�E�����[�h���ăJ�����g�f�B���N�g���ɓW�J���Ă��������B

	- Shift JIS <-> UTF8 �����R�[�h�ϊ����C�u�����p�b�P�[�W (libccc )
	- �ȈՃt�H���g���C�u�����p�b�P�[�W (libpfont)


< �t�@�C�� >

	Makefile         �F ���C�N�t�@�C��
	main.c           �F ���C���v���O����
	pad.c            �F �R���g���[���֘A�v���O����
	pad.h            �F �R���g���[���֘A�w�b�_
	kbd.c            �F USB�L�[�{�[�h�֘A�v���O����
	kbd.h            �F USB�L�[�{�[�h�֘A�w�b�_

	skb/ �ȉ�        �F ATOK���C�u�������g�p�����A
	                    �\�t�g�E�F�A�L�[�{�[�h�֘A�v���O�����B
	                    ( libpfont ���g�p���Ă��܂��B)
	                    �ڍׂ́Askb/readme_j.txt ���Q�Ɖ������B



< Shift JIS <-> UTF8 �����R�[�h�ϊ����C�u�����p�b�P�[�W�ilibccc�j >
	���̃v���O������ Shift JIS �� UTF8 �̕����R�[�h�ϊ����C�u�����Ƃ��ĕʓr
	SCE-NET��胊���[�X����Ă��� Shift JIS <-> UTF8 �����R�[�h�ϊ����C
	�u�����p�b�P�[�W���g�p���Ă��܂��Bmake���ɕK�v�ƂȂ�܂��̂œK�X
	SCE-NET ���_�E�����[�h���A�T���v���v���O�����f�B���N�g���ɓW�J����
	�������B

< �ȈՃt�H���g���C�u�����p�b�P�[�W (libpfont)  >
	���̃v���O�����͊ȈՃt�H���g���C�u����(libpfont) �Ƃ��ĕʓr
	SCE-NET��胊���[�X����Ă��郉�C�u�����p�b�P�[�W���g�p���Ă��܂��B
	make���ɕK�v�ƂȂ�܂��̂œK�X SCE-NET ���_�E�����[�h���A
	�T���v���v���O�����f�B���N�g���ɓW�J���ĉ������B


< �N�����@ >
	% make clean �F�N���[��
	% make       �F�R���p�C��
	% make run   �F���s


< �R���g���[���̑�����@ >

	���{�^���F�L�[�{�[�h�I�[�v��
	�~�{�^��      �F�L�����Z���I���B
	�X�^�[�g�{�^���F����I���B

	���̑��A����ڍׂɂ��ẮAskb/readme_j.txt ���A�Q�Ƃ��������B


< ���������E���ӎ��� >

	- ���̃v���O�����ł́A�\�[�X���� Shift JIS �̕�������܂��
	  ���邽�߁A���ϐ� LANG �̉e�����󂯂�ꍇ������܂��̂�
	  �����ӂ��������B

	  ���̃v���O�������g�p�����ۂ́A���ϐ� LANG �� Shift JIS ��
	  �w�肵�Ă��������B

	 .bashrc �̏ꍇ�F 
	 $ export LANG=C-SJIS

	 .cshrc �̏ꍇ�F
	 $ setenv LANG C-SJIS

