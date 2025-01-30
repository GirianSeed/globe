/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                   <libeenet - netglue_eenet.c>
 *                   <netglue library for libeenet>
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0        2002/07/17  komaki      first version
 */

#include <libeenet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/errno.h>

#include <netglue_eenet.h>
#define sceNetGlueDisableSocketSymbolAliases
#include <netglue.h>


/* netglue.h */

int *__sceNetGlueErrnoLoc(void){
	switch(*__sceEENetThreadErrno()){
	case EINTR:
		*__sceEENetThreadErrno() = ECONNABORTED;
		break;
	case EADDRNOTAVAIL:
	case EINVAL:
		*__sceEENetThreadErrno() = EINVAL;
		break;
	case ENOTCONN:
	case ESHUTDOWN:
		*__sceEENetThreadErrno() = ENOTCONN;
		break;
	}

	return(__sceEENetThreadErrno());
}

int *__sceNetGlueHErrnoLoc(void){
	return(__sceEENetThreadHErrno());
}

int sceNetGlueAbort(int s){
	int ret;
	ret = sceEENetSocketAbort(s);
	if(0 == ret)
		sceEENetCloseWithRST(s);
	return(ret);
}

int sceNetGlueAbortResolver(int tid){
	return(sceEENetThreadAbort(tid));
}

int sceNetGlueThreadInit(int tid){
	return(0);
}

int sceNetGlueThreadTerminate(int tid){
	sceEENetFreeThreadinfo(tid);
	return(0);
}

/* netglue/netdb.h */

sceNetGlueHostent_t *sceNetGlueGethostbyaddr(const char *adr, int n, int type){
	return((sceNetGlueHostent_t *)sceEENetGethostbyaddr(adr, n, type));
}

sceNetGlueHostent_t *sceNetGlueGethostbyname(const char *name){
	return((sceNetGlueHostent_t *)sceEENetGethostbyname(name));
}

/* netglue/arpa/inet.h */

u_int sceNetGlueInetAddr(const char *cp){
	return(sceEENetInetAddr(cp));
}

int sceNetGlueInetAton(const char *cp, sceNetGlueInAddr_t *addr){
	return(sceEENetInetAton(cp, (struct sceEENetInAddr *)addr));
}

u_int sceNetGlueInetLnaof(sceNetGlueInAddr_t in){
	struct sceEENetInAddr sin;
	sin.s_addr = in.s_addr;
	return(sceEENetInetLnaof(sin));
}

sceNetGlueInAddr_t sceNetGlueInetMakeaddr(u_int net, u_int host){
	sceNetGlueInAddr_t in;
 	struct sceEENetInAddr sin = sceEENetInetMakeaddr(net, host);
	in.s_addr = sin.s_addr;
	return(in);	
}

u_int sceNetGlueInetNetof(sceNetGlueInAddr_t in){
	struct sceEENetInAddr sin;
	sin.s_addr = in.s_addr;
	return(sceEENetInetNetof(sin));
}

u_int sceNetGlueInetNetwork(const char *cp){
	return(sceEENetInetNetwork(cp));
}

char *sceNetGlueInetNtoa(sceNetGlueInAddr_t in){
	struct sceEENetInAddr sin;
	sin.s_addr = in.s_addr;
	return(sceEENetInetNtoa(sin));
}

/* netglue/netinet/in.h */

u_int sceNetGlueHtonl(u_int hl){
	return(sceEENetHtonl(hl));
}

u_short sceNetGlueHtons(u_short hs){
	return(sceEENetHtons(hs));
}

u_int sceNetGlueNtohl(u_int nl){
	return(sceEENetNtohl(nl));
}
	
u_short sceNetGlueNtohs(u_short ns){
	return(sceEENetNtohs(ns));
}

/* netglue/sys/socket.h */

int sceNetGlueAccept(int s, sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceEENetAccept(s, (struct sceEENetSockaddr *)addr,
		(socklen_t *)paddrlen));
}

int sceNetGlueBind(int s, const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceEENetBind(s, (struct sceEENetSockaddr *)addr,
		(socklen_t) addrlen));
}

int sceNetGlueConnect(int s, const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceEENetConnect(s, (struct sceEENetSockaddr *)addr,
		(socklen_t) addrlen));
}

int sceNetGlueGetpeername(int s, sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceEENetGetpeername(s, (struct sceEENetSockaddr *)addr,
		(socklen_t *)paddrlen));
}

int sceNetGlueGetsockname(int s , sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceEENetGetsockname(s, (struct sceEENetSockaddr *)addr,
		(socklen_t *)paddrlen));
}

int sceNetGlueGetsockopt(int s, int level, int optname, void *optval,
			sceNetGlueSocklen_t *optlen){
	return(sceEENetGetsockopt(s, level, optname, optval,
		(socklen_t *)optlen));
}

int sceNetGlueListen(int s, int backlog){
	return(sceEENetListen(s, backlog));
}

ssize_t	sceNetGlueRecv(int s, void *buf, size_t len, int flags){
	return(sceEENetRecv(s, buf, len, flags));
}

ssize_t	sceNetGlueRecvfrom(int s, void *buf, size_t len, int flags,
			sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceEENetRecvfrom(s, buf, len, flags,
		(struct sceEENetSockaddr *)addr, (socklen_t *)paddrlen));
}

ssize_t	sceNetGlueSend(int s, const void *buf, size_t len, int flags){
	return(sceEENetSend(s, buf, len, flags));
}

ssize_t	sceNetGlueSendto(int s, const void *buf, size_t len, int flags,
			const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceEENetSendto(s, buf, len, flags,
		(const struct sceEENetSockaddr *)addr,
		(socklen_t)addrlen));
}

int sceNetGlueSetsockopt(int s, int level, int optname, const void *optval,
			sceNetGlueSocklen_t optlen){
	return(sceEENetSetsockopt(s, level, optname, optval,
		(socklen_t)optlen));
}

int sceNetGlueShutdown(int s, int how){
	int ret;
	if((ret = sceEENetShutdown(s, how)) < 0)
		return ret;
	return(sceEENetClose(s));
}

int sceNetGlueSocket(int family, int type, int protocol){
	return(sceEENetSocket(family, type, protocol));
}
