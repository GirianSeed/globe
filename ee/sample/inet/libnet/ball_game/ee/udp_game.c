/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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
 *                     Name : udp_game.c
 *                     Description : ball game main().
 *
 *
 *       Version        Date           Design      Log
 *  --------------------------------------------------------------------
 *       1.0.0          Sep,29,2000    munekis     first verison
 */


#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <sifrpc.h>
#include <libnet.h>
#include <libmrpc.h>
#include <libgraph.h>

#include "main.h"
#include "udp_game.h"
#include "common.h"

#define ERR_STOP	while(1)

static int tcpid, udpid;
static u_short udp_port;
static struct ctlmsg ctlmsg;


char paddr[128];

static u_int seq;

int com_init_sema = 0;
int sync_sema = 0;
int graph_sema = 0;
int com_sema = 0;
int n_cli = 0;
int n_ball = 0;
int n_all_clients = 0;
int syncv = 0;
Object_t *new_spheres;

extern Object_t spheres0[N_SPHERES];
extern Object_t spheres1[N_SPHERES];
extern u_int frame;

int registeration(sceSifMClientData *cd, u_int *buf)
{
    int ret;
    int flag;

    ctlmsg.type = MSG_REG;
    ctlmsg.udp_port = htons(udp_port);
    ctlmsg.num.rand = htonl(rand());
    flag = 0;
    ret = sceInetSend(cd, buf, tcpid, &ctlmsg, sizeof(struct ctlmsg), &flag, -1);

    return ret;
}

int game_reset(sceSifMClientData *cd, u_int *buf, int *i_data)
{

    *i_data = registeration(cd, buf);

    return 0;
}


int game_rcv_pos(sceSifMClientData *cd, u_int *buf, int *pos) {

    int ret;
    
    ret = sceInetRecv(cd, buf, udpid, pos, MAX_POS_DATA, 0, -1);

    return ret;
}

int game_get_num(sceSifMClientData *cd, u_int *buf) {
    int ret;
    int flag;
    ctlmsg.type = MSG_POSGET;
    ctlmsg.num.seq = htonl(seq++);

    flag = 0;
    if((ret = sceInetSend(cd, buf, tcpid, &ctlmsg, sizeof(struct ctlmsg), &flag, -1)) != sizeof(struct ctlmsg)) {
     printf("TcpWrite(): ret = %d\n", ret);
     return ret;
    }


    return 0;
}

int get_connection (sceSifMClientData *cd, u_int *buf) {

    struct sceInetParam param;
    int flags;
    int ret;

    /* generate random seed */
    srand(GetThreadId());

    /* create UDP socket */
    memset(&param, 0, sizeof(struct sceInetParam));
    param.type = sceINETT_DGRAM;
    udp_port = param.local_port = 8000;
    if((udpid = sceInetCreate(cd, buf, &param)) <= 0){
        printf("sceInetCreate() failed.\n");
        return -1;
    }
    if(sceINETE_OK != (ret = sceInetOpen(cd, buf, udpid, -1))){
        printf("sceInetOpen() failed.\n");
        return -1;
    }

    /* create TCP connection */
    memset(&param, 0, sizeof(struct sceInetParam));
    param.type = sceINETT_CONNECT;
    param.local_port = sceINETP_AUTO;
    if(0 > (ret = sceInetName2Address(cd, buf, 0, &param.remote_addr, paddr, 0, 0)))
        printf("invalid peer addr (%s)\n", paddr);
    param.remote_port = Tcp_PORT_Login;

    if((tcpid = sceInetCreate(cd, buf, &param)) <= 0){
        printf("sceInetCreate() failed.\n");
        return -1;
    }
    if(sceINETE_OK != (ret = sceInetOpen(cd, buf, tcpid, -1))){
        printf("sceInetOpen() failed.\n");
        sceInetClose(cd, buf, tcpid, -1);
        return -1;
    }
	flags = sceINETC_FLAGS_NODELAY;
    if(sceINETE_OK != (ret = sceInetControl(cd, buf, tcpid, sceINETC_CODE_SET_FLAGS, &flags, sizeof(int)))){
        printf("sceInetControl() failed.\n");
        return -1;
    }

    if((ret = registeration(cd, buf)) != sizeof(struct ctlmsg)) {
        printf("TcpWrite(): ret = %d\n", ret);
        return ret;
    }

    return 0;
}

int communicate(){

    sceSifMClientData cd;
    struct pos_data *pos_data;
    struct pos_header *pos_header;
    int pos[MAX_POS_DATA];
    int ret;
    int left;
    Object_t *spheres;
    int n_clients = 0;
    u_int recv_seq;
    u_int max_seq = 0;
    u_int *net_buf;
	const int size = sceLIBNET_PAD( MAX_POS_DATA + sceLIBNET_CTRL_DATA_SIZE );

    libnet_init(&cd, size, 8192, 32);
    if((net_buf = (u_int *)memalign(64, size)) == NULL){
        printf("malloc failed.\n");
        return -1;
    }

    get_connection(&cd, net_buf);
    SignalSema(com_init_sema);

    new_spheres = spheres1;

    while(1){

        if((ret = game_rcv_pos(&cd, net_buf, pos)) < 0){
            if(ret == sceINETE_ABORT){
                printf("position packet is lost.\n");
            } else {
                 printf("game_rcv_pos() failed.\n");
                 ERR_STOP;
            }
        }

        pos_header = (struct pos_header *)&pos[0];
        if(pos_header->type == POS_DATA){
            recv_seq = ntohl(pos_header->seq);
            printf("recv_seq = %d, seq = %d\n", recv_seq, seq);
            if(recv_seq + 10 < seq || seq < max_seq)
                continue;
            else
                max_seq = recv_seq;

            n_cli = ntohs(pos_header->n_cli);
            n_ball = ntohs(pos_header->n_ball);

            /* get pos_data */
            left = sizeof(struct pos_data) * n_cli * n_ball;
            pos_data = (struct pos_data *)((char *)pos + sizeof(struct pos_header));


    
            n_clients = 0;
            new_spheres = spheres
                = (new_spheres == spheres0) ? spheres1 : spheres0;

            while(left) {

                pos_data->n_cli = ntohs(pos_data->n_cli);

                spheres[n_clients].position[0] = pos_data->pos1.x_pos;
                spheres[n_clients].position[1] = pos_data->pos1.y_pos;
                spheres[n_clients].position[2] = pos_data->pos1.z_pos;
                spheres[n_clients].rotation[0] = pos_data->pos2.x_pos;
                spheres[n_clients].rotation[1] = pos_data->pos2.y_pos;
                spheres[n_clients].rotation[2] = pos_data->pos2.z_pos;

                n_clients++;
                left -= sizeof(struct pos_data);

                if(left == 0)
                    break;
                pos_data =
                    (struct pos_data *)((char *)pos_data + sizeof(struct pos_data));
            }
            n_all_clients = n_clients;
    
            FlushCache(0);
        } else
            printf("POS_DATA packet is lost.\n");
    }
}

void com_init() {
    struct SemaParam semap;

    semap.initCount = 0;
    semap.maxCount = 1;
    com_init_sema = CreateSema(&semap);
    semap.initCount = 0;
    semap.maxCount = 1;
    sync_sema = CreateSema(&semap);
    semap.initCount = 0;
    semap.maxCount = 1;
    graph_sema = CreateSema(&semap);
    semap.initCount = 0;
    semap.maxCount = 1;
    com_sema = CreateSema(&semap);
}

int CThread(void *func, char *stackp, int size, int prio, void *argp) {
    struct ThreadParam tp;
    int tid;

    tp.entry = func;
    tp.stack = stackp;
    tp.stackSize = size;
    tp.initPriority = prio;
    tp.gpReg = &_gp;

    if ((tid = CreateThread(&tp)) >= 0)
        StartThread(tid, argp);
    else {
        printf("CThread(): CreateThread() failed.\n");
        return -1;
    }
    return 0;
}

int get_pos_handler(int ca){
    iSignalSema(sync_sema);
    return 0;
}



