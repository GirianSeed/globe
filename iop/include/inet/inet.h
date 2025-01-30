/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/* 
 *                      I/O Processor Library
 *                          Version 1.05
 *                           Shift-JIS
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         inet - inet.h
 *                           INET API
 *
 * $Id: inet.h,v 1.8 2001/09/04 09:18:18 ksh Exp $
 */

#if !defined(_INET_H)
#define _INET_H

#if defined(__cplusplus)
extern "C" {
#endif

/* << INET API >> */

typedef struct sceInetAddress {
	int reserved;	/* must be zero */
	char data[12];	/* IP address (4 bytes) + reserved (8 bytes) */
} sceInetAddress_t;

typedef struct sceInetParam {
	int type;
	int local_port;
	struct sceInetAddress remote_addr;
	int remote_port;
	int reserved[9];	/* must be zero */
} sceInetParam_t;

/* type for struct InetParam */
#define sceINETT_DGRAM		0x0	/* UDP */
#define sceINETT_CONNECT	0x1	/* TCP, Active-Open */
#define sceINETT_LISTEN		0x2	/* TCP, Passive-Open */
#define sceINETT_RAW		0x3	/* IP */

/* port for struct InetParam */
#define sceINETP_AUTO		-1
#define sceINETP_ANY		0

typedef struct sceInetInfo {
	int cid;
	int proto;
	int recv_queue_length;
	int send_queue_length;
	struct sceInetAddress local_adr;
	int local_port;
	struct sceInetAddress remote_adr;
	int remote_port;
	int state;
	int reserved[4];
} sceInetInfo_t;

/* proto for sceInetInfo */
#define sceINETI_PROTO_TCP	1	/* CONNECT,LISTEN */
#define sceINETI_PROTO_UDP	2	/* DGRAM */
#define sceINETI_PROTO_IP	3	/* RAW */

/* state for sceInetInfo */
#define sceINETI_STATE_UNKNOWN		0	/* all */
#define sceINETI_STATE_CLOSED		1	/* all */
#define sceINETI_STATE_CREATED		2	/* UDP */
#define sceINETI_STATE_OPENED		3	/* UDP,RAW */
#define sceINETI_STATE_LISTEN		4	/* CONNECT,LISTEN */
#define sceINETI_STATE_SYN_SENT		5	/* CONNECT,LISTEN */
#define sceINETI_STATE_SYN_RECEIVED	6	/* CONNECT,LISTEN */
#define sceINETI_STATE_ESTABLISHED	7	/* CONNECT,LISTEN */
#define sceINETI_STATE_FIN_WAIT_1	8	/* CONNECT,LISTEN */
#define sceINETI_STATE_FIN_WAIT_2	9	/* CONNECT,LISTEN */
#define sceINETI_STATE_CLOSE_WAIT	10	/* CONNECT,LISTEN */
#define sceINETI_STATE_CLOSING		11	/* CONNECT,LISTEN */
#define sceINETI_STATE_LAST_ACK		12	/* CONNECT,LISTEN */
#define sceINETI_STATE_TIME_WAIT	13	/* CONNECT,LISTEN */

int sceInetName2Address(int flags, struct sceInetAddress *paddr, char *name,
	int ms, int nretry);
int sceInetAddress2String(char *buf, int len, struct sceInetAddress *paddr);
int sceInetAddress2Name(int flags, char *buf, int len,
	struct sceInetAddress *paddr, int ms, int nretry);
int sceInetCreate(struct sceInetParam *param);
int sceInetOpen(int cid, int ms);
int sceInetClose(int cid, int ms);
int sceInetRecv(int cid, void *ptr, int count, int *pflags, int ms);
int sceInetRecvFrom(int cid, void *ptr, int count, int *pflags,
	struct sceInetAddress *remote_adr, int *premote_port, int ms);
int sceInetSend(int cid, void *ptr, int count, int *pflags, int ms);
int sceInetSendTo(int cid, void *ptr, int count, int *pflags,
	struct sceInetAddress *remote_adr, int remote_port, int ms);
int sceInetAbort(int cid, int flags);
int sceInetControl(int cid, int code, void *ptr, int len);

/* flags for InetRecv/InetSend */
#define sceINETF_URG		0x02	/* TCP Urgent (Recv-Out, Send-In) */
#define sceINETF_FIN		0x04	/* TCP Fin    (Recv-Out) */
#define sceINETF_TIM		0x08	/* Timeout    (Recv-Out, Send-Out) */
#define sceINETF_ABT		0x20	/* Abort      (Recv-Out, Send-Out) */

/* The followings two bits are removed from INET specifications.
	PSH - always appended by automatically by INET.
	ONE - always true.
 */
#if defined(obsoleted)
#define sceINETF_PSH		0x01	/* TCP Push   (Recv-Out, Send-In) */
#define sceINETF_ONE		0x10	/* One-Byte   (Recv-In) */
#endif	/* obsoleted */

/* flags for sceInetName2Address() */
#define sceInetN2AF_NoDot	0x01	/* disable dot format conversion */
#define sceInetN2AF_NoHosts	0x02	/* disable refer to hosts table */
#define sceInetN2AF_NoDNS	0x04	/* disable using DNS */

/* code for sceInetControl() */
#define sceINETC_CODE_GET_INFO	1
#define sceINETC_CODE_GET_FLAGS	2
#define sceINETC_CODE_SET_FLAGS	3

/* flags for sceINETC_{SET,GET}_FLAGS */
#define sceINETC_FLAGS_NODELAY	0x01	/* TCP no delay */

/* error codes - InetXXX() */
#define sceINETE_OK				0
#define sceINETE_TIMEOUT			-500
#define sceINETE_ABORT				-501
#define sceINETE_BUSY				-502
#define sceINETE_LINK_DOWN			-503
#define sceINETE_INSUFFICIENT_RESOURCES		-504
#define sceINETE_LOCAL_SOCKET_UNSPECIFIED	-505
#define sceINETE_FOREIGN_SOCKET_UNSPECIFIED	-506
#define sceINETE_CONNECTION_ALREADY_EXISTS	-507
#define sceINETE_CONNECTION_DOES_NOT_EXIST	-508
#define sceINETE_CONNECTION_CLOSING		-509	/* TCP */
#define sceINETE_CONNECTION_RESET		-510	/* TCP */
#define sceINETE_CONNECTION_REFUSED		-511	/* TCP */
#define sceINETE_INVALID_ARGUMENT		-512
#define sceINETE_INVALID_CALL			-513
#define sceINETE_NO_ROUTE			-514

/* << INET Control I/F >> */

typedef struct sceInetRoutingEntry {
	struct sceInetAddress dstaddr;
	struct sceInetAddress gateway;
	struct sceInetAddress genmask;
	int flags;
	int mss;
	int window;
	char interface[8 + 1];
} sceInetRoutingEntry_t;

/* flags of struct sceInetRoutingEntry */
#define sceInetRoutingF_Up		0x01	/* route is up */
#define sceInetRoutingF_Host		0x02	/* target is host */
#define sceInetRoutingF_Gateway		0x04	/* use gateway */
#define sceInetRoutingF_Dynamic		0x08	/* dynamically installed */
#define sceInetRoutingF_Modified	0x10	/* modified */

/* code of sceInetInterfaceControl() */
#define sceInetCC_GetInterfaceName	0x00000000	/* interface */
#define sceInetCC_GetModuleName		0x00000001	/* module_name */
#define sceInetCC_GetVendorName		0x00000002	/* vendor_name */
#define sceInetCC_GetDeviceName		0x00000003	/* device_name */
#define sceInetCC_GetBusType		0x00000004	/* bus_type */
#define sceInetCC_GetBusLoc		0x00000005	/* bus_loc */
#define sceInetCC_GetProt		0x00000006	/* prot */
#define sceInetCC_GetImpl		0x00000007	/* impl */
#define sceInetCC_GetFlags		0x00000008	/* flags */
#define sceInetCC_GetAddress		0x00000009	/* ip_addr */
#define sceInetCC_GetNetmask		0x0000000a	/* ip_mask */
#define sceInetCC_GetBroadcast		0x0000000b	/* broad_adr */
#define sceInetCC_GetMTU		0x0000000c	/* MTU */
#define sceInetCC_GetHWaddr		0x0000000d	/* hw_addr */
#define sceInetCC_GetDHCPHostName	0x0000000e	/* DHCP hostname */
#define sceInetCC_GetDHCPFlags		0x0000000f	/* dhcp_flags */

#define sceInetCC_NETDEV		0x80000000
#define sceInetCC_MODEM			0x40000000

// 0x00000000 - 0x7fffffff  INET layer
// 0x80000000 - 0x8fffffff  NETDEV layer (common)
// 0x90000000 - 0xbfffffff  NETDEV layer (depends by implementation) 
// 0xc0000000 - 0xcfffffff  MODEM layer (common)
// 0xd0000000 - 0xffffffff  MODEM layer (depends by implementation) 

int sceInetGetInterfaceList(int *interface_id_list, int n);
int sceInetInterfaceControl(int interface_id, int code, void *ptr, int len);
int sceInetGetRoutingTable(struct sceInetRoutingEntry *p, int n);
int sceInetGetNameServers(struct sceInetAddress *paddr, int n);
int sceInetChangeThreadPriority(int prio);
int sceInetGetLog(char *buf, int len, int ms);
int sceInetAbortLog(void);

#if defined(__cplusplus)
}
#endif

#endif	/* _INET_H */
