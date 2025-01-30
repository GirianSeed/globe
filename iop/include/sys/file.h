/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        file.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

#ifndef _SYS_FILE_H
#define _SYS_FILE_H

/*
  ファイルの読み込み
    fd = open("sim:ふぁいる名", O_RDONLY);
    n = read(fd, buffer, readsize);

  ファイルの書き込み
    ファイルが存在しないとエラーになる
        fd = open("sim:ふぁいる名", O_WRONLY);
        n = write(fd, buffer, readsize);
    ファイルが存在しないと自動的にファイルをつくる
        fd = open("sim:ふぁいる名", O_WRONLY|O_CREAT);
        n = write(fd, buffer, readsize);
    オープン後にファイルのサイズを 0 に戻す。
        fd = open("sim:ふぁいる名", O_WRONLY|O_TRUNC);
        n = write(fd, buffer, readsize);
    ファイルが存在しないと自動的にファイルをつくり、オープン後に
    ファイルのサイズを 0 に戻す。
        fd = open("sim:ふぁいる名", O_WRONLY|O_TRUNC|O_CREAT);
        n = write(fd, buffer, readsize);

*/

/* flags */
#define	FREAD		0x0001		/* readable */
#define	FWRITE		0x0002		/* writable */
#define	FNBLOCK		0x0004		/* non-blocking reads */
#define	FRLOCK		0x0010		/* read locked (non-shared) */
#define	FWLOCK		0x0020		/* write locked (non-shared) */
#define	FAPPEND		0x0100		/* append on each write */
#define	FCREAT		0x0200		/* create if nonexistant */
#define	FTRUNC		0x0400		/* truncate to zero length */
#define	FSCAN		0x1000		/* scan type */
#define	FRCOM		0x2000		/* remote command entry */
#define	FNBUF		0x4000		/* no ring buf. and console interrupt */
#define	FASYNC		0x8000		/* asyncronous i/o */

/* Flag for open() */
#define O_RDONLY        FREAD
#define O_WRONLY        FWRITE
#define O_RDWR          FREAD|FWRITE
#define O_NBLOCK        FNBLOCK /* Non-Blocking I/O */
#define O_APPEND        FAPPEND /* append (writes guaranteed at the end) */
#define O_CREAT         FCREAT  /* open with file create */
#define O_TRUNC         FTRUNC  /* open with truncation */
#define O_NOBUF         FNBUF	/* no device buffer and console interrupt */
#define O_NOWAIT        FASYNC	/* asyncronous i/o */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#endif /* _SYS_FILE_H */
