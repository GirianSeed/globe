/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                                                      All Right Reserved
 *
 */        
  
/* defines */

#define MEMALIGN  //use memalign for data alignment
#define DOUBLE  // for DMA+CPU double buffering


#define QW_PER_PARTICLE 2  
#define VU0_MAX_PARTICLE 256/QW_PER_PARTICLE


/* Vu0 micro supports only hair length 4 */   
#define HAIR_LENGTH 4
/* number of hairpoints in hair4.bin*/         
#define NHAIR (4736)   
#define NPARTICLE (HAIR_LENGTH*NHAIR)

/* size of hair ball */
#define RADIUS 5.04f


#define SPR 0x70000000
#define V4_32 0x6C

/* globals */
extern sceVu0FMATRIX root_mat;
extern sceVu0FVECTOR root_rot;
extern sceVu0FVECTOR root_trans;

extern u_int hair_count;

extern sceVu0FVECTOR hair_points[NPARTICLE/HAIR_LENGTH];
extern sceVu0FVECTOR hair_orig[NPARTICLE/HAIR_LENGTH];

extern sceVu0FVECTOR simulation_parameter;// x=hair_tension,y=restlength*restlength,z=tstep*tstep    


#ifdef MEMALIGN
sceVu0FVECTOR *data;
#else
sceVu0FVECTOR data[NPARTICLE*QW_PER_PARTICLE] __attribute__((aligned(64)));
#endif


inline u_int vu0_status()
{
 u_int ret;

 __asm__ volatile(
  "   cfc2 %0,$vi29\n"
  : "=&r" (ret) );
 return ret;

}


inline void set_dma_tag(u_long *dma_tag_addr,sceVu0FVECTOR *data_addr,u_int vu_addr,u_int qwc) 
{
   u_long addr;  
    
    addr = (u_int) data_addr;
    *(dma_tag_addr++) = (addr << 32) | (3L <<28) | (qwc); // ref DMA tag
    *(dma_tag_addr) = ( (u_long) SCE_VIF0_SET_UNPACK(vu_addr,qwc&0x0FF,V4_32,0) <<32) | SCE_VIF0_SET_STCYCL(1,1,0);
}

#define TTE_MOD_STR 0x00000144  

inline void send_data(sceVu0FVECTOR *in_data,u_int qwc)
{
 u_long *spr;
 u_int pcr;
 
  spr = (u_long *)SPR;
  set_dma_tag(spr,in_data,0x0,qwc);

  // set the D_PCR.CPC0 for VIF0 DMA channel
  pcr=DGET_D_PCR();pcr|=D_PCR_CPC0_M;
  DPUT_D_PCR(pcr);

  // clear the D_STAT.CIS0 for "bc0f" instruction
  DPUT_D_STAT(D_STAT_CIS0_M);

  // set address 
  DPUT_D0_TADR(SPR | 0x80000000 );
  
  // start DMA      
  DPUT_D0_CHCR(TTE_MOD_STR);
  __asm__ volatile( " sync.l " );
  __asm__ volatile( " sync.p " );    
}     


inline void get_data(u_int addr,u_int 	qwc) 
{  
  u_int pcr;
  // set the D_PCR.CPC9 for toSPR DMA channel
  pcr=DGET_D_PCR();pcr|=D_PCR_CPC9_M;
  DPUT_D_PCR(pcr);

  // clear the D_STAT.CIS9 for "bc0f" instruction
  DPUT_D_STAT(D_STAT_CIS9_M);

  // set address 
  DPUT_D9_MADR(0x11004000);
  DPUT_D9_SADR(addr);
  
  // set qwc
  DPUT_D9_QWC(qwc);
  
  // start DMA       
  DPUT_D9_CHCR(1<<8);  
  __asm__ volatile( " sync.l " );
  __asm__ volatile( " sync.p " );    
}

  
inline void write_data(void *dest,u_int src,u_int nparticle)
{
  
  u_int pcr;

  // set the D_PCR.CPC8 for fromSPR DMA channel
  pcr=DGET_D_PCR();pcr|=D_PCR_CPC8_M;
  DPUT_D_PCR(pcr);

  // clear the D_STAT.CIS8 for "bc0f" instruction
  DPUT_D_STAT(D_STAT_CIS8_M);

  // set address 
  DPUT_D8_MADR(( u_int )dest);
  DPUT_D8_SADR(src);
  
  // set qwc
  DPUT_D8_QWC(nparticle);
  
  // start DMA       
  DPUT_D8_CHCR(1<<8);
  __asm__ volatile( " sync.l " );
  __asm__ volatile( " sync.p " );  
}  



inline void sync_dma_bc0()
{
#ifdef __MWERKS__
label_0b:
        asm __volatile__("bc0t label_0f" );
        asm __volatile__("nop");
        asm __volatile__(" bc0t label_0f" );
        asm __volatile__("nop");
        asm __volatile__(" bc0t label_0f" );
        asm __volatile__("nop");
        asm __volatile__("bc0f label_0b");
        asm __volatile__("nop");
label_0f:;
#else
        asm __volatile__("0:");
        asm __volatile__("bc0t 0f" );
        asm __volatile__("nop");
        asm __volatile__(" bc0t 0f" );
        asm __volatile__("nop");
        asm __volatile__(" bc0t 0f" );
        asm __volatile__("nop");
        asm __volatile__("bc0f 0b");
        asm __volatile__("nop");
        asm __volatile__("0:" );
#endif
}           


inline void prefetch(void *address)
{
 __asm__ volatile("pref 0,0x50(%0)"::"r"(address));
}


inline float myrsqrtf(float arg1,float arg2)
{ 
  float result;
  __asm__ ("rsqrt.s %0,%1,%2" : "=f" (result) : "f" (arg1),"f" (arg2));
  return result;
}

static float RECIP_RAND=1.0f/(float)(RAND_MAX);

inline float random(float start , float end) {
  return start+(end-start)*(float)rand()*RECIP_RAND;
}

/* handle collisions  */
inline void cpu_calc(sceVu0FVECTOR *data,u_int index)
{
  float lengthsqr;
  sceVu0FVECTOR help;

  //prefetch(&(hair_points[hair_count]));    
  if (index%HAIR_LENGTH==0)  {CpuCopyVector(*data,hair_points[hair_count++]);
  } else {
    CpuSubVector(help,*data,root_trans);
    lengthsqr=CpuDotProduct(help,help);
    if (lengthsqr<RADIUS*RADIUS) { 
        CpuScaleVector(help,help,myrsqrtf(RADIUS,lengthsqr));
        CpuAddVector(*data,help,root_trans);    

    }
  }
}

inline void cpu_write_get_data(u_int cpu_spr,u_int vu0_spr,u_int cpu_np,u_int vu0_np,u_int index)
{	
       int i;
	   int write_sync=0;
	   int get_sync=0;
	   
       sceVu0FVECTOR *pos=(sceVu0FVECTOR *)cpu_spr;


       for (i=0;i<cpu_np;i++) {      
        cpu_calc(pos,i);
        pos++; 

#ifdef __MWERKS__
        __asm__ __volatile__(" bc0f label_1f" ); //sync dma
#else
        __asm__ volatile(" bc0f 0f" ); //sync dma
#endif
          if(!write_sync) {
           write_sync=1;
           // write the data back to main memory
           write_data(&data[index*QW_PER_PARTICLE][0],vu0_spr,VU0_MAX_PARTICLE*QW_PER_PARTICLE);
#ifdef __MWERKS__
           __asm__ __volatile__("b label_1f" );
#else
           __asm__ volatile("b 0f" );
#endif
          }           
          if(!get_sync) {    	      
           if(!(vu0_status()&0x1)){ // Wait for [E]-bit found
                get_sync=1;   
                get_data(vu0_spr,vu0_np*QW_PER_PARTICLE);//now send the data to srcatchpad
           }
          }   
#ifdef __MWERKS__
label_1f:;
#else
        __asm__ volatile(" 0:");              
#endif
	  }

	  // if dma did not finish already sync it now
	  if (!write_sync) 	{
         write_data(&data[index*QW_PER_PARTICLE][0],vu0_spr,VU0_MAX_PARTICLE*QW_PER_PARTICLE);
      }   
	  if (!get_sync) {
         sync_dma_bc0(); //sync dma
	 while((vu0_status() & 0x1)){}; // Wait for [E]-bit found    
         get_data(vu0_spr,vu0_np*QW_PER_PARTICLE); //now send the data to scratchpad
      }
}


inline void cpu_get_data(u_int cpu_spr,u_int vu0_spr,u_int cpu_np,u_int vu0_np)
{	
       int i;
       int get_sync=0;
       sceVu0FVECTOR *pos=(sceVu0FVECTOR *)cpu_spr;

       
       for (i=0;i<cpu_np;i++) {
        cpu_calc(pos,i);
        pos++;
         

#ifdef __MWERKS__
        __asm__ __volatile__(" bc0f label_2f" ); //sync dma
#else
        __asm__ volatile(" bc0f 0f" ); //sync dma
#endif
          if(!get_sync) {    	      
           if(!(vu0_status()&0x1)){ // Wait for [E]-bit found
                get_sync=1;   
                get_data(vu0_spr,vu0_np*QW_PER_PARTICLE);//now send the data to srcatchpad
           }
          }   
#ifdef __MWERKS__
label_2f:;
#else
        __asm__ volatile(" 0:");              
#endif
	   } 
	   
	   if (!get_sync) {
              sync_dma_bc0(); //sync dma
	      while((vu0_status() &0x1)){}; // Wait for [E]-bit found    
              get_data(vu0_spr,vu0_np*QW_PER_PARTICLE); //now send the data to scratchpad
       } 
}


inline void cpu(u_int sprP,u_int np)
{	
       int i;
       sceVu0FVECTOR *pos=(sceVu0FVECTOR *)sprP;
	   	   
       for (i=0;i<np;i++) {
        cpu_calc(pos,i);
        pos++;
      }     
}

