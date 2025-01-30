/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
// Version 0.00
// ASCII
// 
// Copyright (C) 1998-1999 Sony Computer Entertainment Inc. All Rights Reserved.
// 
// Version        Date            Design      Log
// --------------------------------------------------------------------
// 0.00           06/18/99        koji        1st version

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <deci2.h>
#include "pdriver.h"

typedef struct {
	volatile int    s;			// socket descriptor
	volatile u_int  wlen;		// write length
	volatile u_int  rlen;		// read length
	u_char         *wptr;		// write buffer pointer
	u_char         *rptr;		// read  buffer pointer
	volatile int    rbytes;		// total receive length
} OPT;

static OPT opt;
static char wbuf[BUFSZ] __attribute__((aligned (64))); // must be 64 for cache
static char rbuf[BUFSZ] __attribute__((aligned (64))); // must be 64 for cache


static void EventHandler(int event, int param, OPT *opt) {
	int n;

	switch (event) {
		case DECI2_READ:
			if ((opt->rlen + param) > BUFSZ) {
				opt->rbytes = -1;
				return;
			}
			// receive packet
			if ((n = sceDeci2ExRecv(opt->s, &opt->rptr[opt->rlen], param)) < 0)
				opt->rbytes = -1;
			opt->rlen += n;
			break;

		case DECI2_READDONE:
			opt->rbytes = opt->rlen;
			opt->rlen = 0;
			break;

		case DECI2_WRITE:
			// send packet
			if ((n = sceDeci2ExSend(opt->s, opt->wptr, opt->wlen)) < 0) {
				opt->rbytes = -1;
				return;
			}
			opt->wptr += n;
			opt->wlen -= n;
			break;

		case DECI2_WRITEDONE:
			if (opt->wlen != 0) opt->rbytes = -1;
			break;

		case DECI2_CHSTATUS:
			break;

		case DECI2_ERROR:
		default:
			opt->rbytes = -1;
			break;
	}
}


int InitProtocolDriver(void) {
	if ((opt.s = sceDeci2Open(PROTO_SAMPLE, &opt, (void *)EventHandler)) < 0)
		return 0;

	return 1;
}


int SystemOnHost(char *command, char *result) {
	int          ret = 0;
	char        *data;
	u_int        datalen = 0;
	sceDeci2Hdr *hdr;

	opt.wptr = UNCACHED(wbuf);
	opt.rptr = UNCACHED(rbuf);
	opt.wlen = 0;
	opt.rlen = 0;
	opt.rbytes = 0;

	hdr  = (sceDeci2Hdr *)opt.wptr;
	data = opt.wptr + sizeof(sceDeci2Hdr) + sizeof(u_int);

	// copy command to data buffer
	while ((*data++ = *command++)) datalen++;
	*data = '\0';
	datalen++;

	// caliculate length of deci2 packet
	opt.wlen = sizeof(sceDeci2Hdr) + sizeof(u_int) + datalen;

	// make deci2 header
	hdr->len   = opt.wlen;
	hdr->rsvd  = 0;
	hdr->proto = PROTO_SAMPLE;
	hdr->src   = NODE_EE;
	hdr->dest  = NODE_HOST;
	*(u_int *)(hdr+1) = 0;

	// request to send a packet
	if ((ret = sceDeci2ReqSend(opt.s, hdr->dest)) < 0) {
		printf("sceDeci2ReqSend fail, reason code = %08x\n", ret);
		return -1;
	}

	while (opt.rbytes == 0) ;					// wait until packet receive
	if (opt.rbytes < 0) return opt.rbytes;

	hdr = (sceDeci2Hdr *)opt.rptr;
	data = opt.rptr + sizeof(sceDeci2Hdr) + sizeof(u_int);
	datalen = hdr->len - (sizeof(sceDeci2Hdr) + sizeof(u_int));

	if ((*(u_int *)(hdr + 1))) {				// check result
		printf("Executed Command failed.\n");
		return -1;
	}

	memcpy(result, data, datalen);				// copy result and return
	return datalen;
}

// Local variables:
// tab-width: 4
// End:
// vi:set tabstop=4:
