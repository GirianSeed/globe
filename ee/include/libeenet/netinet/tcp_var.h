/*	$NetBSD: tcp_var.h,v 1.72.4.1 2000/08/16 01:22:24 itojun Exp $	*/

/*
%%% portions-copyright-nrl-98
Portions of this software are Copyright 1998 by Randall Atkinson,
Ronald Lee, Daniel McDonald, Bao Phan, and Chris Winters. All Rights
Reserved. All rights under this copyright have been assigned to the US
Naval Research Laboratory (NRL). The NRL Copyright Notice and License
Agreement Version 1.1 (January 17, 1995) applies to these portions of the
software.
You should have received a copy of the license with this software. If you
didn't get a copy, you may request one from <license@ipv6.nrl.navy.mil>.

*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1997, 1998, 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1993, 1994, 1995
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)tcp_var.h	8.4 (Berkeley) 5/24/95
 */

#ifndef _NETINET_TCP_VAR_H_
#define _NETINET_TCP_VAR_H_

/*
 * TCP statistics.
 * Many of these should be kept per connection,
 * but that's inconvenient at the moment.
 */
struct	sceEENetTcpstat {
	u_quad_t tcps_connattempt;	/* connections initiated */
	u_quad_t tcps_accepts;		/* connections accepted */
	u_quad_t tcps_connects;		/* connections established */
	u_quad_t tcps_drops;		/* connections dropped */
	u_quad_t tcps_conndrops;	/* embryonic connections dropped */
	u_quad_t tcps_closed;		/* conn. closed (includes drops) */
	u_quad_t tcps_segstimed;	/* segs where we tried to get rtt */
	u_quad_t tcps_rttupdated;	/* times we succeeded */
	u_quad_t tcps_delack;		/* delayed acks sent */
	u_quad_t tcps_timeoutdrop;	/* conn. dropped in rxmt timeout */
	u_quad_t tcps_rexmttimeo;	/* retransmit timeouts */
	u_quad_t tcps_persisttimeo;	/* persist timeouts */
	u_quad_t tcps_keeptimeo;	/* keepalive timeouts */
	u_quad_t tcps_keepprobe;	/* keepalive probes sent */
	u_quad_t tcps_keepdrops;	/* connections dropped in keepalive */
	u_quad_t tcps_persistdrops;	/* connections dropped in persist */
	u_quad_t tcps_connsdrained;	/* connections drained due to memory
					   shortage */

	u_quad_t tcps_sndtotal;		/* total packets sent */
	u_quad_t tcps_sndpack;		/* data packets sent */
	u_quad_t tcps_sndbyte;		/* data bytes sent */
	u_quad_t tcps_sndrexmitpack;	/* data packets retransmitted */
	u_quad_t tcps_sndrexmitbyte;	/* data bytes retransmitted */
	u_quad_t tcps_sndacks;		/* ack-only packets sent */
	u_quad_t tcps_sndprobe;		/* window probes sent */
	u_quad_t tcps_sndurg;		/* packets sent with URG only */
	u_quad_t tcps_sndwinup;		/* window update-only packets sent */
	u_quad_t tcps_sndctrl;		/* control (SYN|FIN|RST) packets sent */

	u_quad_t tcps_rcvtotal;		/* total packets received */
	u_quad_t tcps_rcvpack;		/* packets received in sequence */
	u_quad_t tcps_rcvbyte;		/* bytes received in sequence */
	u_quad_t tcps_rcvbadsum;	/* packets received with ccksum errs */
	u_quad_t tcps_rcvbadoff;	/* packets received with bad offset */
	u_quad_t tcps_rcvmemdrop;	/* packets dropped for lack of memory */
	u_quad_t tcps_rcvshort;		/* packets received too short */
	u_quad_t tcps_rcvduppack;	/* duplicate-only packets received */
	u_quad_t tcps_rcvdupbyte;	/* duplicate-only bytes received */
	u_quad_t tcps_rcvpartduppack;	/* packets with some duplicate data */
	u_quad_t tcps_rcvpartdupbyte;	/* dup. bytes in part-dup. packets */
	u_quad_t tcps_rcvoopack;	/* out-of-order packets received */
	u_quad_t tcps_rcvoobyte;	/* out-of-order bytes received */
	u_quad_t tcps_rcvpackafterwin;	/* packets with data after window */
	u_quad_t tcps_rcvbyteafterwin;	/* bytes rcvd after window */
	u_quad_t tcps_rcvafterclose;	/* packets rcvd after "close" */
	u_quad_t tcps_rcvwinprobe;	/* rcvd window probe packets */
	u_quad_t tcps_rcvdupack;	/* rcvd duplicate acks */
	u_quad_t tcps_rcvacktoomuch;	/* rcvd acks for unsent data */
	u_quad_t tcps_rcvackpack;	/* rcvd ack packets */
	u_quad_t tcps_rcvackbyte;	/* bytes acked by rcvd acks */
	u_quad_t tcps_rcvwinupd;	/* rcvd window update packets */
	u_quad_t tcps_pawsdrop;		/* segments dropped due to PAWS */
	u_quad_t tcps_predack;		/* times hdr predict ok for acks */
	u_quad_t tcps_preddat;		/* times hdr predict ok for data pkts */

	u_quad_t tcps_pcbhashmiss;	/* input packets missing pcb hash */
	u_quad_t tcps_noport;		/* no socket on port */
	u_quad_t tcps_badsyn;		/* received ack for which we have
					   no SYN in compressed state */
};

#define sceEENettcpstat	((struct sceEENetTcpstat *)&__sceEENettcpstat)

struct sceEENetTcpcbstat {
	struct sceEENetTcpcbstat	*ts_next;	/* list of tcpcbstat */
	u_long		ts_so_snd_sb_cc;/* actual chars in send buffer */
	u_long		ts_so_rcv_sb_cc;/* actual chars in recv buffer */
	struct sceEENetInAddr	ts_inp_laddr;	/* local address */
	struct sceEENetInAddr	ts_inp_faddr;	/* foreign address */
	u_int16_t	ts_inp_lport;	/* local port */
	u_int16_t	ts_inp_fport;	/* foreign port */
	short		ts_t_state;	/* state of this connection */
};

int sceEENetGetTcpcbstat(struct sceEENetTcpcbstat **);
void sceEENetFreeTcpcbstat(struct sceEENetTcpcbstat *);

struct sceEENetTcp6cbstat {
	struct sceEENetTcp6cbstat *ts_next;	/* list of tcpcbstat */
	u_long		ts_so_snd_sb_cc;/* actual chars in send buffer */
	u_long		ts_so_rcv_sb_cc;/* actual chars in recv buffer */
	struct sceEENetIn6Addr	ts_in6p_laddr;	/* local address */
	struct sceEENetIn6Addr	ts_in6p_faddr;	/* foreign address */
	u_int16_t	ts_in6p_lport;	/* local port */
	u_int16_t	ts_in6p_fport;	/* foreign port */
	short		ts_t_state;	/* state of this connection */
};

int sceEENetGetTcp6cbstat(struct sceEENetTcp6cbstat **);
void sceEENetFreeTcp6cbstat(struct sceEENetTcp6cbstat *);

/*
 * Names for TCP sysctl objects.
 */
#define	TCPCTL_RFC1323		1	/* RFC1323 timestamps/scaling */
#define	TCPCTL_SENDSPACE	2	/* default send buffer */
#define	TCPCTL_RECVSPACE	3	/* default recv buffer */
#define	TCPCTL_MSSDFLT		4	/* default seg size */
#define	TCPCTL_SYN_CACHE_LIMIT	5	/* max size of comp. state engine */
#define	TCPCTL_SYN_BUCKET_LIMIT	6	/* max size of hash bucket */
#define	TCPCTL_SYN_CACHE_INTER	7	/* interval of comp. state timer */
#define	TCPCTL_INIT_WIN		8	/* initial window */
#define	TCPCTL_MSS_IFMTU	9	/* mss from interface, not in_maxmtu */
#define	TCPCTL_SACK		10	/* RFC2018 selective acknowledgement */
#define	TCPCTL_WSCALE		11	/* RFC1323 window scaling */
#define	TCPCTL_TSTAMP		12	/* RFC1323 timestamps */
#define	TCPCTL_COMPAT_42	13	/* 4.2BSD TCP bug work-arounds */
#define	TCPCTL_CWM		14	/* Congestion Window Monitoring */
#define	TCPCTL_CWM_BURSTSIZE	15	/* burst size allowed by CWM */
#define	TCPCTL_ACK_ON_PUSH	16	/* ACK immediately on PUSH */
#define	TCPCTL_KEEPIDLE		17	/* keepalive idle time */
#define	TCPCTL_KEEPINTVL	18	/* keepalive probe interval */
#define	TCPCTL_KEEPCNT		19	/* keepalive count */
#define	TCPCTL_SLOWHZ		20	/* PR_SLOWHZ (read-only) */
#define	TCPCTL_NEWRENO		21	/* NewReno Congestion Control */
#define TCPCTL_LOG_REFUSED	22	/* Log refused connections */
#if 0	/*obsoleted*/
#define	TCPCTL_RSTRATELIMIT	23	/* RST rate limit */
#endif
#define	TCPCTL_RSTPPSLIMIT	24	/* RST pps limit */
#define	TCPCTL_STAT		25	/* TCP statistics */
#define	TCPCTL_MAXID		26

#define	TCPCTL_NAMES { \
	{ 0, 0 }, \
	{ "rfc1323",	CTLTYPE_INT }, \
	{ "sendspace",	CTLTYPE_INT }, \
	{ "recvspace",	CTLTYPE_INT }, \
	{ "mssdflt",	CTLTYPE_INT }, \
	{ "syn_cache_limit", CTLTYPE_INT }, \
	{ "syn_bucket_limit", CTLTYPE_INT }, \
	{ "syn_cache_interval", CTLTYPE_INT },\
	{ "init_win", CTLTYPE_INT }, \
	{ "mss_ifmtu", CTLTYPE_INT }, \
	{ "sack", CTLTYPE_INT }, \
	{ "win_scale", CTLTYPE_INT }, \
	{ "timestamps", CTLTYPE_INT }, \
	{ "compat_42", CTLTYPE_INT }, \
	{ "cwm", CTLTYPE_INT }, \
	{ "cwm_burstsize", CTLTYPE_INT }, \
	{ "ack_on_push", CTLTYPE_INT }, \
	{ "keepidle",	CTLTYPE_INT }, \
	{ "keepintvl",	CTLTYPE_INT }, \
	{ "keepcnt",	CTLTYPE_INT }, \
	{ "slowhz",	CTLTYPE_INT }, \
	{ "newreno",	CTLTYPE_INT }, \
	{ "log_refused",CTLTYPE_INT }, \
	{ 0, 0 }, \
	{ "rstppslimit", CTLTYPE_INT }, \
	{ "stat", CTLTYPE_STRUCT }, \
}

#endif /* _NETINET_TCP_VAR_H_ */
