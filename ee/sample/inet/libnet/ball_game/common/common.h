/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         ball_game - common.h
 */

#ifndef __COMMON_H__
#define __COMMON_H__


#define Tcp_PORT_Login 8920
#define Udp_PORT_State 8921

#define MSG_REG 0x1
#define MSG_POSGET 0x2
#define MSG_RESET 0x4

#define POS_NUM		0x1
#define POS_DATA	0x2

struct ctlmsg {
	char type;
	char padding;
	u_short udp_port;
	union {
		u_int seq;
		u_int rand;
	} num;
};

struct pos {
	float x_pos;
	float y_pos;
	float z_pos;
};

struct pos_header {
	char type;
	char padding[3];
	u_short n_cli;
	u_short n_ball;
	u_int seq;
};

struct pos_data {
	char padding[2];
	u_short n_cli;
	struct pos pos1;
	struct pos pos2;
};

#define TcpPut2byte(p, v)   ({  \
	u_char *_p = (u_char *)(p);     \
	u_short _v = (u_short)(v);  \
	*(_p + 0) = _v >>  8;       \
	*(_p + 1) = _v >>  0;       \
	_p + 2;             \
})

#define TcpPut4byte(p, v)   ({  \
	u_char *_p = (u_char *)(p); \
	u_long _v = (u_long)(v);    \
	*(_p + 0) = _v >> 24;       \
	*(_p + 1) = _v >> 16;       \
	*(_p + 2) = _v >>  8;       \
	*(_p + 3) = _v >>  0;       \
	_p + 4;             \
})

#define TcpGet2byte(p)  ({      \
	u_char *_p = (u_char *)(p); \
	(*(_p + 0) <<  8)       \
		| (*(_p + 1) <<  0);    \
})

#define TcpGet4byte(p)  ({      \
	u_char *_p = (u_char *)(p); \
	(*(_p + 0) << 24)       \
		| (*(_p + 1) << 16) \
		| (*(_p + 2) <<  8) \
		| (*(_p + 3) <<  0);    \
})


#define dump_byte(p, size)	({ \
	int i; \
	for(i = 0; i < (size); i++) \
		printf("%02x", *((char *)(p) + i) & 0xff); \
	printf("\n"); \
})

#ifdef BIG_ENDIAN
#if 0
#define myntohs(sv)   ({          \
	u_short _sv = (sv);         \
	(u_short)((_sv << 8) | (_sv >> 8)); \
	})

#define myntohl(lv)   (u_int)({       \
	u_long _lv = (lv);           \
	(myntohs(_lv) << 16) | myntohs(_lv >> 16);  \
	})

#define myntohf(f) ({ \
	u_long l; \
	l = ntohl(*(u_long *)&(f)); \
	memcpy(&(f), &l, sizeof(float)); \
})
#endif
#else

#define myntohs(sv)   ({          \
	u_short _sv = (sv);         \
	(u_short)((_sv << 8) | (_sv >> 8)); \
	})

#define myntohl(lv)   (u_int)({       \
	u_int _lv = (lv);           \
	(myntohs(_lv) << 16) | myntohs(_lv >> 16);  \
	})

#define ntohf(f) ({ \
	u_int l; \
	l = ntohl(*(u_int *)&(f)); \
	memcpy(&(f), &l, sizeof(float)); \
})

#define myhtons(s) myntohs(s)
#define myhtonl(l) myntohl(l)
#define htonf(f) ntohf(f)

#endif


#endif // __COMMON_H__
