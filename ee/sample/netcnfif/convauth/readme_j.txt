[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


���ʃl�b�g���[�N�ݒ�C���^�t�F�[�X���C�u�����T���v���v���O����(convauth)


< �T���v���̉�� >

	���̃v���O�����́A���Ȃ��̃l�b�g���[�N�ݒ�t�@�C���g�p�K���
	�g�p���K�肳��Ă��� sceNetcnfifConvAuthname() �֐��̎g�p���@��
	�������T���v���v���O�����ł��B


< �t�@�C�� >

	Makefile		�F ���C�N�t�@�C��
	convauth.c		�F ���C���v���O����


< �g�p���@ >

	1. make ���܂��B

	your-host > make


	2. �ȉ��̂悤�ɋN�����ĉ������B

	dsedb S> reset;run ./convauth.elf aaa


	3. �ȉ��̂悤�ɓ��͂��������񂪂��̂܂܏o�͂����� OK �ł��B

	EE DECI2 Manager version 0.06 Feb 19 2002 10:08:18
	  CPUID=2e14, BoardID=4126, ROMGEN=2002-0319, 32M

	Loading program (address=0x00100000 size=0x00008da4) ...
	Loading program (address=0x00108e00 size=0x00002550) ...
	Loading 593 symbols ...
	Entry address = 0x00100008
	GP value      = 0x00113370

	"aaa" -> "aaa"

	*** End of Program
	*** retval=0x00000000
