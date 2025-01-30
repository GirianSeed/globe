/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
// Version 0.00
// ASCII
// --------------------------------------------------------------------
// 0.00           06/18/99        koji        1st version

#ifndef _PDRIVER_H
#define _PDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

#define PROTO_SAMPLE	0xe000
#define NODE_EE		'E'
#define NODE_HOST	'H'

#define BUFSZ 0x10000           // must be multiple of 64 for cache

//      3                   2                   1                  
//    1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                            result                             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                                         Data ...  |
//                                                      -+-+-+-+-+-+-+


#define UNCACHED(p) ((char *)((unsigned int)p | 0x20000000))

// function prototypes
int InitProtocolDriver(void);
int SystemOnHost(char *command, char *result);

#ifdef __cplusplus
}
#endif

#endif _PDRIVER_H

// Local variables:
// tab-width: 4
// End:
// vi:set tabstop=4:
