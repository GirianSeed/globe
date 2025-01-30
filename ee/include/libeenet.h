/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <libeenet - libeenet.h>
 *                       <eenet general header>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Dec,03,2001     komaki      first version
 */

#ifndef _LIBEENET_H_
#define _LIBEENET_H_

#include <sys/types.h>
#include <libeenet/eenettypes.h>
#include <libeenet/eeneterrno.h>
#include <libeenet/eenet_name.h>
#include <libeenet/ent_smap.h>
#include <libeenet/ent_eth.h>
#include <libeenet/ent_ppp.h>
#include <libeenet/net/bpf.h>
#include <libeenet/netdb.h>
#include <sys/time.h>

int *__sceEENetThreadErrno(void);
int *__sceEENetThreadHErrno(void);

#define	sceEENetErrno		(*__sceEENetThreadErrno())
#define	sceEENetHErrno		(*__sceEENetThreadHErrno())

/*
 * malloc statistics
 */
typedef struct sceEENetMallocStat {
	u_int	nmalloc;	/* # of sceEENetMalloc() called times */
	u_int	nrealloc;	/* # of sceEENetRealloc() called times */
	u_int	nfree;		/* # of sceEENetFree() called times */
	size_t	upages;		/* # of pages in use */
	size_t	ubytes;		/* # of bytes in use */
} sceEENetMallocStat_t;

#ifdef __cplusplus
extern "C" {
#endif

int sceEENetInit(int malloc_size, int eenet_tpl, int hsync_mode, int stacksize, int priority);
int sceEENetTerm(void);

int sceEENetGetDNSAddr(char *nameserver1, char *nameserver2);
int sceEENetSetDNSAddr(const char *nameserver1, const char *nameserver2);

int sceEENetGetDeviceInfo(const char *ifname, char *vendor_name,
	int vendor_len, char *product_name, int product_len);

/* BSD socket API */
int sceEENetSelect(int nfds, fd_set *readfds, fd_set *writefds,
	fd_set *exceptfds, struct timeval *timeout);
int sceEENetClose(int s);
int sceEENetCloseWithRST(int s);
int sceEENetAbort(int s);
int sceEENetGetaddrinfoTO(const char *hostname, const char *servname,
	const struct sceEENetAddrinfo *hints, struct sceEENetAddrinfo **res,
	int retrans, int retry);
struct sceEENetHostent *sceEENetGethostbynameTO(const char *name, int retrans,
	int retry);
struct sceEENetHostent *sceEENetGethostbyname2TO(const char *name, int af,
	int retrans, int retry);
struct sceEENetHostent *sceEENetGethostbyaddrTO(const char *addr, int len,
	int af, int retrans, int retry);
int sceEENetGetnameinfoTO(const struct sceEENetSockaddr *sa,
	socklen_t salen, char *host, size_t hostlen, char *serv,
	size_t servlen, int flags, int retrans, int retry);
void sceEENetSetResolvTO(int retrans, int retry);
void sceEENetGetResolvTO(int *retrans, int *retry);

unsigned int sceEENetHtonl(unsigned int host32);
unsigned short sceEENetHtons(unsigned short host16);
unsigned int sceEENetNtohl(unsigned int net32);
unsigned short sceEENetNtohs(unsigned short net16);

/* utilities */
time_t sceEENetTime(time_t *t);
int sceEENetGettimeofday(struct timeval *tp, struct timezone *tzp);
unsigned int sceEENetSleep(unsigned int seconds);
int sceEENetUsleep(unsigned int seconds);

/* internal malloc */
void *sceEENetMalloc(size_t);
void sceEENetFree(void *);
void *sceEENetRealloc(void *, size_t);
void *sceEENetCalloc(size_t, size_t);
void *sceEENetMemalign(size_t, size_t);
void sceEENetMallocStat(struct sceEENetMallocStat *);

/* log */
int sceEENetOpenLog(const char *, int, u_short);
int sceEENetCloseLog(void);
void sceEENetSetLogLevel(int);
int sceEENetGetLogLevel(void);
#define EENET_LOG_EMERG		0
#define EENET_LOG_ALERT		1
#define EENET_LOG_CRIT		2
#define EENET_LOG_ERR		3
#define EENET_LOG_WARNING	4
#define EENET_LOG_NOTICE	5
#define EENET_LOG_INFO		6
#define EENET_LOG_DEBUG		7

/* for debug */
typedef struct sceEENetTcpdumpParam {
	int term_sid;
	int killwait_sid;
	int tid;
} sceEENetTcpdumpParam_t;

int sceEENetTcpdump(const char *ifname, const char *WFileName,
	char *filebuf, int fbsize, int promisc,
	struct sceEENetBpfProgram *fcode, int snaplen, int cnt,
	char *stackptr, int stacksize, int priority, sceEENetTcpdumpParam_t *daemonp);
int sceEENetTcpdumpTerm(sceEENetTcpdumpParam_t *daemonp);

int sceEENetDhcpClient(const char *ifname, const char *hostname, char *stackptr, int stacksize, int priority, int timeout);
int sceEENetPppoeClient(const char *ifname, const char *authname, const char *authkey, int authtype, char *stackptr, int stacksize, int priority, int timeout);
int sceEENetRtsold(const char *ifname, char *stackptr, int stacksize, int priority, int timeout);

int sceEENetIfconfig(int argc, ...);
int sceEENetRoute(int argc, ...);
int sceEENetNetstat(int argc, ...);

#ifdef __cplusplus
}
#endif

#endif /* _LIBEENET_H_ */
