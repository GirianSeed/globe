[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5
		Copyright (C) 2001 Sony Computer Entertainment Inc.
						All Rights Reserved


	���ʃl�b�g���[�N�ݒ�p�A�C�R���t�@�C��


< ���e >

	- ���{��ŗp�A�C�R���t�@�C��

	japanese/icon.sys
	japanese/SYS_NET.ICO

	- �p��ŗp�A�C�R���t�@�C��

	english/icon.sys
	english/SYS_NET.ICO


< ���ӎ��� >

	�� ���ʃl�b�g���[�N�ݒ胉�C�u����(netcnf)���g�p���鎞�ɃA�C�R��
	   �t�@�C�����ȉ��̂悤�Ɏw�肵�Ă��g���������B

	   - dsidb �R���\�[�����N������ꍇ

	   dsidb R> mstart netcnf.irx icon=SYS_NET.ICO iconsys=icon.sys

	   - EE ���v���O������胍�[�h����ꍇ

	   {
	       static char netcnf_arg[] = "icon=host0:SYS_NET.ICO\0iconsys=host0:icon.sys";

	       while(sceSifLoadModule("host0:netcnf.irx", sizeof(netcnf_arg), netcnf_arg) < 0)
	           {
	               printf("Can't load module netcnf.irx\n");
	           }
	   }

	�� �p�ꌗ�n��ł͕K���p��ŗp�A�C�R���t�@�C�������g���������B
