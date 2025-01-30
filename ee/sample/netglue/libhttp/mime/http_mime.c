/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_mime -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        <http_mime.c>
 *                     <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 *       2.0.0     Aug,28,2002    komaki              for multiple network libraries
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <malloc.h>
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

static void subparts(sceHTTPMimeFilter_t *p){
	int closed;

	p = p->next;
	if (!p)
		return;

	closed = 0;
	while (1){
		if (sceHTTPMimeFilterParseHeaders(p) < 0)
			break;
		if (sceHTTPMimeFilterGetMultipartType(p)
				== sceHTTPMultipart_MIXED)
			subparts(p);
		else {
			if (sceHTTPMimeFilterApply(p, &closed) < 0)
				break;
			if (closed)
				break;
			if (sceHTTPFreeHeaderList(p->headers) < 0){
				scePrintf("sceHTTPFreeHeaderList failed\n");
				return;
			}
			p->headers = NULL;
		}
	}
}

int http_mime_init(int argc, char **argv){
	int ret;

	if(argc < 2){
		scePrintf("usage: http_mime.elf url\n");
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

int http_mime(int argc, char **argv){
	sceHTTPClient_t *client;
	sceHTTPResponse_t *response;
	sceHTTPHeaderList_t *list;
	sceHTTPMimeFilter_t *fil;
	int i;
	char *odata;
	int olen;

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

	scePrintf("<<Apply MIME filter>>\n");
	fil = sceHTTPMimeFilterCreate(sceHTTPMimeFilter_STRING,
				(void *)response->entity,
				(int)response->length,
				sceHTTPMimeFilter_STRING, (void *)0);
	if (fil == NULL){
		scePrintf("can't create mime filter\n");
		return -1;
	}
	if (sceHTTPMimeFilterParseHeaders(fil) < 0){
		scePrintf("sceHTTPMimeFilterParseHeaders failed\n");
		return -1;
	}
	if (sceHTTPMimeFilterGetMultipartType(fil))
		subparts(fil);
	else{
		if (sceHTTPMimeFilterApply(fil, NULL) < 0){
			scePrintf("sceHTTPMimeFilterApply failed\n");
			return -1;
		}
	}

	if (sceHTTPMimeFilterGetStringOutput(fil, &odata, &olen) < 0){
		scePrintf("sceHTTPMimeFilterGetStringOutput failed\n");
		return -1;
	}

	scePrintf("<<filtered length %d>>\n", olen);
	for(i = 0; i < olen; i++)
		scePrintf("%c", odata[i]);
	sceHTTPMimeFilterFree(fil);
	scePrintf("<<End of MIME filtering>>\n");

	free(odata);
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
