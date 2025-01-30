/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_auth -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <http_auth.c>
 *                         <libhttp functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,21,2001    komaki              first version
 *       2.0.0     Aug,28,2002    komaki              for multiple netowrk libraries
 */

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <libhttp.h>

static int wait_eot;
static int transaction_failed;
static sceHTTPParsedURI_t *p;

static void eot_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	scePrintf("<<End of Transaction (%d)>>\n", rtn);
	wait_eot = 1;
	transaction_failed = rtn;
}

int http_auth_init(int argc, char **argv){
	int ret;

	if(argc < 4){
		scePrintf("usage: http_auth.elf url username passwd\n");
		return -1;
	}

	if((ret = sceHTTPInit()) < 0){
		scePrintf("sceHTTPInit() failed.\n");
		return -1;
	}

	p = sceHTTPParseURI(argv[1], (sceHTTPParseURI_FILENAME
					| sceHTTPParseURI_SEARCHPART));
	if(!p){
		scePrintf("bad URI\n");
		return -1;
	}

	return 0;
}

int http_auth(char *user, char *password){
	sceHTTPClient_t *client;
	sceHTTPResponse_t *response;
	sceHTTPHeaderList_t *list;
	sceHTTPAuthList_t *alist;
	sceHTTPAuthInfo_t *infop;
	sceHTTPDigest_t dig;
	char cnonce[16];
	int method;
	int rnd;

	method = sceHTTPM_GET;

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

	switch (response->code){
	case sceHTTPC_Unauthorized:
		alist = sceHTTPParseAuth(response);
		if (alist == NULL){
			scePrintf("Can't get authenticate info\n");
			return -1;
		}

		rnd = (int)response->date + rand();

		if (sceHTTPCleanUpResponse(client) < 0){
			scePrintf("sceHTTPCleanUpResponse failed\n");
			return -1;
		}
		/* clear old request headers */
		if (sceHTTPSetOption(client, sceHTTPO_RequestHeaders, NULL, 1)
				< 0){
			scePrintf("sceHTTPSetOption failed\n");
			return -1;
		}

		/* Handle first info for simplicity. This is a sample. */
		if (alist->auth.type == sceHTTPAuth_BASIC){
			if (sceHTTPSetBasicAuth(client, user, password, 0) < 0){
				scePrintf("sceHTTPSetBasicAuth failed\n");
				return -1;
			}
		} else {
			int qop;
			dig.username = user;
			dig.password = password;
			dig.realm = (char *)alist->auth.realm;
			dig.uri = (char *)p->filename;
			dig.nonce = (char *)alist->auth.nonce;
			dig.opaque = (char *)alist->auth.opaque;
			dig.algorithm = alist->auth.algorithm;
			dig.nc = 1;
			qop = alist->auth.qop;
			if (qop & sceHTTPDigestQOP_AUTH)
				dig.qop = sceHTTPDigestQOP_AUTH;
			else
				dig.qop = qop & sceHTTPDigestQOP_AUTHINT;
			/* If qop is set, set cnonce. */
			if (dig.qop){
				sprintf(cnonce, "%08x", rnd);
				dig.cnonce = cnonce;
			} else
				dig.cnonce = NULL;
			dig.method = method;
			dig.entity = NULL;
			dig.length = 0;
			if (sceHTTPSetDigestAuth(client, &dig, 0) < 0){
				scePrintf("sceHTTPSetDigestAuth failed\n");
				return -1;
			}
		}

		/* try again */
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

		/* show response */
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
		sceHTTPClose(client);

		/* Verify Authentication-Info header if exists */
		infop = sceHTTPParseAuthInfo(response);
		if (infop){
			switch(sceHTTPVerifyAuthInfo(client, infop, &dig, 0)){
			case 1:
				scePrintf("<<Authentication-Info verified>>\n");
				break;
			case 0:
				scePrintf("<<Authentication-Info not verified>>\n");
				break;
			case -1:
			default:
				scePrintf("sceHTTPVerifyAuthInfo failed\n");
				return -1;
				break;
			}
			if (sceHTTPFreeAuthInfo(infop) < 0){
				scePrintf("sceHTTPFreeAuthInfo failed\n");
				return -1;
			}
		}
		if (sceHTTPFreeAuthList(alist) < 0){
			scePrintf("sceHTTPFreeAuthList failed\n");
			return -1;
		}
		break;
	default:
		break;
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
