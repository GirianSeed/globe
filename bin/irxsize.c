/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/* 
 *		I/O Processor Utiltiy Program
 *
 *		-- Utility for irx module size detect --
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                         irxsize.c
 * 
 *  Date            Design      Log
 *  ------------------------------------
 *  2002-5-17       isii
 *  2002-6-08       isii       module name bug fix
 *
 */

/* HOW to compile
 *
 * linux(for intel) or Solaris or MacOS X
 *   $ cc -o irxsize irxsize.c
 * IOP kernel
 *   $ iop-gcc -o irxsize.irx irxsize.c
 * Windows + visual C++
 *   use visual studio
 *
 */

#include <stdio.h>

/* ------------ cpu depend defines ------------ */
#if defined(__R3000__) && defined(__MIPSEL__)
# define __IOP__ 1
# define NEED_SWAP 0
typedef unsigned short	Elf32_Half;
typedef unsigned int	Elf32_Addr;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Word;
typedef 	 int	Elf32_SWord;
#endif

#if defined(__i386) || defined(_M_IX86)
# define NEED_SWAP 0
typedef unsigned short	Elf32_Half;
typedef unsigned int	Elf32_Addr;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Word;
typedef 	 int	Elf32_SWord;
#endif

#if defined(__sparc)
# define NEED_SWAP 1
typedef unsigned short	Elf32_Half;
typedef unsigned int	Elf32_Addr;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Word;
typedef 	 int	Elf32_SWord;
#endif

#if defined(__ppc__)
# define NEED_SWAP 1
typedef unsigned short	Elf32_Half;
typedef unsigned int	Elf32_Addr;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Word;
typedef 	 int	Elf32_SWord;
#endif

/* ------------ END of cpu depend defines ------------ */

/* ------------ system depend defines ------------ */
/* IOP kernel */
#ifdef __IOP__
# include <kernel.h>
# include <sys/file.h>
# include <errno.h>
# include <string.h>
# define OPEN_READ_BIN	(O_RDONLY)
#endif

#if  defined(__linux) || defined(__svr4) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__) )
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# define OPEN_READ_BIN	(O_RDONLY)
#endif

#if  defined(WIN32)
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <io.h>
# include <errno.h>
# include <string.h>
# define OPEN_READ_BIN	(_O_RDONLY|_O_BINARY)
# define open _open
# define read _read
# define close _close
#endif

/* ------------ END of system depend defines ------------ */

#if NEED_SWAP
void swapmemory(void *addr, const char *format, int times);
#else
#define swapmemory(a,b,c)	do {} while(0)
#endif

#define EI_IDENT	16
/* ------ ELF header ------ */
typedef struct _Elf32_ehdr {
    unsigned char e_ident[EI_IDENT];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

/* Fields in e_ident[] */
/* e_ident[0..3]  0x7f,'ELF' */
#define EI_CLASS	4		/* File class */
#define 	ELFCLASS32	1	/* 32-bit objects */
#define EI_DATA		5		/* Data encoding */
#define 	ELFDATA2LSB	1	/* 2's complement, little endian */
#define EI_VERSION	6		/* File version */
#define EI_PAD		7		/* Start of padding bytes */

/* Values for e_type, which identifies the object file type */
#define ET_SCE_IOPRELEXEC 0xFF80 /* SCE IOP Relocatable Executable file version 1 */
#define ET_SCE_IOPRELEXEC2 0xFF81 /* SCE IOP Relocatable Executable file version 2 */

#define EM_MIPS		8	/* MIPS R3000 (officially, big-endian only) */

#define EV_NONE		0		/* Invalid ELF version */
#define EV_CURRENT	1		/* Current version */

/* ------ Program header ------ */
typedef struct _Elf32_Phdr {
    Elf32_Word	p_type;
    Elf32_Off	p_offset;
    Elf32_Addr	p_vaddr;
    Elf32_Addr	p_paddr;
    Elf32_Word	p_filesz;
    Elf32_Word	p_memsz;
    Elf32_Word	p_flags;
    Elf32_Word	p_align;
} Elf32_Phdr;

#define PT_SCE_IOPMOD	0x70000080

typedef struct _Elf32_IopMod {
    Elf32_Word	moduleinfo;
    Elf32_Word	entry;
    Elf32_Word	gp_value;
    Elf32_Word  text_size;
    Elf32_Word	data_size;
    Elf32_Word	bss_size;
    Elf32_Half	moduleversion;
    char          modulename[1];
} Elf32_IopMod;
#define SizeOfElf32_IopMod	(sizeof(Elf32_Word)*6+sizeof(Elf32_Half)+1)

/* ------ IRX header ------ */
typedef struct _irxheader {
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr[2];
    Elf32_IopMod iopmod;
} Irxheader;
#define SizeOfIrxheader	(sizeof(Elf32_Ehdr)+sizeof(Elf32_Phdr)*2+SizeOfElf32_IopMod)

#define IOP_MODULE_overhead	(0x40)
#define SWAP_ELF_HD		"ccccccccccccccccsslllllssssss"
#define SWAP_PROGRAM_HD		"llllllll"
#define	SWAP_IOPMOD		"lllllls"

#define SWAP_IRX_HEAD  SWAP_ELF_HD SWAP_PROGRAM_HD SWAP_PROGRAM_HD SWAP_IOPMOD


/* ------------------------ main ------------------------------*/
int get_module_info(const char *fname, Elf32_IopMod *result,
		    int *irxver, char *modname);
void print_title(void);
void print_module_info(const char *fname, Elf32_IopMod *iopmod,
		       int *irxver, const char *modname);

#ifdef __IOP__
int start(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int i;
    Elf32_IopMod iopmodinfo;
    int irxver;
    char modname[100];

    print_title();
    for( i = 1 ; i < argc; i++ ) {
	if( get_module_info(argv[i], &iopmodinfo, &irxver, modname) ) {
	    print_module_info(argv[i], &iopmodinfo, &irxver, modname);
	} else
	    printf("%s: read error\n", argv[i]);
    }
#ifdef __IOP__
    return NO_RESIDENT_END;
#else
    return 0;
#endif
}


int get_module_info(const char *fname, Elf32_IopMod *result,
		    int *irxver, char *modname)
{
    int fd;
    Irxheader  irxhdr;
    Elf32_Ehdr *ehdr;
    Elf32_Phdr *phdr;

    fd = open(fname, OPEN_READ_BIN);
    if( fd < 0 ) return 0;
    if( read(fd, &irxhdr, SizeOfIrxheader) != SizeOfIrxheader ) {
	close(fd);
	return 0;
    }
    swapmemory(&irxhdr, SWAP_IRX_HEAD, 1);
    ehdr = &irxhdr.ehdr;
    phdr = irxhdr.phdr;
    if( ehdr->e_ident[EI_CLASS] == ELFCLASS32
	&& ehdr->e_ident[EI_DATA] == ELFDATA2LSB
	&& ehdr->e_machine   == EM_MIPS
	&& ehdr->e_phentsize == sizeof(Elf32_Phdr)
	&& ehdr->e_phnum     == 2
	&& phdr->p_type      == PT_SCE_IOPMOD
	&& (ehdr->e_type     == ET_SCE_IOPRELEXEC
	    || ehdr->e_type  == ET_SCE_IOPRELEXEC2 )
	&& phdr->p_offset == (sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr)*2) ){
	memcpy( result, &irxhdr.iopmod, sizeof(Elf32_IopMod));
	modname[0] = irxhdr.iopmod.modulename[0];
	if( phdr->p_filesz > SizeOfElf32_IopMod ) 
	    read(fd, modname+1, phdr->p_filesz-SizeOfElf32_IopMod);
	close(fd);
	*irxver = 0;
	if( ehdr->e_type == ET_SCE_IOPRELEXEC )	 *irxver = 1;
	if( ehdr->e_type == ET_SCE_IOPRELEXEC2 ) *irxver = 2;
	return 1;
    }
    close(fd);
    return 0;
}


void print_title()
{
    printf("# file_name\tsize\ttext+data+bss\tirxver\tmodver\tmodname\n");
}

void print_module_info(const char *fname, Elf32_IopMod *iopmod,
		       int *irxver, const char *modname)
{
    int size;
    size = iopmod->text_size + iopmod->data_size + iopmod->bss_size;
    size += IOP_MODULE_overhead;
    size = (size+0xf) & (~0xf);
    printf("%s\t%d\t%d+%d+%d\t%d\t%04x\t%s\n", fname, size,	   
	   iopmod->text_size, iopmod->data_size, iopmod->bss_size,
	   *irxver, iopmod->moduleversion, modname);
}

#if  NEED_SWAP == 1
void swapmemory(void *aaddr, const char *format, int times)
{
    char *fp;
    unsigned char *addr;
    unsigned char d0, d1, d2, d3;

    addr = (unsigned char *)aaddr;
    while( times > 0 ) {
	fp = format;
	while( *fp ) {
	    switch( *fp ){
	    case 'l':
		d0 = addr[0]; d1 = addr[1]; d2 = addr[2]; d3 = addr[3];
		*addr++ = d3; *addr++ = d2; *addr++ = d1; *addr++ = d0;
		break;
	    case 's':
		d0 = addr[0]; d1 = addr[1];
		*addr++ = d1; *addr++ = d0;
		break;
	    case 'c':
		addr++;
		break;
	    default:
		printf("Illegal swapmemory format \n");
		exit(1);
	    }
	    fp++;
	}
	times--;
    }
}
#endif
