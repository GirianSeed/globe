/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                       - http_cookie -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        <http_cookie.c>
 *                      <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 *       2.0.0     Aug,28,2002    komaki              for multiple network library
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <libhttp.h>

static int wait_eot;
static int transaction_failed;
static sceHTTPParsedURI_t *p;
static int method;

static void eot_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	scePrintf("<<End of Transaction (%d)>>\n", rtn);
	wait_eot = 1;
	transaction_failed = rtn;
}

int http_cookie_init(int argc, char **argv){
	int ret;

	if(argc < 2){
		scePrintf("usage: http_cookie.elf url\n");
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

	return 0;
}

int http_cookie(void){
	sceHTTPClient_t *client;
	sceHTTPResponse_t *response;
	sceHTTPHeaderList_t *list;
	sceHTTPCookieList_t *cklist;

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

	if (sceHTTPClose(client) < 0){
		scePrintf("sceHTTPClose failed\n");
		return -1;
	}

	cklist = sceHTTPParseCookie(client, response);
	if (cklist == NULL){
		scePrintf("No cookies\n");
		goto end;
	}

	/* try again with cookie*/
	if (sceHTTPCleanUpResponse(client) < 0){
		scePrintf("sceHTTPCleanUpResponse failed\n");
		return -1;
	}
	/* clear old request headers */
	if (sceHTTPSetOption(client, sceHTTPO_RequestHeaders, NULL, 1) < 0){
		scePrintf("sceHTTPSetOption failed\n");
		return -1;
	}
	if (sceHTTPSetCookie(client, cklist) < 0){
		scePrintf("sceHTTPSetCookie failed\n");
		return -1;
	}
	if (sceHTTPFreeCookieList(cklist) < 0){
		scePrintf("sceHTTPFreeCookieList failed\n");
		return -1;
	}
	wait_eot = 0;
	if (sceHTTPOpen(client) < 0){
		scePrintf("sceHTTPOpen failed\n");
		return -1;
	}
	if (sceHTTPRequest(client) < 0){
		scePrintf("2nd sceHTTPRequest failed\n");
		return -1;
	}

	/* wait callback */
	while (wait_eot == 0)
		;

	if (transaction_failed){
		scePrintf("Transaction failed\n");
		return -1;
	}

	/* show 2nd response */
	if ((response = sceHTTPGetResponse(client)) == NULL){
		scePrintf("sceHTTPGetResponse failed\n");
		return -1;
	}
	scePrintf("<<Response2>>\n");
	scePrintf("Status-line: Protocol=%s Version=%d Revison=%d Code=%d(%s,%s)\n",
		(response->server_prot == sceHTTPProt_HTTP ? "http" : "https"),
		response->http_ver, response->http_rev,
		response->code, response->reason,
		sceHTTPErrorString(response->code));

	for (list = response->headers; list; list = sceHTTPNextHeader(list))
		scePrintf("%s: %s\n", list->name, list->value);

	scePrintf("<<Body length=%d>>\n", (int)response->length);

end:
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
