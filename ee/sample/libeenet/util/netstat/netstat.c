/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                     - <libeenet netstat> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <netstat.c>
 *              <sample function of "netstat" command>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Dec,10,2002     komaki      first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <libeenet.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp_var.h>
#include <netinet/udp_var.h>
#include <netinet6/udp6_var.h>
#include <netinet/tcp_fsm.h>

#define MAX_TCPCB_NUM 20
#define MAX_UDPCB_NUM 20

#define PRINT_BUF_SIZE 256

char *tcpstates[] = {
	"CLOSED",	"LISTEN",	"SYN_SENT",	"SYN_RCVD",
	"ESTABLISHED",	"CLOSE_WAIT",	"FIN_WAIT_1",	"CLOSING",
	"LAST_ACK",	"FIN_WAIT_2",	"TIME_WAIT",
};

static char print_buf[PRINT_BUF_SIZE];
static union {
	struct sceEENetTcpcbstat thead[MAX_TCPCB_NUM];
	struct sceEENetUdpcbstat uhead[MAX_UDPCB_NUM];
} pcb;

#define PRINTF(...) \
    do { \
        sceSnprintf(print_buf, PRINT_BUF_SIZE, __VA_ARGS__), scePrintf("%s", print_buf); \
    }while(0)

#define NPRINTF(n, ...) \
    do { \
	n += sceSnprintf(print_buf, PRINT_BUF_SIZE, __VA_ARGS__), scePrintf("%s", print_buf); \
    }while(0)

void sceEENetShowNetstat(void) {
	int num, n;
	struct sceEENetTcpcbstat *thead, *ts;
	struct sceEENetUdpcbstat *uhead, *us;

	PRINTF("Active Internet connections (including servers)\n");
	PRINTF("%-5s %-6s %-6s %-22s %-22s %s\n",
		"Proto", "Recv-Q", "Send-Q", "Local Address", "Foreign Address", "State");

	thead = pcb.thead;
	num = MAX_TCPCB_NUM;
	sceEENetGetTcpcbstat(thead, &num);
	for (ts = thead; ts < thead + num; ts++) {
		PRINTF("%-5s %-6ld %-6ld ",
			"tcp",
			ts->ts_so_rcv_sb_cc,
			ts->ts_so_snd_sb_cc);
		n = 0;
		if (ts->ts_inp_laddr.s_addr == INADDR_ANY)
			NPRINTF(n, "*");
		else
			NPRINTF(n, "%s", sceEENetInetNtoa(ts->ts_inp_laddr));
		if (ts->ts_inp_lport == 0)
			NPRINTF(n, ":*");
		else
			NPRINTF(n, ":%d", ntohs(ts->ts_inp_lport));
		PRINTF("%*s ", 22 - n, "");

		n = 0;
		if (ts->ts_inp_faddr.s_addr == INADDR_ANY)
			NPRINTF(n, "*");
		else
			NPRINTF(n, "%s", sceEENetInetNtoa(ts->ts_inp_faddr));
		if (ts->ts_inp_fport == 0)
			NPRINTF(n, ":*");
		else
			NPRINTF(n, ":%d", ntohs(ts->ts_inp_fport));
		PRINTF("%*s ", 22 - n, "");

		if (ts->ts_t_state < 0 || ts->ts_t_state >= TCP_NSTATES)
			PRINTF("%d", ts->ts_t_state);
		else
			PRINTF("%s", tcpstates[ts->ts_t_state]);
		PRINTF("\n");
	}

udpcb:
	uhead = pcb.uhead;
	num = MAX_UDPCB_NUM;
	sceEENetGetUdpcbstat(uhead, &num);
	for (us = uhead; us < uhead + num; us++) {
		PRINTF("%-5s %-6ld %-6ld ",
			"udp",
			us->us_so_rcv_sb_cc,
			us->us_so_snd_sb_cc);
		n  = 0;
		if (us->us_inp_laddr.s_addr == INADDR_ANY)
			NPRINTF(n, "*");
		else
			NPRINTF(n, "%s", sceEENetInetNtoa(us->us_inp_laddr));
		if (us->us_inp_lport == 0)
			NPRINTF(n, ":*");
		else
			NPRINTF(n, ":%d", ntohs(us->us_inp_lport));
		PRINTF("%*s ", 22 - n, "");

		if (us->us_inp_faddr.s_addr == INADDR_ANY)
			NPRINTF(n, "*");
		else
			NPRINTF(n, "%s", sceEENetInetNtoa(us->us_inp_faddr));
		if (us->us_inp_fport == 0)
			NPRINTF(n, ":*");
		else
			NPRINTF(n, "%d", ntohs(us->us_inp_fport));
		PRINTF("%-*s ", 22 - n, "");
		PRINTF("\n");
	}
}

