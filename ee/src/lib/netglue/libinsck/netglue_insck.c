/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          netglue Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0        2002/04/16  komaki      first version
 *       1.1.0        2002/11/26  ksh         timeout for recv(), recvfrom()
 *       1.1.1        2003/03/25  ksh         getsockopt(), setsockopt()
 *       1.2.0        2003/10/27  ksh         timeout for 1.1.0 removed
 */

#include <libinsck.h>
#define sceNetGlueDisableSocketSymbolAliases
#include <netglue.h>

/* netglue.h */

int *__sceNetGlueErrnoLoc(void){
	return(__sceInsockErrnoLoc());
}

int *__sceNetGlueHErrnoLoc(void){
	return(__sceInsockHErrnoLoc());
}

int sceNetGlueAbort(int s){
	return(sceInsockAbort(s, 0));
}

int sceNetGlueAbortResolver(int tid){
	(void)tid;
	sceInsockAbortGethostbyname(NULL);
	sceInsockAbortGethostbyaddr(NULL, 0, 0);
	return(0);
}

int sceNetGlueThreadInit(int s){
	(void)s;
	return(0);
}

int sceNetGlueThreadTerminate(int s){
	return(sceInsockTerminate(s));
}

/* netglue/netdb.h */

sceNetGlueHostent_t *sceNetGlueGethostbyaddr(const char *adr, int n, int type){
	return((sceNetGlueHostent_t *)sceInsockGethostbyaddr(adr, n, type));
}

sceNetGlueHostent_t *sceNetGlueGethostbyname(const char *name){
	return((sceNetGlueHostent_t *)sceInsockGethostbyname(name));
}

/* netglue/arpa/inet.h */

u_int sceNetGlueInetAddr(const char *cp){
	return(sceInsockInetAddr(cp));
}

int sceNetGlueInetAton(const char *cp, sceNetGlueInAddr_t *addr){
	return(sceInsockInetAton(cp, (sceInsockInAddr_t *)addr));
}

u_int sceNetGlueInetLnaof(sceNetGlueInAddr_t in){
	sceInsockInAddr_t sin;
	sin.s_addr = in.s_addr;
	return(sceInsockInetLnaof(sin));
}

sceNetGlueInAddr_t sceNetGlueInetMakeaddr(u_int net, u_int host){
	sceNetGlueInAddr_t in;
 	sceInsockInAddr_t sin = sceInsockInetMakeaddr(net, host);
	in.s_addr = sin.s_addr;
	return(in);	
}

u_int sceNetGlueInetNetof(sceNetGlueInAddr_t in){
	sceInsockInAddr_t sin;
	sin.s_addr = in.s_addr;
	return(sceInsockInetNetof(sin));
}

u_int sceNetGlueInetNetwork(const char *cp){
	return(sceInsockInetNetwork(cp));
}

char *sceNetGlueInetNtoa(sceNetGlueInAddr_t in){
	sceInsockInAddr_t sin;
	sin.s_addr = in.s_addr;
	return(sceInsockInetNtoa(sin));
}

const char *sceNetGlueInetNtop(int af, const void *src, char *dst, size_t size){
	return(sceInsockInetNtop(af, src, dst, size));
}

/* netglue/netinet/in.h */

u_int sceNetGlueHtonl(u_int hl){
	return(sceInsockHtonl(hl));
}

u_short sceNetGlueHtons(u_short hs){
	return(sceInsockHtons(hs));
}

u_int sceNetGlueNtohl(u_int nl){
	return(sceInsockNtohl(nl));
}
	
u_short sceNetGlueNtohs(u_short ns){
	return(sceInsockNtohs(ns));
}

/* netglue/sys/socket.h */

int sceNetGlueAccept(int s, sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceInsockAccept(s, (sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t *)paddrlen));
}

int sceNetGlueBind(int s, const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceInsockBind(s, (sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t) addrlen));
}

int sceNetGlueConnect(int s, const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceInsockConnect(s, (sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t) addrlen));
}

int sceNetGlueGetpeername(int s, sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceInsockGetpeername(s, (sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t *)paddrlen));
}

int sceNetGlueGetsockname(int s , sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceInsockGetsockname(s, (sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t *)paddrlen));
}

int sceNetGlueGetsockopt(int s, int level, int optname, void *optval,
			sceNetGlueSocklen_t *optlen){
	int ival;
	int ilen;
	int ret;

	if (level == IPPROTO_IP) {
		if (optname == IP_MULTICAST_TTL || optname == IP_MULTICAST_LOOP) {
			if (optval != NULL && optlen != NULL && *optlen == sizeof(char)) {
				ilen = sizeof(int);
				ret = sceInsockGetsockopt(s, level, optname, &ival,
					(sceInsockSocklen_t *)&ilen);
				if (ret == 0) {
					*(char *)optval = (char)ival;
				}
				return(ret);
			}
		}
	}
	return(sceInsockGetsockopt(s, level, optname, optval,
		(sceInsockSocklen_t *)optlen));
}

int sceNetGlueListen(int s, int backlog){
	return(sceInsockListen(s, backlog));
}

ssize_t	sceNetGlueRecv(int s, void *buf, size_t len, int flags){
	return(sceInsockRecv(s, buf, len, flags));
}

ssize_t	sceNetGlueRecvfrom(int s, void *buf, size_t len, int flags,
			sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t *paddrlen){
	return(sceInsockRecvfrom(s, buf, len, flags,
		(sceInsockSockaddr_t *)addr, (sceInsockSocklen_t *)paddrlen));
}

ssize_t	sceNetGlueSend(int s, const void *buf, size_t len, int flags){
	return(sceInsockSend(s, buf, len, flags));
}

ssize_t	sceNetGlueSendto(int s, const void *buf, size_t len, int flags,
			const sceNetGlueSockaddr_t *addr,
			sceNetGlueSocklen_t addrlen){
	return(sceInsockSendto(s, buf, len, flags,
		(const sceInsockSockaddr_t *)addr,
		(sceInsockSocklen_t)addrlen));
}

int sceNetGlueSetsockopt(int s, int level, int optname, const void *optval,
			sceNetGlueSocklen_t optlen){
	int ival;
	int ret;

	if (level == IPPROTO_IP) {
		if (optname == IP_MULTICAST_TTL || optname == IP_MULTICAST_LOOP) {
			if (optval != NULL && optlen == sizeof(char)) {
				ival = *(char *)optval;
				optlen = sizeof(int);
				ret = sceInsockSetsockopt(s, level, optname, &ival,
					(sceInsockSocklen_t)optlen);
				return(ret);
			}
		}
	}
	return(sceInsockSetsockopt(s, level, optname, optval,
		(sceInsockSocklen_t)optlen));
}

int sceNetGlueShutdown(int s, int how){
	return(sceInsockShutdown(s, how));
}

int sceNetGlueSocket(int family, int type, int protocol){
	return(sceInsockSocket(family, type, protocol));
}
