[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                   Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

voicesdr - �{�C�X�����̃T���v�� (libsdr �g�p)

<�T���v���̉��>
	libsdr �̃T���v���ł��BEE ���� IOP �o�R�ɂ� SPU2 �̃{�C�X����
	���s�Ȃ��܂��B
	�I�v�V�����ŁA�o�b�`�����A�]���I�����荞�݃n���h���A����� 
	SPU2 ���荞�݃n���h���̎������ł��܂��B

	�g�`�f�[�^�ł��� VAG �t�@�C����Ǝ��ɍ쐬����ꍇ�� AIFF2VAG 
	���g�p���Ă��������B

	SPU2 �ł̓R�A�̑I���ɒ��ӂ��K�v�ł��B�R�A 0 �̏o�͂̓R�A 1 ��
	���C���{�����[����ʂ�i�܂�R�A 1 �̃��C���{�����[�����[��
	�̏ꍇ�̓R�A 0 �̉�����������Ȃ��j���Ƃɒ��ӂ��Ă��������B

	BATCH_MODE �� 1 �ɐݒ肵�ăR���p�C�������ꍇ�A�o�b�`�����̃R�[
	�h���L���ɂȂ�܂��B
	����ɂ��A����܂œƗ��̊֐��Ăяo���ōs�Ȃ��Ă��������̏���
	���o�b�`�����ɂĈꊇ���čs�Ȃ��܂��B

	IRQ_CB_TEST �� 1 �ɐݒ肵�ăR���p�C������ƁASPU2 ���荞�݃n��
	�h����ݒ肷��R�[�h���L���ɂȂ�܂��B
	����ɂ��A�g�`�f�[�^�̐擪���� 0x1000 �o�C�g�iIRQ_ADDR_OFST�j
	����̔Ԓn�����������ɂ���ăA�N�Z�X���ꂽ���A"interrupt
	detected" �̃��b�Z�[�W���R���\�[���ɏo�͂���܂��B

	DMA_CB_TEST �� 1 �ɐݒ肵�ăR���p�C������ƁA�]���I�����荞��
	�n���h����ݒ肷��R�[�h���L���ɂȂ�܂��B
	VAG �t�@�C���̓]�����I���������� "interrupt detected" �̃��b�Z�[
	�W���R���\�[���ɏo�͂���܂��B

<�t�@�C��>
	main.c	

<�N�����@>
	% make		�F�R���p�C��
	% make run	�F���s

	Piano����1�I�N�^�[�u8���̉��K�����������ΐ���B

<�R���g���[���̑�����@>
	�Ȃ�

<���l>
	���̃T���v���� IOP ���Ńt�@�C���̓��e��ǂݍ��ފ֐� 
	sceSifLoadIopHeap() ���g�p���Ă��邽�߁A�t�@�C���ǂݍ��ݎ��̃t�@
	�C�����w��� IOP ���̎w��ł��� "host1:" ���g�p���Ă��܂��B
	�܂��Amake run ���ɂ� dsifilesv �ɂ���Ď��s�t�@�C�������s����
	�Ă��܂��B
