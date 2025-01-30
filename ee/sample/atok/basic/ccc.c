/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#include <libccc.h>
#include "app.h"
#include "io.h"

// Makefile���CCC_PATH�����ꤵ��Ƥ���������
// �ե�����ǥХ�����io.c�����
#define JIS2UCS_PATH    CCC_PATH "/libccc/lib/jis2ucs.bin"
#define UCS2JIS_PATH    CCC_PATH "/libccc/lib/ucs2jis.bin"

// �Ѵ��Хåե��Υ�����
#define BUFSZ           (256)

static sceCccJISCS      *_jis2ucs;
static sceCccUCS2       *_ucs2jis;
static sceCccUTF8	dst[BUFSZ];
static int		len;		// �Ǹ���Ѵ��ˤ��ʸ����

/*
 * libccc��Ȥ��ˤ��Ѵ��ơ��֥���������Ƥ����ʤ��ƤϤʤ�ʤ�
 */
void ccc_init(void)
{
    // �Ѵ��ơ��֥�ե�������ɤ߹���
    _jis2ucs = (sceCccJISCS *)file_read(JIS2UCS_PATH);
    _ucs2jis  = (sceCccUCS2 *)file_read(UCS2JIS_PATH);

    // �Ѵ��ơ��֥�����ꤹ��
    sceCccSetTable(_jis2ucs, _ucs2jis);
}

/*
 * SJIS->UTF8�Ѵ���Ԥ�dst�Υ��ɥ쥹���֤�
 * �Ѵ���̤�ʸ������get_len_ccc()��Ȥ�����
 * static�ʥХåե���ȤäƤ���Τ�thread safe�ǤϤʤ�
 * BUFSZ byte�ʾ�ξ��ϴ����Ѵ�����ʤ��������
 */
const sceCccUTF8 *ccc_conv_sjis2utf8(const char *sjis)
{
    len = sceCccSJIStoUTF8(dst, BUFSZ, sjis);
    return dst;
}

/*
 * �Ǹ���Ѵ���ʸ�������֤�
 */
int ccc_get_conv_length(void)
{
    return len;
}
