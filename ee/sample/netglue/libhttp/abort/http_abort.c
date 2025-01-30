/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_abort -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <http_abort.c>
 *                     <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Dec,02,2002    komaki              first version
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <sifdev.h>
#include <netglue.h>
#include <libhttp.h>

static int wait_eoo;
static int open_failed;
static int wait_eot;
static int transaction_failed;
static sceHTTPParsedURI_t *p;
static int method;
static char *idata;
static int fd = -1;

static void eoo_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	scePrintf("<<End of Open (%d)>>\n", rtn);
	wait_eoo = 1;
	open_failed = rtn;
}

static void eot_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	scePrintf("<<End of Transaction (%d)>>\n", rtn);
	wait_eot = 1;
	transaction_failed = rtn;
}

static void force_abort(void *p){
	sceHTTPClient_t *client = p;

	sceNetGlueThreadInit(0);
	/* Wait 5 sec. */
	DelayThread(5 * 1000000);
	scePrintf("<<Force abort>>\n");
	sceHTTPAbort(client);
	sceNetGlueThreadTerminate(0);
}

int http_abort_init(int argc, char **argv){
	int ret;

	if(argc < 3){
usage:
		scePrintf("usage: run http_abort.elf (-get|-post|-head) url [post-data]\n");
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
	} else if (method == sceHTTPM_GET && argc == 4){
		fd = sceOpen(argv[3], SCE_WRONLY|SCE_CREAT|SCE_TRUNC);
		if (fd < 0){
			scePrintf ("can't open file %s\n", argv[3]);
			return -1;
		}
	}

	p = sceHTTPParseURI(argv[2], (sceHTTPParseURI_FILENAME
					| sceHTTPParseURI_SEARCHPART));
	if(!p){
		scePrintf("bad URI\n");
		return -1;
	}

	return 0;
}

int http_abort(void){
	sceHTTPClient_t *client;
	sceHTTPResponse_t *response;
	struct ThreadParam params, *param = &params;
	int aid;
	static int ss[2048];

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
		if (sceHTTPSetOption(client, sceHTTPO_RequestEntity, idata, n, 0) < 0){
			scePrintf("sceHTTPSetOption failed\n");
			return -1;
		}

    	} else if (method == sceHTTPM_GET && fd >= 0){
		if (sceHTTPSetOption (client, sceHTTPO_LocalFile, fd) < 0){
			scePrintf("sceHTTPSetOption failed\n");
			return -1;
		}
	}
	if (sceHTTPSetOption(client, sceHTTPO_EndOfOpenCB, eoo_cb, NULL) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetOption(client, sceHTTPO_BlockingOpenMode, 0) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetOption(client, sceHTTPO_EndOfTransactionCB, eot_cb, NULL) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetOption(client, sceHTTPO_BlockingMode, 0) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}

	/* create process for aborting timer */
	param->entry = (void *)force_abort;
	param->stack = (void *)(((int)ss + 16) & ~0xf);
	param->stackSize = sizeof(ss) - 16;
	param->gpReg = &_gp;
	param->initPriority = 40;
	aid = CreateThread(param);
	StartThread(aid, client);

	if (sceHTTPOpen(client) < 0){
		scePrintf("sceHTTPOpen failed\n");
		return -1;
	}

	/* wait callback */
	while (wait_eoo == 0)
		;

	if (open_failed){
		if (sceHTTPGetClientError(client) == sceHTTPError_INTR){
			scePrintf("<<Open Interrupted!>>\n");
			goto end;
		}
		scePrintf("Open failed with %d\n",	sceHTTPGetClientError(client));
		return -1;
	}

	if (sceHTTPRequest(client) < 0){
		scePrintf("sceHTTPRequest failed\n");
		return -1;
	}

	/* wait callback */
	while (wait_eot == 0)
		;

	/* get response */
	if ((response = sceHTTPGetResponse(client)) == NULL){
		scePrintf("sceHTTPGetResponse failed\n");
		return -1;
	}
	if (response->interrupted)
		scePrintf("<<Transfer interrupted!>>\n");
	else if (transaction_failed)
		scePrintf("Transaction failed\n");

	if (method == sceHTTPM_GET && fd >= 0){
		if (sceClose(fd) < 0){
			scePrintf("sceClose failed\n");
			return -1;
		}
	}
	if (sceHTTPClose(client) < 0){
		scePrintf("sceHTTPClose failed\n");
		return -1;
	}

	end:
	TerminateThread(aid);
	if (DeleteThread(aid) < 0){
		scePrintf("DeleteThread failed\n");
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
