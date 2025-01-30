/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - ball_game -
 *
 *                         Version 1.0.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     Name : udp_game.h
 *                     Description : 
 *
 *
 *       Version        Date           Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0          Sep,29,2000    munekis     first verison
 */

#ifndef __UDP_GAME_H__
#define __UDP_GAME_H__

#define BUFSIZE 512
#define RPCSIZE BUFSIZE*4

#define MAX_POS_DATA	    2048

#define STACKSIZE 0x00010000
#define PRIO       52

#define SUNIT       0x01
#define PACKETSIZE  (0x100*SUNIT)
#define WORKSIZE    (0x80)
#define WORKBASE    0x70000000
#define SUPPER      (WORKBASE+WORKSIZE)
#define SLOWER      (WORKBASE+WORKSIZE+PACKETSIZE)

#define GRAPH_TH	0x01
#define COM_TH		0x02

/****
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   224
#define OFFX        (((4096-SCREEN_WIDTH)/2)<<4)
#define OFFY        (((4096-SCREEN_HEIGHT)/2)<<4)
***/

extern struct sceInetAddress myaddr;
extern char paddr[];
extern int com_init_sema;
extern int sync_sema;
extern int graph_sema;
extern int com_sema;
extern int v_sema;
extern int n_ball;
extern int n_cli;
extern int n_all_clients;
extern int syncv;
extern Object_t *new_spheres;

int get_connection(sceSifMClientData *cd, u_int *buf);
int game_reset(sceSifMClientData *cd, u_int *buf, int *i_data);
int game_get_num(sceSifMClientData *cd, u_int *buf);
int game_rcv_pos(sceSifMClientData *cd, u_int *buf, int *data);
int communicate();
void com_init();
int CThread(void *func, char *stackp, int size, int prio, void *argp);
int get_pos_handler(int);

#endif // __UDP_GAME_H__
