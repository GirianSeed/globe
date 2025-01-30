/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_blk -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <http_blk.c>
 *                      <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 *       2.0.0     Aug,28,2002    komaki              for multiple network libraries
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <eeregs.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libmrpc.h>
#include <libnet.h>
#include <errno.h>
#include <libhttp.h>

static sceHTTPParsedURI_t *p;
static int method;
static char *idata;

int http_blk_init(int argc, char **argv){
	int ret;

	if(argc < 3){
usage:
		scePrintf("usage: run http_blk.elf (-get|-post|-head) url [post-data]\n");
		return -1;
	}

	if((ret = sceHTTPInit()) < 0){
		scePrintf("sceHTTPInit() failed.\n");
		return -1;
	}

	if(strcmp (argv[1], "-get") == 0)
		method = sceHTTPM_GET;
	else if(strcmp (argv[1], "-post") == 0)
		method = sceHTTPM_POST;
	else if (strcmp (argv[1], "-head") == 0)
		method = sceHTTPM_HEAD;
	else
		goto usage;

	if (method == sceHTTPM_POST){
		if (argc != 4)
			goto usage;
		idata = argv[3];
	}

	p = sceHTTPParseURI(argv[2], (sceHTTPParseURI_FILENAME
					| sceHTTPParseURI_SEARCHPART));
	if(!p){
		scePrintf("bad URI\n");
		return -1;
	}

	return 0;
}

int http_blk(void){
	sceHTTPClient_t *client;
	sceHTTPResponse_t *response;
	sceHTTPHeaderList_t *list;
	int i;

	if ((client = sceHTTPCreate()) == NULL){
		scePrintf("sceHTTPCreate failed\n");
		return -1;
	}

	if (sceHTTPSetOption(client, sceHTTPO_ParsedURI, p) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetOption(client, sceHTTPO_Method, method) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (method == sceHTTPM_POST){
		unsigned int n = strlen(idata);
		sceHTTPSetOption(client, sceHTTPO_RequestEntity, idata, n, 0);
    	}
	if (sceHTTPSetOption(client, sceHTTPO_BlockingMode, 1) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPOpen(client) < 0){
		scePrintf("sceHTTPOpen failed\n");
		return -1;
	}
	if (sceHTTPRequest(client) < 0){
		scePrintf("sceHTTPRequest failed\n");
		return -1;
	}

	/* show response */
	if ((response = sceHTTPGetResponse(client)) == NULL){
		scePrintf("sceHTTPGetResponse failed\n");
		return -1;
	}
	scePrintf("<<Response>>\n");
	scePrintf("Status-line: Protocol=%s Version=%d Revison=%d Code=%d(%s,%s)\n",
		(response->server_prot == sceHTTPProt_HTTP ? "http" : "https"),
		response->http_ver, response->http_rev,
		response->code, response->reason,
		sceHTTPErrorString(response->code));

	for (list = response->headers; list; list = sceHTTPNextHeader(list))
		scePrintf("%s: %s\n", list->name, list->value);

	scePrintf("<<Body length=%d>>\n", (int)response->length);
	for(i = 0; i < response->length; i++)
		scePrintf("%c", response->entity[i]);
	scePrintf("<<End of Body>>\n");

	if (sceHTTPClose(client) < 0){
		scePrintf("sceHTTPClose failed\n");
		return -1;
	}
	if (sceHTTPDestroy(client) < 0){
		scePrintf("sceHTTPDestroy failed\n");
		return -1;
	}
	if (sceHTTPFreeURI(p)){
		scePrintf("sceHTTPFreeURI failed\n");
		return -1;
	}

	if (sceHTTPTerminate()){
		scePrintf("sceHTTPTerminate failed\n");
		return -1;
	}

	return 0;
}
