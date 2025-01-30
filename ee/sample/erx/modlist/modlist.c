/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ERX Library Sample
 *
 *                     - <liberx modlist> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <modlist.c>
 *                         <module list>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,15,2003     kono        first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <liberx.h>

// ���W���[�����
const SceErxModuleInfo Module = {
	"modlist",
	0x0201
};


int start(int reason, int argc, const char *const argv[]);
void EnumModules(void);
void DumpModuleStatus(int modid);

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// ���W���[���N��
			EnumModules();
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// ���W���[����~
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// �s���ȗ��R�R�[�h�̂Ƃ�
	return (SCE_ERX_NO_RESIDENT_END);
}



// ���W���[���̗�
void EnumModules(void)
{
	int modids[1024];
	int cnt;
	int i;

	cnt = sceErxGetModuleIdList(modids, 1024, NULL);
	printf("sceErxGetModuleIdList: %d\n", cnt);

	for (i=0; i<cnt; i++) {
		DumpModuleStatus(modids[i]);
	}
	return;
}



// ���W���[���̃X�e�[�^�X�\��
void DumpModuleStatus(int modid)
{
	int res;
	SceErxModuleStatus modstat;

	// �w�肳�ꂽ���W���[��ID�̏�Ԃ��擾
	res = sceErxReferModuleStatus(modid, &modstat);

	printf("-----\n");
	printf("id:            %d\n",   modstat.id);			// ���W���[�������ʂ���ID�ԍ�
	printf("name:          %s\n",   modstat.name);			// ���W���[����
	printf("version:       %04X\n", modstat.version);		// ���W���[���̃o�[�W�����ԍ�
	printf("entry_addr:    %08X\n", modstat.entry_addr);	// ���W���[���̎��s�J�n�A�h���X
	printf("gp_value:      %08X\n", modstat.gp_value);		// ���W���[���� GP ���W�X�^�l
	printf("text_addr:     %08X\n", modstat.text_addr);		// ���W���[���̃e�L�X�g�擪�A�h���X
	printf("text_size:     %08X\n", modstat.text_size);		// ���W���[���̃e�L�X�g�Z�O�����g�T�C�Y
	printf("data_size:     %08X\n", modstat.data_size);		// ���W���[���̃f�[�^�Z�O�����g�T�C�Y
	printf("bss_size:      %08X\n", modstat.bss_size);		// ���W���[���̖��������f�[�^�Z�O�����g�T�C�Y
	printf("erx_lib_addr:  %08X\n", modstat.erx_lib_addr);	// ERX���C�u�����G���g���̊J�n�A�h���X
	printf("erx_lib_size:  %08X\n", modstat.erx_lib_size); 	// ERX���C�u�����G���g���̃T�C�Y
	printf("erx_stub_addr: %08X\n", modstat.erx_stub_addr);	// ERX���C�u�����X�^�u�̊J�n�A�h���X
	printf("erx_stub_size: %08X\n", modstat.erx_stub_size);	// ERX���C�u�����X�^�u�̃T�C�Y
	return;
}
