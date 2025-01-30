/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : packbuf.h
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.30      Apr,4,2000     thatake     Beta0
 *            0.40      Jun,16,2000     	   +animation, clut
 *            2.1       Nov,17,2000
 */

#ifndef _PACKETBUF_H_
#define _PACKETBUF_H_

#include <eetypes.h>
#include <eestruct.h>

typedef struct{
    qword* pHead;
    qword* pCurr;
    int size;
} fwPackbuf_t;

extern fwPackbuf_t fwPackbufVal;
extern fwPackbuf_t fwPackbufKickBufVal;

static inline void COPY_QW(u_int v0, u_int v1) {
    __asm__ __volatile__(" \n\
        lq    $6,0x0(%1) \n\
        sq    $6,0x0(%0)\n\
        ": : "r" (v0) , "r" (v1) : "$6", "memory");
} 

static inline void  SET_QW(qword qw, u_int three, u_int two, u_int one, u_int zero) {
    qw[3]=three; qw[2]=two; qw[1]=one; qw[0]=zero;
}

static inline void DMA_CNT(qword curr,u_int size) {
    (curr)[0] = 0x10000000 | (u_int)size; 
    (curr)[1] = 0x0; 
    (curr)[2] = SCE_VIF1_SET_NOP(0); 
    (curr)[3] = SCE_VIF1_SET_NOP(0);
}

static inline void  DMA_REF(qword curr, u_int src_addr ,u_int size) {
    curr[0] = 0x30000000 | size; 
    curr[1] = (u_int)src_addr & 0x7fffffff; 
    curr[2] = SCE_VIF1_SET_NOP(0); 
    curr[3] = SCE_VIF1_SET_NOP(0);
}

static inline void  DMA_CALL(qword curr, u_int index,u_int size) {
    curr[0] = 0x50000000 | size; 
    curr[1] = index; 
    curr[2] = SCE_VIF1_SET_NOP(0); 
    curr[3] = SCE_VIF1_SET_NOP(0);
}

static inline void  DMA_RET(qword curr, u_int size) {
    curr[0] = 0x60000000 | size; 
    curr[1] = 0x0; 
    curr[2] = SCE_VIF1_SET_NOP(0); 
    curr[3] = SCE_VIF1_SET_NOP(0);
}

static inline void  ADD_QWDATA(qword curr, qword qwdata){
    COPY_QW((u_int) curr, (u_int) qwdata); 
}

static inline void  ADD_UNPACK(qword curr,u_int field, u_int dest_addr, u_int size){
    curr[field] = SCE_VIF1_SET_UNPACK(dest_addr, size, 0x0c, 0);
}

static inline void  ADD_UNPACKR(qword curr, u_int field, u_int dest_addr,u_int size){
    curr[field] = SCE_VIF1_SET_UNPACKR(dest_addr, size, 0x0c, 0);
}

static inline void  ADD_MSCNT(qword curr, u_int field){
    curr[field] = SCE_VIF1_SET_MSCNT(0);
}

static inline void  ADD_MSCAL(qword curr, u_int field,u_int  start) {
    curr[field] = SCE_VIF1_SET_MSCAL(start, 0);
}

static inline void ADD_FLUSH(qword curr, u_int field){
    curr[field] = SCE_VIF1_SET_FLUSH(0);
}

static inline void  ADD_BASE(qword curr, u_int field, u_int num){
    curr[field] = SCE_VIF1_SET_BASE(num, 0);
}

static inline void  ADD_OFFSET(qword curr, u_int field,u_int num){
    curr[field] = SCE_VIF1_SET_OFFSET(num, 0);
}

static inline void  ADD_STCYCL(qword curr, u_int field, u_int wl, u_int cl){
    curr[field] = SCE_VIF1_SET_STCYCL(wl, cl, 0);
}

static inline void ADD_STROW(qword curr, u_int field){
    curr[field] = SCE_VIF1_SET_STROW(0);
}


static inline void  ADD_DIRECT(qword curr, u_int field, u_int num){
    curr[field] = SCE_VIF1_SET_DIRECT(num, 0);
}

static inline void ADD_STMOD(qword curr, u_int field, u_int mod) {
    curr[field] = SCE_VIF1_SET_STMOD(mod, 0);
}

static inline void REF_UNPACK(qword curr, u_int src_addr, u_int dest_addr,u_int size){
    DMA_REF(curr, src_addr, size); 
    ADD_UNPACK(curr, 3, dest_addr, size);
}

static inline void  REF_UNPACKR(qword curr, u_int src_addr, u_int dest_addr,u_int size){
    DMA_REF(curr, src_addr, size); 
    ADD_UNPACKR(curr, 3, dest_addr, size);
}

static inline void CALL_NOP(qword curr, u_int index,u_int  size){
    DMA_CALL(curr, index, size);
}

static inline void  RET_NOP(qword curr,u_int  size){
    DMA_RET(curr, size);
}

static inline void  RET_BASE_OFFSET(qword curr,u_int  base,u_int offset){
    DMA_RET(curr, 0); 
    ADD_BASE(curr, 2, base); 
    ADD_OFFSET(curr, 3, offset);
}

static inline void  CNT_MSCAL_FLUSH(qword curr,u_int  size,u_int  start){
    DMA_CNT(curr, size); 
    ADD_MSCAL(curr,2, start); 
    ADD_FLUSH(curr,3 ); 
}

static inline void  CNT_FLUSH(qword curr,u_int  size){
    DMA_CNT(curr, size); 
    ADD_FLUSH(curr,3 ); 
}

static inline void  CNT_MSCNT(qword curr,u_int  size){
    DMA_CNT(curr, size); 
    ADD_MSCNT(curr, 3); 
}

static inline void  CNT_UNPACKR(qword curr,u_int  dest_addr,u_int size){
    DMA_CNT(curr, size); 
    ADD_UNPACKR(curr, 3, dest_addr, size);
}

static inline void  CNT_UNPACK(qword curr,u_int  dest_addr,u_int size){
    DMA_CNT(curr,size); 
    ADD_UNPACK(curr, 3, dest_addr, size);
}

static inline void  CNT_STCYCL(qword curr, u_int size, u_int wl,u_int  cl){
    DMA_CNT(curr, size); 
    ADD_STCYCL(curr, 2, wl, cl ); 
}

static inline void  CNT_STCYCL_ROW(qword curr,u_int  size,u_int  wl,u_int cl){
    DMA_CNT(curr, size); 
    ADD_STCYCL(curr, 2, wl, cl ); 
    ADD_STROW(curr, 3);
}

static inline void REF_STMOD_STROW(qword curr, u_int mod, u_int src_addr) {
    DMA_REF(curr, src_addr, 1);
    ADD_STMOD(curr, 2, mod);
    ADD_STROW(curr, 3);
}

static inline void CNT_STMOD_UNPACKR(qword curr, u_int mod, u_int dst_addr, u_int size) {
    DMA_CNT(curr, size);
    ADD_STMOD(curr, 2, mod);
    ADD_UNPACKR(curr, 3, dst_addr, size);
}

static inline void  REF_STCYCL_UNPACK(qword curr,u_int  src_addr,u_int  dest_addr,u_int size,u_int  wl,u_int  cl) {
    DMA_REF(curr, src_addr, size); 
    ADD_STCYCL(curr, 2, wl, cl); 
    ADD_UNPACK(curr, 3, dest_addr, size);
}

static inline void REF_STCYCL_UNPACKR(qword curr,u_int  src_addr,u_int  dest_addr,u_int  size,u_int  wl,u_int cl){
    DMA_REF(curr, src_addr, size); 
    ADD_STCYCL(curr, 2, wl, cl); 
    ADD_UNPACKR(curr, 3, dest_addr, size);
}

static inline void  CNT_STCYCL_UNPACKR(qword curr,u_int  dest_addr,u_int  size,u_int  wl,u_int cl){
  DMA_CNT(curr, size); 
  ADD_STCYCL(curr, 2, wl, cl); 
  ADD_UNPACKR(curr, 3, dest_addr, size);
}

static inline void  CNT_DIRECT(qword curr,u_int  size){
    DMA_CNT(curr, size); 
    ADD_DIRECT(curr, 3, (size));
}

static inline void  REF_DIRECT(qword curr,u_int  src_addr,u_int size){
    DMA_REF(curr, src_addr, size); 
    ADD_DIRECT(curr, 3, size);
}

extern void fwPackbufInit(fwPackbuf_t *, int size, u_int mask);
extern void fwPackbufFinish(fwPackbuf_t *);
extern void fwPackbufReset(fwPackbuf_t *);
extern int fwPackbufCheckOverflow(fwPackbuf_t *);

extern void fwPackbufAddCall(qword *, qword *);

extern void fwPackbufKickDMA(fwPackbuf_t *);
#endif /* !_PACKETBUF_H_ */
