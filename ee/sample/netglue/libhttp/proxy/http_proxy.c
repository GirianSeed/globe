/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                        - http_proxy -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        <http_proxy.c>
 *                      <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 *       2.0.0     Aug,28,2002    komaki              for multiple network libraries
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <libhttp.h>

static int wait_eot;
static int transaction_failed;
static sceHTTPParsedURI_t *p;
static sceHTTPParsedURI_t *proxy;
static int method;

static void eot_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	scePrintf("<<End of Transaction (%d)>>\n", rtn);
	wait_eot = 1;
	transaction_failed = rtn;
}

int http_proxy_init(int argc, char **argv){
	int ret;

	if(argc < 3){
		scePrintf("usage: http_proxy.elf url proxy\n");
		return -1;
	}

	if((ret = sceHTTPInit()) < 0){
		scePrintf("sceHTTPInit() failed.\n");
		return -1;
	}

	method = sceHTTPM_GET;

	p = sceHTTPParseURI(argv[1], (sceHTTPParseURI_FILENAME
					| sceHTTPParseURI_SEARCHPART));
	if(!p){
		scePrintf("bad URI\n");
		return -1;
	}

	proxy = sceHTTPParseURI(argv[2], 0);
	if(!p){
		scePrintf("bad Proxy\n");
		return -1;
	}

	return 0;
}

int http_proxy(void){
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
	if (sceHTTPSetOption(client, sceHTTPO_ProxyURI, proxy) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetOption(client, sceHTTPO_Method, method) < 0){
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
	if (sceHTTPOpen(client) < 0){
		scePrintf("sceHTTPOpen failed\n");
		return -1;
	}
	if (sceHTTPRequest(client) < 0){
		scePrintf("sceHTTPRequest failed\n");
		return -1;
	}

	/* wait callback */
	while (wait_eot == 0)
		;

	if (transaction_failed){
		scePrintf("Transaction failed\n");
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
	if (sceHTTPFreeURI(proxy) < 0){
		scePrintf("sceHTTPFreeURI failed\n");
		return -1;
	}

	if (sceHTTPTerminate()){
		scePrintf("sceHTTPTerminate failed\n");
		return -1;
	}

	return 0;
}
