/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                       Timer Library
 *                        Version 1.0
 *                         Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         modname.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <string.h>
#include <eekernel.h>
#include <sceerrno.h>
#include <liberx.h>
#include <libtimer.h>

// �v���g�^�C�v�錾
int start(int reason, int argc, const char *const argv[]);

// ���W���[�����
const SceErxModuleInfo Module = {
	"libtimer",
	0x0203
};

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// ���W���[���N��(�A�����[�h�Ή�)
			{
				int i;
				int res;
				u_int mode = SCE_TIMER_PRESCALE256;

				// �I�v�V������]��
				for (i=1; i<argc; i++) {
					if (strcmp(argv[i], "-prescale256")==0) {
						mode = SCE_TIMER_PRESCALE256;		// '-prescale256'�I�v�V�����w��
					} else if (strcmp(argv[i], "-prescale16")==0) {
						mode = SCE_TIMER_PRESCALE16;		// '-prescale16'�I�v�V�����w��
					} else if (strcmp(argv[i], "-prescale1")==0) {
						mode = SCE_TIMER_PRESCALE1;			// '-prescale1'�I�v�V�����w��
					} else {
						scePrintf("Unknown option: %s\n", argv[i]);
					}
				}

				// libtimer���C�u������������
				res = sceTimerInit(mode);
				if (res<0) {
					return (SCE_ERX_NO_RESIDENT_END);
				}
			}
			return (SCE_ERX_REMOVABLE_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// ���W���[����~
			{
				int res;

				// libtimer���C�u�����̏I������
				res = sceTimerEnd();
				if (SCE_ERROR_ERRNO(res)==SCE_EBUSY) {
					// �܂��g�p���̃J�E���^���������ꍇ
					return (SCE_ERX_REMOVABLE_RESIDENT_END);
				}
			}
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// �s���ȗ��R�R�[�h�̂Ƃ�
	return (SCE_ERX_NO_RESIDENT_END);
}

