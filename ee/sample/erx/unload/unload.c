/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ERX Library Sample
 *
 *                     - <liberx unload> -
 *
 *                         Version 1.01
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <unload.c>
 *                       <module unloader>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,15,2003     kono        first version
 *       1.01           May,19,2003     kono        add error strings
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <liberx.h>
#include <sceerrno.h>

// ���W���[�����
const SceErxModuleInfo Module = {
	"Module_Unloader",
	0x0201
};

static const char szTitle[] = "ERX moodule unloader utility\n";
static const char szUsage[] = "usage: unload.erx [-s] <module id>|<module name>\n";

int start(int reason, int argc, const char *const argv[]);
void UnloadModule(int argc, const char *const argv[]);

void DumpErrCode(int err);
static const char *sceErrorGetLibraryName(int err);
static const char *sceErrorGetErrorName(int err);

int start(int reason, int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	switch (reason) {
		case SCE_ERX_REASON_START:
			// ���W���[���N��
			UnloadModule(argc, argv);
			return (SCE_ERX_NO_RESIDENT_END);

		case SCE_ERX_REASON_STOP:
			// ���W���[����~
			return (SCE_ERX_NO_RESIDENT_END);
	}

	// �s���ȗ��R�R�[�h�̂Ƃ�
	return (SCE_ERX_NO_RESIDENT_END);
}



// ���W���[���̒�~����уA�����[�h
void UnloadModule(int argc, const char *const argv[])
{
	int i;
	int res;
	int sopt;
	int modid;
	const char *pszModName;
	SceErxModuleStatus modstat;

	sopt = 0;
	modid = -1;
	pszModName = NULL;
	for (i=1; i<argc; i++) {
		char c;
		c = argv[i][0];
		if (c>='0' && c<='9') {
			// ���l���w�肳�ꂽ�Ƃ�
			modid = atoi(argv[i]);
			break;
		} else if (c=='/' || c=='-') {
			// �I�v�V����
			switch (argv[i][1]) {
				case 'S':
				case 's':
					// -s�I�v�V����(��~�̂�)
					sopt = 1;
					break;
			}
		} else {
			// ����ȊO�̕����񂪎w�肳�ꂽ�Ƃ�
			pszModName = argv[i];
			break;
		}
	}

	printf(szTitle);
	if (modid==-1 && pszModName==NULL) {
		// �g�p���@��\�����ďI��
		printf(szUsage);
		return;
	}

	if (pszModName!=NULL) {
		// ������Ń��W���[�������w�肳�ꂽ�Ƃ�
		modid = sceErxSearchModuleByName(pszModName);

		if (modid < 0) {
			// ���W���[����������Ȃ�����
			printf("Cant't find '%s' module id\n", pszModName);
			DumpErrCode(modid);
			return;
		}
	}

	// ��~���悤�Ƃ��Ă��郂�W���[���̃X�e�[�^�X���擾
	res = sceErxReferModuleStatus(modid, &modstat);
	if (res < 0) {
		printf("Cant't refer module status(modid=%d)\n", modid);
		DumpErrCode(modid);
		return;
	}

	// ���W���[�����~
	res = sceErxStopModule(modid, 0, NULL, &i);
	printf("Stopping module:  modid=%d name='%s'(%d.%d)\n", modid, modstat.name, (modstat.version>>8), (modstat.version & 0xFF));
	DumpErrCode(res);
	if (res<0 && SCE_ERROR_ERRNO(res)==SCE_ENOT_REMOVABLE) {
		// �A�����[�h���T�|�[�g���Ă��Ȃ����W���[���������Ƃ�
		return;
	}
	if (sopt!=0) {
		// -s�I�v�V�������w�肳��Ă����Ƃ�
		return;
	}

	// �A�����[�h����
	res = sceErxUnloadModule(modid);
	printf("Unloading module: modid=%d name='%s'(%d.%d)\n", modid, modstat.name, (modstat.version>>8), (modstat.version & 0xFF));
	DumpErrCode(res);
	return;
}



// liberx�̃G���[�R�[�h���_���v�\��
void DumpErrCode(int err)
{
	if (err==SCE_OK) {
		// �����R�[�h
		printf("SUCCESS(SCE_OK)\n");
	} else if (err >= 0) {
		// ���Ԃ񐬌��R�[�h
		printf("maybe SUCCESS(%d)\n", err);
	} else {
		// �G���[����
		const char *pszLibName   = sceErrorGetLibraryName(err);
		const char *pszErrorName = sceErrorGetErrorName(err);

		printf("ERROR library=0x%04X(%s), code=0x%04X(%s)\n",
							SCE_ERROR_LIB_PREFIX(err), pszLibName,
							SCE_ERROR_ERRNO(err), pszErrorName);
	}
	return;
}




// �G���[�R�[�h���烉�C�u���������擾
static const char *sceErrorGetLibraryName(int err)
{
	static const struct {
		int uiPrefix;
		const char *pszName;
	} libname[] = {
		{SCE_ERROR_PREFIX_MC2,		"libmc2"},
		{SCE_ERROR_PREFIX_BNNETCNF,	"bnnetcnf"},
		{SCE_ERROR_PREFIX_SDHD,		"libsdhd"},
		{SCE_ERROR_PREFIX_SDSQ,		"sdsq"},
		{SCE_ERROR_PREFIX_ERX,		"liberx"},
		{SCE_ERROR_PREFIX_TIMER,	"libtimer"},
		{0, NULL}
	};
	int i;

	if (err>=0) {
		return ("");
	}
	i = 0;
	while (libname[i].pszName!=NULL) {
		if (SCE_ERROR_LIB_PREFIX(err)==libname[i].uiPrefix) {
			return (libname[i].pszName);
		}
		i++;
	}
	return ("unknown");
}


// �G���[�R�[�h�̕������Ԃ�
static const char *sceErrorGetErrorName(int err)
{
	static const struct {
		int num;
		const char *pszName;
	} libname[] = {
		{SCE_OK,				"SCE_OK"},					// ����

		{SCE_EINIT,				"SCE_EINIT"},				// �������������s���Ă��Ȃ�
		{SCE_EID,				"SCE_EID"},					// ID �����݂��Ȃ�
		{SCE_ESEMAPHORE,		"SCE_ESEMAPHORE"},			// �Z�}�t�H�������s���Ȃ�
		{SCE_ETHREAD,			"SCE_ETHREAD"},				// �X���b�h�������s���Ȃ�
		{SCE_ETIMER,			"SCE_ETIMER"},				// �^�C�}�[/�A���[���������s���Ȃ�
		{SCE_EHANDLER,			"SCE_EHANDLER"},			// �n���h���o�^�������s���Ȃ�
		{SCE_EILLEGAL_CONTEXT,	"SCE_EILLEGAL_CONTEXT"},	// ��O�E���荞�݃n���h������̌Ăяo��
		{SCE_ECPUDI,			"SCE_ECPUDI"},				// ���Ɋ��荞�݋֎~������

		{SCE_EDEVICE_BROKEN,	"SCE_EDEVICE_BROKEN"},		// �f�o�C�X�j���̉\��
		{SCE_EFILE_BROKEN,		"SCE_EFILE_BROKEN"},		// �t�@�C���܂��̓f�B���N�g���j���̉\��
		{SCE_ENEW_DEVICE,		"SCE_ENEW_DEVICE"},			// �V�K�f�o�C�X���o
		{SCE_EMDEPTH,			"SCE_EMDEPTH"},				// �f�B���N�g�����[�߂���
		{SCE_ENO_PROGRAM,		"SCE_ENO_PROGRAM"},			// �v���O�����`�����N������
		{SCE_ENO_SAMPLESET,		"SCE_ENO_SAMPLESET"},		// �T���v���Z�b�g�`�����N������
		{SCE_ENO_SAMPLE,		"SCE_ENO_SAMPLE"},			// �T���v���`�����N������
		{SCE_ENO_VAGINFO,		"SCE_ENO_VAGINFO"},			// VAGInfo�`�����N������
		{SCE_ENO_SBADDR,		"SCE_ENO_SBADDR"},			// �X�v���b�g�u���b�N�̃A�h���X��񂪖���
		{SCE_EBAD_PNUM,			"SCE_EBAD_PNUM"},			// �v���O�����i���o�[�͔͈͊O
		{SCE_ENO_PNUM,			"SCE_ENO_PNUM"},			// �v���O�����i���o�[�͖��g�p
		{SCE_EBAD_SSNUM,		"SCE_EBAD_SSNUM"},			// �T���v���Z�b�g�i���o�[�͔͈͊O
		{SCE_ENO_SSNUM,			"SCE_ENO_SSNUM"},			// �T���v���Z�b�g�i���o�[�͖��g�p
		{SCE_EBAD_SPNUM,		"SCE_EBAD_SPNUM"},			// �T���v���i���o�[�͔͈͊O
		{SCE_ENO_SPNUM,			"SCE_ENO_SPNUM"},			// �T���v���i���o�[�͖��g�p
		{SCE_EBAD_VAGNUM,		"SCE_EBAD_VAGNUM"},			// VAGInfo�i���o�[�͔͈͊O
		{SCE_ENO_VAGNUM,		"SCE_ENO_VAGNUM"},			// VAGInfo�i���o�[�͖��g�p
		{SCE_EBAD_SBNUM,		"SCE_EBAD_SBNUM"},			// �X�v���b�g�u���b�N�i���o�[�͔͈͊O
		{SCE_EVAGINFO_NOISE,	"SCE_EVAGINFO_NOISE"},		// VAGInfo���w������VAG�̓m�C�Y�ł���
		{SCE_ENO_SPLITNUM,		"SCE_ENO_SPLITNUM"},		// �X�v���b�g�i���o�[�͖��g�p

		{SCE_EINT_HANDLER,		"SCE_EINT_HANDLER"}, 		// ���荞�݃n���h���������s��
		{SCE_EDMA_HANDLER,		"SCE_EDMA_HANDLER"},		// DMA ���荞�݃n���h���������s��

		{SCE_ENO_MIDI,			"SCE_ENO_MIDI"},			// Midi�`�����N������
		{SCE_ENO_SONG,			"SCE_ENO_SONG"},			// Song�`�����N������
		{SCE_ENO_MIDINUM,		"SCE_ENO_MIDINUM"},			// Midi�f�[�^�u���b�N�i���o�[�͖��g�p
		{SCE_ENO_SONGNUM,		"SCE_ENO_SONGNUM"},			// Song�i���o�[�͖��g�p
		{SCE_ENO_COMPTABLE,		"SCE_ENO_COMPTABLE"},			// ���k�e�[�u��������
		{SCE_EBAD_COMPTABLEINDEX, "SCE_EBAD_COMPTABLEINDEX"},	// ���k�e�[�u���C���f�b�N�X�͔͈͊O
		{SCE_EBAD_POLYKEYDATA,	"SCE_EBAD_POLYKEYDATA"},	// �|���t�H�j�b�N�L�[�v���b�V���[�͕s��

		{SCE_ELINKERROR,		"SCE_ELINKERROR"},			// ���[�h�������W���[�����K�v�Ƃ���풓���C�u���������݂��Ȃ�
		{SCE_EILLEGAL_OBJECT,	"SCE_EILLEGAL_OBJECT"},		// �I�u�W�F�N�g�t�@�C���̌`�����������Ȃ�
		{SCE_EUNKNOWN_MODULE, 	"SCE_EUNKNOWN_MODULE"},		// �w�肵�����W���[����������Ȃ�
		{SCE_EMEMINUSE,			"SCE_EMEMINUSE"},			// �w�肵���A�h���X�͊��Ɏg�p��
		{SCE_EALREADY_STARTED,	"SCE_EALREADY_STARTED"},	// �w�肵�����W���[���͊��ɃX�^�[�g���Ă���
		{SCE_ENOT_STARTED,		"SCE_ENOT_STARTED"},		// �w�肵�����W���[���̓X�^�[�g���Ă��Ȃ�
		{SCE_EALREADY_STOPPING,	"SCE_EALREADY_STOPPING"},	// �w�肵�����W���[���̓X�g�b�v������
		{SCE_EALREADY_STOPED,	"SCE_EALREADY_STOPED"},		// �w�肵�����W���[���͊��ɃX�g�b�v���Ă���
		{SCE_ENOT_STOPPED,		"SCE_ENOT_STOPPED"},		// �w�肵�����W���[���̓X�g�b�v���Ă��Ȃ�
		{SCE_ECAN_NOT_STOP,		"SCE_ECAN_NOT_STOP"},		// ���W���[���̒�~�͏o���Ȃ�����
		{SCE_ENOT_REMOVABLE,	"SCE_ENOT_REMOVABLE"},		// �w�肵�����W���[���͍폜�\�ł͂Ȃ�
		{SCE_ELIBRARY_FOUND, 	"SCE_ELIBRARY_FOUND"},		// ���C�u�����͊��ɓo�^����Ă���
		{SCE_ELIBRARY_NOTFOUND, "SCE_ELIBRARY_NOTFOUND"},	// ���C�u�����͓o�^����Ă��Ȃ�
		{SCE_ELIBRARY_INUSE,	"SCE_ELIBRARY_INUSE"},		// ���C�u�����͎g�p��
		{SCE_EILLEGAL_LIBRARY, 	"SCE_EILLEGAL_LIBRARY"},	// ���C�u�����w�b�_���ُ�


		// ���̑��A�W���I��errno
		{EPERM,					"EPERM"},					//Not super-user
		{ENOENT,				"ENOENT"},					// No such file or directory
		{ESRCH,					"ESRCH"},					// No such process
		{EINTR,					"EINTR"},					// Interrupted system call
		{EIO,					"EIO"},						// I/O error
		{ENXIO,					"ENXIO"},					// No such device or address
		{E2BIG,					"E2BIG"},					// Arg list too long
		{ENOEXEC,				"ENOEXEC"},					// Exec format error
		{EBADF,					"EBADF"},					// Bad file number
		{ECHILD,				"ECHILD"},					// No children
		{EAGAIN,				"EAGAIN"},					// No more processes
		{ENOMEM,				"ENOMEM"},					// Not enough core
		{EACCES,				"EACCES"},					// Permission denied
		{EFAULT,				"EFAULT"},					// Bad address
		{ENOTBLK,				"ENOTBLK"},					// Block device required
		{EBUSY,					"EBUSY"},					// Mount device busy
		{EEXIST,				"EEXIST"},					// File exists
		{EXDEV,					"EXDEV"},					// Cross-device link
		{ENODEV,				"ENODEV"},					// No such device
		{ENOTDIR,				"ENOTDIR"},					// Not a directory
		{EISDIR,				"EISDIR"},					// Is a directory
		{EINVAL,				"EINVAL"},					// Invalid argument
		{ENFILE,				"ENFILE"},					// Too many open files in system
		{EMFILE,				"EMFILE"},					// Too many open files
		{ENOTTY,				"ENOTTY"},					// Not a typewriter
		{ETXTBSY,				"ETXTBSY"},					// Text file busy
		{EFBIG,					"EFBIG"},					// File too large
		{ENOSPC,				"ENOSPC"},					// No space left on device
		{ESPIPE,				"ESPIPE"},					// Illegal seek
		{EROFS,					"EROFS"},					// Read only file system
		{EMLINK,				"EMLINK"},					// Too many links
		{EPIPE,					"EPIPE"},					// Broken pipe
		{EDOM,					"EDOM"},					// Math arg out of domain of func
		{ERANGE,				"ERANGE"},					// Math result not representable
		{ENOMSG,				"ENOMSG"},					// No message of desired type
		{EIDRM,					"EIDRM"},					// Identifier removed
		{ECHRNG,				"ECHRNG"},					// Channel number out of range
		{EL2NSYNC,				"EL2NSYNC"},				// Level 2 not synchronized
		{EL3HLT,				"EL3HLT"},					// Level 3 halted
		{EL3RST,				"EL3RST"},					// Level 3 reset
		{ELNRNG,				"ELNRNG"},					// Link number out of range
		{EUNATCH,				"EUNATCH"},					// Protocol driver not attached
		{ENOCSI,				"ENOCSI"},					// No CSI structure available
		{EL2HLT,				"EL2HLT"},					// Level 2 halted
		{EDEADLK,				"EDEADLK"},					// Deadlock condition
		{ENOLCK,				"ENOLCK"},					// No record locks available
		{EBADE,					"EBADE"},					// Invalid exchange
		{EBADR,					"EBADR"},					// Invalid request descriptor
		{EXFULL,				"EXFULL"},					// Exchange full
		{ENOANO,				"ENOANO"},					// No anode
		{EBADRQC,				"EBADRQC"},					// Invalid request code
		{EBADSLT,				"EBADSLT"},					// Invalid slot
		{EDEADLOCK,				"EDEADLOCK"},				// File locking deadlock error
		{EBFONT,				"EBFONT"},					// Bad font file fmt
		{ENOSTR,				"ENOSTR"},					// Device not a stream
		{ENODATA,				"ENODATA"},					// No data (for no delay io)
		{ETIME,					"ETIME"},					// Timer expired
		{ENOSR,					"ENOSR"},					// Out of streams resources
		{ENONET,				"ENONET"},					// Machine is not on the network
		{ENOPKG,				"ENOPKG"},					// Package not installed
		{EREMOTE,				"EREMOTE"},					// The object is remote
		{ENOLINK,				"ENOLINK"},					// The link has been severed
		{EADV,					"EADV"},					// Advertise error
		{ESRMNT,				"ESRMNT"},					// Srmount error
		{ECOMM,					"ECOMM"},					// Communication error on send
		{EPROTO,				"EPROTO"},					// Protocol error
		{EMULTIHOP,				"EMULTIHOP"},				// Multihop attempted
		{ELBIN,					"ELBIN"},					// Inode is remote (not really error)
		{EDOTDOT,				"EDOTDOT"},					// Cross mount point (not really error)
		{EBADMSG,				"EBADMSG"},					// Trying to read unreadable message
		{ENOTUNIQ,				"ENOTUNIQ"},				// Given log. name not unique
		{EBADFD,				"EBADFD"},					// f.d. invalid for this operation
		{EREMCHG,				"EREMCHG"},					// Remote address changed
		{ELIBACC,				"ELIBACC"},					// Can't access a needed shared lib
		{ELIBBAD,				"ELIBBAD"},					// Accessing a corrupted shared lib
		{ELIBSCN,				"ELIBSCN"},					// .lib section in a.out corrupted
		{ELIBMAX,				"ELIBMAX"},					// Attempting to link in too many libs
		{ELIBEXEC,				"ELIBEXEC"},				// Attempting to exec a shared library
		{ENOSYS,				"ENOSYS"},					// Function not implemented
		{ENMFILE,		 		"ENMFILE"},					// No more files
		{ENOTEMPTY,				"ENOTEMPTY"},				// Directory not empty
		{ENAMETOOLONG,			"ENAMETOOLONG"},			// File or path name too long
		{ELOOP,					"ELOOP"},					// Too many symbolic links
		{EOPNOTSUPP,			"EOPNOTSUPP"},				// Operation not supported on transport endpoint
		{EPFNOSUPPORT,			"EPFNOSUPPORT"},			// Protocol family not supported
		{ECONNRESET,			"ECONNRESET"},			 	// Connection reset by peer
		{ENOBUFS,				"ENOBUFS"},					// No buffer space available
		{EAFNOSUPPORT,			"EAFNOSUPPORT"},			// Address family not supported by protocol family
		{EPROTOTYPE,			"EPROTOTYPE"},				// Protocol wrong type for socket
		{ENOTSOCK,				"ENOTSOCK"},				// Socket operation on non-socket
		{ENOPROTOOPT,			"ENOPROTOOPT"},				// Protocol not available
		{ESHUTDOWN,				"ESHUTDOWN"},				// Can't send after socket shutdown
		{ECONNREFUSED,			"ECONNREFUSED"},			// Connection refused
		{EADDRINUSE,			"EADDRINUSE"},				// Address already in use
		{ECONNABORTED,			"ECONNABORTED"},			// Connection aborted
		{ENETUNREACH,			"ENETUNREACH"},				// Network is unreachable
		{ENETDOWN,				"ENETDOWN"},				// Network interface is not configured
		{ETIMEDOUT,				"ETIMEDOUT"},				// Connection timed out
		{EHOSTDOWN,				"EHOSTDOWN"},				// Host is down
		{EHOSTUNREACH,			"EHOSTUNREACH"},			// Host is unreachable
		{EINPROGRESS,			"EINPROGRESS"},				// Connection already in progress
		{EALREADY,				"EALREADY"},				// Socket already connected
		{EDESTADDRREQ,			"EDESTADDRREQ"},			// Destination address required
		{EMSGSIZE,				"EMSGSIZE"},				// Message too long
		{EPROTONOSUPPORT,		"EPROTONOSUPPORT"},			// Unknown protocol
		{ESOCKTNOSUPPORT,		"ESOCKTNOSUPPORT"},			// Socket type not supported
		{EADDRNOTAVAIL,			"EADDRNOTAVAIL"},			// Address not available
		{ENETRESET,				"ENETRESET"},
		{EISCONN,				"EISCONN"},					// Socket is already connected
		{ENOTCONN,				"ENOTCONN"},				// Socket is not connected
		{ETOOMANYREFS,			"ETOOMANYREFS"},
		{EPROCLIM,				"EPROCLIM"},
		{EUSERS,				"EUSERS"},
		{EDQUOT,				"EDQUOT"},
		{ESTALE,				"ESTALE"},
		{ENOTSUP,				"ENOTSUP"},					// Not supported
		{ENOMEDIUM,				"ENOMEDIUM"}, 				// No medium (in tape drive)
		{ENOSHARE,				"ENOSHARE"},			    // No such host or network path
#if defined(ECASECLASH)
		{ECASECLASH,			"ECASECLASH"},			    // Filename exists with different case
#endif	/* defined(ECASECLASH) */
		{0, NULL}
	};
	int i;

	if (err>0) {
		return (NULL);
	}
	i = 0;
	while (libname[i].pszName!=NULL) {
		if (SCE_ERROR_ERRNO(err)==libname[i].num) {
			return (libname[i].pszName);
		}
		i++;
	}
	return ("unknown");
}

