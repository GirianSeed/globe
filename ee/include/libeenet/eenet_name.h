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
 *                       <libeenet - eenet_name.h>
 *              <header for function/structure name chnage>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Jul,02,2002     komaki      first version
 */

#ifndef _EENET_NAME_H_
#define _EENET_NAME_H_

/* functions */

#define accept sceEENetAccept
#define bind sceEENetBind
#define connect sceEENetConnect
#define getpeername sceEENetGetpeername
#define getsockname sceEENetGetsockname
#define getsockopt sceEENetGetsockopt
#define listen sceEENetListen
#define recv sceEENetRecv
#define recvfrom sceEENetRecvfrom
#define recvmsg sceEENetRecvmsg
#define send sceEENetSend
#define sendto sceEENetSendto
#define sendmsg sceEENetSendmsg
#define setsockopt sceEENetSetsockopt
#define shutdown sceEENetShutdown
#define socket sceEENetSocket
#define gettimeofday sceEENetGettiemofday
#define poll sceEENetPoll
#define select sceEENetSelect
#define htonl sceEENetHtonl
#define htons sceEENetHtons
#define ntohl sceEENetNtohl
#define ntohs sceEENetNtohs
#define time sceEENetTime
#define ether_ntoa sceEENetEtherNtoa
#define ether_aton sceEENetEtherAton
#define getaddrinfo sceEENetGetaddrinfo
#define freeaddrinfo sceEENetFreeaddrinfo
#define gethostbyaddr sceEENetGethostbyaddr
#define gethostbyname sceEENetGethostbyname
#define gethostbyname2 sceEENetGethostbyname2
#define getifaddrs sceEENetGetifaddrs
#define freeifaddrs sceEENetFreeifaddrs
#define getnameinfo sceEENetGetnameinfo
#define if_indextoname sceEENetIfIndextoname
#define if_nameindex sceEENetIfNameindex
#define if_freenameindex sceEENetIfFreeanameindex
#define if_nametoindex sceEENetIfNametoindex
#define inet_lnaof sceEENetInetLnaof
#define inet_makeaddr sceEENetInetMakeaddr
#define inet_netof sceEENetInetNetof
#define inet_network sceEENetInetNetwork
#define inet_ntoa sceEENetInetNtoa
#define inet_ntop sceEENetInetNtop
#define inet_addr sceEENetInetAddr
#define inet_aton sceEENetInetAton
#define inet_pton sceEENetInetPton
#define link_addr sceEENetLinkAddr
#define link_ntoa sceEENetLinkNtoa
#define sleep sceEENetSleep
#define usleep sceEENetUsleep

/* structures */

#define ifaddrs sceEENetIfaddrs
#define hostent sceEENetHostent
#define addrinfo sceEENetAddrinfo
#define pollfd sceEENetPollfd
#define linger sceEENetLinger
#define sockaddr sceEENetSockaddr
#define sockaddr_storage sceEENetSockaddrStorage
#define msghdr sceEENetMsghdr
#define cmsghdr sceEENetCmsghdr
#define iovec sceEENetIovec
#define bpf_program sceEENetBpfProgram
#define bpf_sta sceEENetBpfStat
#define bpf_version sceEENetBpfVersion
#define bpf_timeval sceEENetBpfTimeval
#define bpf_hdr sceEENetBpfHdr
#define bpf_insn sceEENetBpfInsn
#define if_clonereq sceEENetIfClonereq
#define if_data sceEENetIfData
#define if_msghdr sceEENetIfMsghdr
#define ifa_msghdr sceEENetIfaMsghdr
#define if_announcemsghdr sceEENetIfAnnouncemsghdr
#define ifreq sceEENetIfreq
#define ifaliasreq sceEENetIfaliasreq
#define ifmediareq sceEENetIfmediareq
#define ifdrv sceEENetIfdrv
#define ifconf sceEENetIfconf
#define if_laddrreq sceEENetIfLaddrreq
#define if_nameindex sceEENetIfNameindex
#define arphdr sceEENetArphdr
#define arpreq sceEENetArpreq
#define sockaddr_dl sceEENetSockaddrDl
#define ether_addr sceEENetEtherAddr
#define ether_header sceEENetEtherHeader
#define rt_metrics sceEENetRtMetrics
#define rt_msghdr sceEENetRtMsghdr
#define rt_addrinfo sceEENetRtAddrinfo
#define in_addr sceEENetInAddr
#define sockaddr_in sceEENetSockaddrIn
#define ip_opts sceEENetIpOpts
#define ip_mreq sceEENetIpMreq
#define in6_addr sceEENetIn6Addr
#define sockaddr_in6 sceEENetSockaddrIn6
#define ipv6_mreq sceEENetIpv6Mreq
#define in6_pktinfo sceEENetIn6Pktinfo

#endif /* _EENET_NAME_H_ */
