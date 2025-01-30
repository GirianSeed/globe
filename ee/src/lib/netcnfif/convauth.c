/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7.1
 $Id: convauth.c,v 1.2 2002/10/19 12:38:26 tetsu Exp $
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         convauth.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.05.05      tetsu       First version
 *       1.2            2002.10.19      tetsu       Change path of netcnfif.h
 */

#include <string.h>
#include <netcnfif.h>

#define AOL_PREFIX "aolnet/"

int sceNetcnfifConvAuthname(char *src, char *dst, int dst_size)
{
    char *p;
    int i = 0, realm_string = 0;

    /* dst の初期化 */
    bzero(dst, dst_size);

    /* src, dst のチェック */
    if(src == NULL || dst == NULL)
	return (sceNETCNFIF_INVALID_ARG);

    /* realm string であるかどうかのチェック */
    if(strncmp(src, AOL_PREFIX, strlen(AOL_PREFIX)) == 0){
	for(p = src, i = 0; (p = strchr(p, '.')) != NULL; p++, i++);
	if(i == 5) realm_string = 1;
    }

    if(realm_string){
	/* realm string であるなら screen name/password を取り出す */
	p = strchr(src, '.');
	p = strchr(++p, '.');
	p++;
	for(i = 0; i < dst_size - 1 && p[i] != '\0' && p[i] != '.'; i++){
	    dst[i] = p[i];
	}
	dst[i] = '\0';
    }else{
	/* realm string でないなら dst_size - 1 分だけ dst にコピーする */
	for(i = 0; i < dst_size - 1 && src[i] != '\0'; i++){
	    dst[i] = src[i];
	}
	dst[i] = '\0';
    }

    return (0);
}
