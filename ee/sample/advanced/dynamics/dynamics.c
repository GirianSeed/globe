/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
*/
/*
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                                                      All Right Reserved
 *
 */
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <eeregs.h>
#include <libvu0.h>
#include <libdma.h>
#include <libhig.h>
#include <libhip.h>
#include <malloc.h>
#include "util.h"
#include "camera.h"
#include <math.h>
#include "vector.h"
#include "dynamics.h"


#define COLL_DIST 0.02f
#define SEPARATE_DIST 0.002f 
#define WARNING_DIST COLL_DIST*0.25f

#define POS 0
#define ROT 1
#define IMP 2
#define ANG 3

#define FRICTION 1
#define NOFRICTION 0 
#define MICROCOLL  1
#define NOMICROCOLL 0



#define DEBUG 0
void printvector(char *s, sceVu0FVECTOR vect)
{
#if DEBUG
  scePrintf("%s %f,%f,%f,%f\n",s,vect[0],vect[1],vect[2],vect[3]);
#endif
}



#define STATE_DIM 4



void body_to_state(rigid_body **bodylist,int nbody,sceVu0FVECTOR *state_vect)
{
  int i=0;
  rigid_body *body;
  
  for(i=0;i<nbody;i++) {
    body=*(bodylist++);
    sceVu0CopyVector(*(state_vect++),body->x);
    sceVu0CopyVector(*(state_vect++),body->rq);
    sceVu0CopyVector(*(state_vect++),body->p);  
    sceVu0CopyVector(*(state_vect++),body->L);
  }  
}


void state_to_body(rigid_body **bodylist,int nbody,sceVu0FVECTOR *state_vect)
{
  int i;
  rigid_body *body;

  for(i=0;i<nbody;i++) {
    body=*(bodylist++);
    if (body->type!=PASSIVE) {
      sceVu0CopyVector(body->x,*(state_vect++));
      sceVu0CopyVector(body->rq,*(state_vect++));
      sceVu0CopyVector(body->p,*(state_vect++));
      sceVu0CopyVector(body->L,*(state_vect++));
    } else state_vect=state_vect+4;
  }
}





void normalize(sceVu0FVECTOR q) 
{
  float norm,help;
  norm=sceVu0InnerProduct(q,q)+q[3]*q[3];
  help=my_rsqrtf(1.0f,norm);
  sceVu0ScaleVector(q,q,help);
}

void q_to_matrix(sceVu0FMATRIX matrix,sceVu0FVECTOR q)
{
  float d_qxqx=2*q[0]*q[0];
  float d_qyqy=2*q[1]*q[1];
  float d_qzqz=2*q[2]*q[2];

  float d_qxqy=2*q[0]*q[1];
  float d_qxqz=2*q[0]*q[2];

  float d_qyqz=2*q[1]*q[2];

  float d_qwqx=2*q[3]*q[0];
  float d_qwqy=2*q[3]*q[1];
  float d_qwqz=2*q[3]*q[2];

  matrix[0][0]=1-d_qyqy-d_qzqz;
  matrix[0][1]=d_qxqy+d_qwqz;
  matrix[0][2]=d_qxqz-d_qwqy;
  matrix[0][3]=0;

  matrix[1][0]=d_qxqy-d_qwqz;
  matrix[1][1]=1-d_qxqx-d_qzqz;
  matrix[1][2]=d_qyqz+d_qwqx;
  matrix[1][3]=0;

  matrix[2][0]=d_qxqz+d_qwqy;
  matrix[2][1]=d_qyqz-d_qwqx;
  matrix[2][2]=1-d_qxqx-d_qyqy;
  matrix[2][3]=0;

  matrix[3][0]=0;
  matrix[3][1]=0;
  matrix[3][2]=0;
  matrix[3][3]=1.0;
}

void vect_q_mult(sceVu0FVECTOR state_vect, sceVu0FVECTOR vect,sceVu0FVECTOR q)
{
  sceVu0FVECTOR qtemp;
  
  sceVu0OuterProduct(state_vect,vect,q);
  sceVu0ScaleVector(qtemp,vect,q[3]);
  sceVu0AddVector(state_vect,state_vect,qtemp);
  state_vect[3]=-sceVu0InnerProduct(vect,q);
}

void rigid_body_deriv(float t, sceVu0FVECTOR *Xn,sceVu0FVECTOR *Xndot,int i,rigid_body *body)
{ 

  sceVu0FMATRIX temp,R,R_t,Iinv_t;
  sceVu0FVECTOR omega;
  sceVu0FVECTOR *p;
      
  /* xdot=v=p/m; */  
  sceVu0ScaleVectorXYZ(*(Xndot+i*STATE_DIM+POS),*(Xn+i*STATE_DIM+IMP),body->invM);
  
  /*
    R=get_rot_mat(q);
    Iinv(t)=R*Iinvbody*R_t;
    omega(t)=Iinv(t)*L;
    qdot=0.5*omega(t)*q;
  */
  p=Xn+i*STATE_DIM+ROT;
  
  normalize(*p);
  q_to_matrix(R,*(Xn+i*STATE_DIM+ROT));
  sceVu0MulMatrix(temp,R,body->Iinv);
  sceVu0TransposeMatrix(R_t,R);
  sceVu0MulMatrix(Iinv_t,temp,R_t);
  sceVu0ApplyMatrix(omega,Iinv_t,*(Xn+i*STATE_DIM+ANG));
  vect_q_mult(*(Xndot+i*STATE_DIM+ROT),omega,*(Xn+i*STATE_DIM+ROT));
  sceVu0ScaleVector(*(Xndot+i*STATE_DIM+ROT),*(Xndot+i*STATE_DIM+ROT),0.5f);
  /*
    pdot=force(t);
    Ldot=tourque(t);
  */
  sceVu0CopyVector(*(Xndot+i*STATE_DIM+IMP),body->F);
  sceVu0CopyVector(*(Xndot+i*STATE_DIM+ANG),body->torque);  
}




void runge_kutta(sceVu0FVECTOR *Xt0,sceVu0FVECTOR *Xt1,float t,float step,void (*f) (float , sceVu0FVECTOR *,sceVu0FVECTOR *,int,rigid_body *),rigid_body **bodylist,int nbody,SimMEM *mem)
{

  sceVu0FVECTOR *d1,*d2,*d3,*d4,*d5;
  sceVu0FVECTOR *Xtemp,temp;
  rigid_body *body;
  int i,j;
  

  d1=mem->d1;
  d2=mem->d2;
  d3=mem->d3;
  d4=mem->d4;
  d5=mem->d5;
  Xtemp= mem->Xtemp;
  

  for(i=0;i<nbody;i++) {
    body=*(bodylist+i);
    if(body->type!=PASSIVE){
      /* d1 */
      (*f)(t,Xt0,d1,i,body);
      
      /* d2 */
      for(j=0;j<STATE_DIM;j++){
	sceVu0ScaleVector(temp,(d1+i*STATE_DIM)[j],0.5f*step);
	sceVu0AddVector((Xtemp+i*STATE_DIM)[j],temp,(Xt0+i*STATE_DIM)[j]);
      }
      (*f)(t+0.5f*step,Xtemp,d2,i,body);
      
      /* d3 */
      for(j=0;j<STATE_DIM;j++){
	sceVu0ScaleVector(temp,(d2+i*STATE_DIM)[j],0.5f*step);
	sceVu0AddVector((Xtemp+i*STATE_DIM)[j],temp,(Xt0+i*STATE_DIM)[j]);
      }
      (*f)(t+0.5f*step,Xtemp,d3,i,body);
      
      /* d4 */
      
      for(j=0;j<STATE_DIM;j++){
	sceVu0ScaleVector(temp,(d3+i*STATE_DIM)[j],step);
	sceVu0AddVector((Xtemp+i*STATE_DIM)[j],temp,(Xt0+i*STATE_DIM)[j]);
      }
      (*f)(t+step,Xtemp,d4,i,body);
    

      for(j=0;j<STATE_DIM;j++){
	sceVu0ScaleVector((d2+i*STATE_DIM)[j],(d2+i*STATE_DIM)[j],1.0f/3.0f);
	sceVu0ScaleVector((d3+i*STATE_DIM)[j],(d3+i*STATE_DIM)[j],1.0f/3.0f);
	sceVu0ScaleVector((d4+i*STATE_DIM)[j],(d4+i*STATE_DIM)[j],1.0f/6.0f);
	
	sceVu0ScaleVector((d5+i*STATE_DIM)[j],(d1+i*STATE_DIM)[j],1.0f/6.0f);
	sceVu0AddVector((d5+i*STATE_DIM)[j],(d5+i*STATE_DIM)[j],(d2+i*STATE_DIM)[j]);
	sceVu0AddVector((d5+i*STATE_DIM)[j],(d5+i*STATE_DIM)[j],(d3+i*STATE_DIM)[j]);
	sceVu0AddVector((d5+i*STATE_DIM)[j],(d5+i*STATE_DIM)[j],(d4+i*STATE_DIM)[j]);
     
	/* increment Xn and t*/  
	sceVu0ScaleVector((d2+i*STATE_DIM)[j],(d5+i*STATE_DIM)[j],step);
	sceVu0AddVector((Xt1+i*STATE_DIM)[j],(Xt0+i*STATE_DIM)[j],(d2+i*STATE_DIM)[j]);
      }
    }
  }
}



void update_lw(sceVu0FVECTOR *X,rigid_body **bodylist,int nbody);
float get_coll_time(float t,float tstep,Contact *c,sceVu0FVECTOR *Xstart,sceVu0FVECTOR *Xend,sceVu0FVECTOR *d1,sceVu0FVECTOR *d5,int aID, int bID,float min,float max);
void roll_back(float t,float tstep,sceVu0FVECTOR *Xstart,sceVu0FVECTOR *Xend,sceVu0FVECTOR *d1,sceVu0FVECTOR *d5,int aID, int bID,rigid_body **bodylist,int nbody);
void analyze_contact(Contact *c,sceVu0FVECTOR *X,int aID,int bID,u_int friction,u_int microc);
void tolerance_corr(Contact *c,sceVu0FVECTOR *X,int aID,int bID);
void add_friction_impulse(Contact *c,sceVu0FVECTOR v,sceVu0FVECTOR *X,int aID,int bID);
void add_nofriction_impulse(Contact *c,sceVu0FVECTOR v,sceVu0FVECTOR *X,int aID,int bID,u_int microc);

#define CALC_POINTS 1

void simulation(rigid_body **bodylist,int nbody,float tstep,int nsteps,float tstart,SimMEM *mem,void (*calc_force)(sceVu0FVECTOR *,rigid_body **,int ,float ))
{
  sceVu0FVECTOR *Xstart;
  sceVu0FVECTOR *Xend;
  sceVu0FVECTOR *help;
  float t;
  int k,i,j,count;
  Contact *cl=mem->cache_contact_list;
 
  Xstart=mem->Xstart;
  Xend=mem->Xend;
 
  body_to_state(bodylist,nbody,Xstart);
 
  t=tstart;
  for(k=0;k<nsteps;k++) {
    (*calc_force)(Xstart,bodylist,nbody,t);
 
    // integrate equations of motion
    runge_kutta(Xstart,Xend,t,tstep,rigid_body_deriv,bodylist,nbody,mem);
 
    // update local_world matrices
    update_lw(Xend,bodylist,nbody);

    count=-1;
    for(i=0;i<nbody-1;i++) {
      for(j=i+1;j<nbody;j++) {
        count++;
        cl[count].bodyA=*(bodylist+i);
        cl[count].bodyB=*(bodylist+j);       
        if (cl[count].bodyA->type!=PASSIVE || cl[count].bodyB->type!=PASSIVE) {
          if ((cl[count].dist=calc_dist(&(cl[count].simplex),cl[count].bodyA->gjkobj,cl[count].bodyB->gjkobj,1))<COLL_DIST) {
            // seperate objects if they intersect
            if(cl[count].dist<SEPARATE_DIST) {
              get_coll_time(tstep,tstep,&cl[count],Xstart,Xend,mem->d1,mem->d5,i,j,0.0f,SEPARATE_DIST);
            }
            // add impulse and angular-momentum
            analyze_contact(&cl[count],Xend,i,j,NOFRICTION,NOMICROCOLL);
            // if objects are to close seperate them
            if(cl[count].dist<WARNING_DIST) tolerance_corr(&cl[count],Xend,i,j);
          }
        }
      }
    }
 
    help=Xstart;
    Xstart=Xend;
    Xend=help;
  }
  state_to_body(bodylist,nbody,Xstart);
}                  


void simulation_with_rollback(rigid_body **bodylist,int nbody,float tstep,int nsteps,float tstart,SimMEM *mem,void (*calc_force)(sceVu0FVECTOR *,rigid_body **,int ,float ))
{
  sceVu0FVECTOR *Xstart;
  sceVu0FVECTOR *Xend;
  sceVu0FVECTOR *help;
  Contact *cl=mem->cache_contact_list;
  float t;
  int k,i,j;
  float coll_time;
  int count;
  int notresolved=1;
  int loop_count=0;
  Xstart=mem->Xstart; 
  Xend=mem->Xend; 
  
  body_to_state(bodylist,nbody,Xstart);
  
  t=tstart;
  for(k=0;k<nsteps;k++) {
    (*calc_force)(Xstart,bodylist,nbody,t);

    // integrate equations of motion
    runge_kutta(Xstart,Xend,t,tstep,rigid_body_deriv,bodylist,nbody,mem);


    notresolved=1;
    coll_time=tstep;
    while (notresolved) {

      //debug
      loop_count++;if (loop_count>1000) {scePrintf("resolved %d collisions in one simulation step\n",loop_count);}

      update_lw(Xend,bodylist,nbody);  
      notresolved=0;
      count=-1;
      for(i=0;i<nbody-1;i++) {
	for(j=i+1;j<nbody;j++) {
	  count++;
	  cl[count].bodyA=*(bodylist+i);
	  cl[count].bodyB=*(bodylist+j);
	  if (cl[count].bodyA->type!=PASSIVE || cl[count].bodyB->type!=PASSIVE) {
	    cl[count].dist=calc_dist(&(cl[count].simplex),cl[count].bodyA->gjkobj,cl[count].bodyB->gjkobj,1);
	    if (cl[count].dist<COLL_DIST) {
	      // seperate objects if they intersect
	      if(cl[count].dist<SEPARATE_DIST) {
		coll_time=get_coll_time(coll_time,tstep,&(cl[count]),Xstart,Xend,mem->d1,mem->d5,i,j,SEPARATE_DIST,COLL_DIST);
		roll_back(coll_time,tstep,Xstart,Xend,mem->d1,mem->d5,i,j,bodylist,nbody);
		notresolved=1; break;
	      }	  	            
	    }
	  }	
	}
	if (notresolved) break; 
      }
    }
    
    // add impulse
    count=-1;
    for(i=0;i<nbody-1;i++) {
      for(j=i+1;j<nbody;j++) { 
       count++; 
       if (cl[count].bodyA->type!=PASSIVE || cl[count].bodyB->type!=PASSIVE) {
         // use ALWAYS "MICROCOLL" when using this function !!!!	  
	 if (cl[count].dist<COLL_DIST) analyze_contact(&(cl[count]),Xend,i,j,FRICTION,MICROCOLL); 
       }
      }
    }

    help=Xstart;
    Xstart=Xend;
    Xend=help;   
  }
  
  state_to_body(bodylist,nbody,Xstart);
}
  

float get_coll_time(float ts,float tstep,Contact *c,sceVu0FVECTOR *Xstart,sceVu0FVECTOR *Xend,sceVu0FVECTOR *d1,sceVu0FVECTOR *d5,int aID, int bID,float min,float max)
{
  
  float k1fact,t,dist,delta;
  sceVu0FVECTOR temp,temp2;
  int i;
  float distpre;
  int count=0;

  if(c->bodyA->type!=PASSIVE) {
    q_to_matrix(*(c->bodyA->gjkobj->lw),*(Xstart+aID*STATE_DIM+ROT));
    sceVu0CopyVector((*(c->bodyA->gjkobj->lw))[3],*(Xstart+aID*STATE_DIM+POS));
    (*(c->bodyA->gjkobj->lw))[3][3]=1.0;
    q_to_matrix(*(c->bodyA->gjkobj->lwrot),*(Xstart+aID*STATE_DIM+ROT));
  }

  if(c->bodyB->type!=PASSIVE) {
    q_to_matrix(*(c->bodyB->gjkobj->lw),*(Xstart+bID*STATE_DIM+ROT));
    sceVu0CopyVector((*(c->bodyB->gjkobj->lw))[3],*(Xstart+bID*STATE_DIM+POS));
    (*(c->bodyB->gjkobj->lw))[3][3]=1.0;
    q_to_matrix(*(c->bodyB->gjkobj->lwrot),*(Xstart+bID*STATE_DIM+ROT));
  }
  
  distpre=calc_dist(&(c->simplex),c->bodyA->gjkobj,c->bodyB->gjkobj,CALC_POINTS); 	  


  if (distpre<=min) {
    // debug
    scePrintf("Can not calculate interpolation time:objects are alreay intersecting the prev. frame!!\n");
    for(i=0;i<STATE_DIM-2;i++){
      sceVu0CopyVector(*(Xend+aID*STATE_DIM+i),*(Xstart+aID*STATE_DIM+i));
      sceVu0CopyVector(*(Xend+bID*STATE_DIM+i),*(Xstart+bID*STATE_DIM+i));
    }
    return 0.0f; 
  }

  t=delta=ts*0.5f;
  do {
    k1fact=(1-t/tstep);
    /* x rq */
      if(c->bodyA->type!=PASSIVE) {
       for(i=0;i<2;i++){ 
	sceVu0ScaleVector(temp,*(d1+aID*STATE_DIM+i),k1fact*t);
	sceVu0ScaleVector(temp2,*(d5+aID*STATE_DIM+i),t*t/tstep);
	sceVu0AddVector(temp,temp,temp2);
	sceVu0AddVector(*(Xend+aID*STATE_DIM+i),*(Xstart+aID*STATE_DIM+i),temp);
       }
	normalize(*(Xend+aID*STATE_DIM+ROT));
	q_to_matrix(*(c->bodyA->gjkobj->lw),*(Xend+aID*STATE_DIM+ROT));
	sceVu0CopyVector((*(c->bodyA->gjkobj->lw))[3],*(Xend+aID*STATE_DIM+POS));
	(*(c->bodyA->gjkobj->lw))[3][3]=1.0;
	q_to_matrix(*(c->bodyA->gjkobj->lwrot),*(Xend+aID*STATE_DIM+ROT));
      }

      if(c->bodyB->type!=PASSIVE) {
       for(i=0;i<2;i++){  
	sceVu0ScaleVector(temp,*(d1+bID*STATE_DIM+i),k1fact*t);
	sceVu0ScaleVector(temp2,*(d5+bID*STATE_DIM+i),t*t/tstep);
	sceVu0AddVector(temp,temp,temp2);
	sceVu0AddVector(*(Xend+bID*STATE_DIM+i),*(Xstart+bID*STATE_DIM+i),temp);
       } 
	normalize(*(Xend+bID*STATE_DIM+ROT));
	q_to_matrix(*(c->bodyB->gjkobj->lw),*(Xend+bID*STATE_DIM+ROT));
	sceVu0CopyVector((*(c->bodyB->gjkobj->lw))[3],*(Xend+bID*STATE_DIM+POS));
	(*(c->bodyB->gjkobj->lw))[3][3]=1.0;
	q_to_matrix(*(c->bodyB->gjkobj->lwrot),*(Xend+bID*STATE_DIM+ROT));
      }
        
    dist=calc_dist(&(c->simplex),c->bodyA->gjkobj,c->bodyB->gjkobj,CALC_POINTS); 	  
    delta*=0.5f;

    if(dist<min  ){
      t-=delta;
    }
    
    if(dist>max) {
      t+=delta;
    } 

    count++;
    if(count>10000) {
      // debug
      scePrintf("Can not find collision time after %d interpolation steps. Interpolation process stoped.\n",count); 
      break;
    }
   } while(dist<min || dist >max);
  
  c->dist=dist;

  return t;
}

void roll_back(float t,float tstep,sceVu0FVECTOR *Xstart,sceVu0FVECTOR *Xend,sceVu0FVECTOR *d1,sceVu0FVECTOR *d5,int aID, int bID,rigid_body **bodylist,int nbody)
{
  int j,i;
  sceVu0FVECTOR temp,temp2;

  for (j=0;j<nbody;j++)
    if ((*(bodylist+j))->type==ACTIVE && (j!=aID ||j!=bID))
      for(i=0;i<STATE_DIM;i++){
	sceVu0ScaleVector(temp,*(d1+j*STATE_DIM+i),(1-t/tstep)*t);
	sceVu0ScaleVector(temp2,*(d5+j*STATE_DIM+i),t*t/tstep);
	sceVu0AddVector(temp,temp,temp2);
	sceVu0AddVector(*(Xend+j*STATE_DIM+i),*(Xstart+j*STATE_DIM+i),temp);
      }
}


  
void update_lw(sceVu0FVECTOR *X,rigid_body **bodylist,int nbody)
{
  int i;
  rigid_body *body;
 
  for(i=0;i<nbody;i++) {
    body=*(bodylist+i);
    if (body->type!=PASSIVE) {
      normalize(*(X+i*STATE_DIM+ROT));
      q_to_matrix(*(body->gjkobj->lw),*(X+i*STATE_DIM+ROT));
      sceVu0CopyVector((*(body->gjkobj->lw))[3],*(X+i*STATE_DIM+POS));
      (*(body->gjkobj->lw))[3][3]=1.0;
      q_to_matrix(*(body->gjkobj->lwrot),*(X+i*STATE_DIM+ROT));
    }
  }
}


void analyze_contact(Contact *c,sceVu0FVECTOR *X,int aID,int bID,u_int friction,u_int microc) 
{
  
  sceVu0FVECTOR temp,temp2,v,va,vb;
  sceVu0FMATRIX R_t,tempM;
  

  /* get contact points in world coordinates */
  c->simplex.closest_pointA[3]=1.0f;
  c->simplex.closest_pointB[3]=1.0f;
  sceVu0ApplyMatrix(c->simplex.closest_pointA,*(c->bodyA->gjkobj->lw),c->simplex.closest_pointA);
  sceVu0ApplyMatrix(c->simplex.closest_pointB,*(c->bodyB->gjkobj->lw),c->simplex.closest_pointB);

  /* contact normal */
  sceVu0SubVector(c->normal,c->simplex.closest_pointA,c->simplex.closest_pointB);
  sceVu0Normalize(c->normal,c->simplex.cache_vect);

  /* ra = contact-point - center of mass */
  sceVu0SubVector(c->ra,c->simplex.closest_pointA,*(X+STATE_DIM*aID+POS));
  sceVu0SubVector(c->rb,c->simplex.closest_pointB,*(X+STATE_DIM*bID+POS));

  
  /* calc point velocities */
  if (c->bodyA->type!=PASSIVE) {
    sceVu0MulMatrix(tempM,*(c->bodyA->gjkobj->lwrot),c->bodyA->Iinv);
    sceVu0TransposeMatrix(R_t,*(c->bodyA->gjkobj->lwrot));
    sceVu0MulMatrix(c->bodyA->Iinv_t,tempM,R_t);
    sceVu0ApplyMatrix(c->bodyA->omega,c->bodyA->Iinv_t,*(X+STATE_DIM*aID+ANG));
    sceVu0OuterProduct(temp,c->bodyA->omega,c->ra);
    sceVu0ScaleVector(temp2,*(X+STATE_DIM*aID+IMP),c->bodyA->invM);
    sceVu0AddVector(va,temp2,temp);
  } else {
    sceVu0ScaleVector(va,va,0.0f);
  }

  if (c->bodyB->type!=PASSIVE) {    
    sceVu0MulMatrix(tempM,*(c->bodyB->gjkobj->lwrot),c->bodyB->Iinv);
    sceVu0TransposeMatrix(R_t,*(c->bodyB->gjkobj->lwrot));
    sceVu0MulMatrix(c->bodyB->Iinv_t,tempM,R_t);
    sceVu0ApplyMatrix(c->bodyB->omega,c->bodyB->Iinv_t,*(X+STATE_DIM*bID+ANG));
    sceVu0OuterProduct(temp,c->bodyB->omega,c->rb);
    sceVu0ScaleVector(temp2,*(X+STATE_DIM*bID+IMP),c->bodyB->invM);
    sceVu0AddVector(vb,temp2,temp);
  } else {
    sceVu0ScaleVector(vb,vb,0.0f);
  }


  sceVu0SubVector(v,va,vb);
  c->vrel=sceVu0InnerProduct(c->normal,v);
  
  if (c->vrel>0) return;
  if (friction) {
     //use always "micro-collisions" when friction is turned on
     add_friction_impulse(c,v,X,aID,bID);
  } else {
     add_nofriction_impulse(c,v,X,aID,bID,microc);
  }

}

#define DIM 3 
void  LU_fact(sceVu0FMATRIX amatrix,sceVu0FVECTOR index)
{
  int i,j,k,imax=0;
  float max,sum,help;
  sceVu0FVECTOR scale;

  for(i=0;i<DIM;i++){
    max=0.0f;
    for (j=0;j<DIM;j++){
      if ((help=fabs(amatrix[i][j]))>max)
        max=help;
    }
    if (max==0.0f) {scePrintf("UPS matrix is singular!\n");
    while(1);}
    scale[i]=1.0f/max;
  }
  for(j=0;j<DIM;j++) {
    for(i=0;i<j;i++){
      sum=amatrix[i][j];
      for(k=0;k<i;k++) sum-=amatrix[i][k]*amatrix[k][j];
      amatrix[i][j]=sum;
    }
    max=0.0f;
    for(i=j;i<DIM;i++){
      sum=amatrix[i][j];
      for(k=0;k<j;k++) sum-=amatrix[i][k]*amatrix[k][j];
      amatrix[i][j]=sum;
      if ((help=scale[i]*fabs(sum))>=max) {
        max=help;
        imax=i;
      }
    }
    if(j!=imax) {
      for (k=0;k<DIM;k++){
        help=amatrix[imax][k];
        amatrix[imax][k]=amatrix[j][k];
        amatrix[j][k]=help;
      }
      scale[imax]=scale[j];
    }
    index[j]=imax;
#define SMALL 0.00002f
    if (amatrix[j][j] == 0.0f) amatrix[j][j]=SMALL;
    if (j!=DIM-1) {
      help=1.0f/amatrix[j][j];
      for(i=j+1;i<DIM;i++) amatrix[i][j]*=help;
    }
  }
}                                      


void solveLU(sceVu0FMATRIX amatrix,sceVu0FVECTOR result,sceVu0FVECTOR bvec,sceVu0FVECTOR index)
{
  int i,j,ip,ii=0,valid=0;
  float sum;
 
  for(i=0;i<DIM;i++) {
    ip=index[i];
    sum=bvec[ip];
    bvec[ip]=bvec[i];
    if (valid)
      for(j=ii;j<=i-1;j++) sum -=amatrix[i][j]*(result[j]);
    else if (sum) { ii=i;valid=1;}
    result[i]=sum;
  }
  for(i=DIM-1;i>=0;i--) {
    sum=result[i];
    for(j=i+1;j<DIM;j++) sum -=amatrix[i][j]*result[j];
    result[i]=sum/amatrix[i][i];
  }
}        



#define ABS(x)  (((x<0.0f)?(-x):(x)))  
void add_friction_impulse(Contact *c,sceVu0FVECTOR v,sceVu0FVECTOR *X,int aID,int bID) 
{
  sceVu0FVECTOR V,r,t,A,B,C;
  sceVu0FVECTOR Aa,Ba,Ca,Ab,Bb,Cb;
  sceVu0FVECTOR result,result2,bvec,index;
  sceVu0FVECTOR temp,temp2;
  sceVu0FVECTOR impulse,angimpulse,mimpulse;
  sceVu0FMATRIX m;
  float invM,n,slip,friction_limit;
  float mu=0.7f;
  float epsilon=0.7f;
  float alpha,beta,gamma;
  float help;

  if(ABS(c->normal[0])<=ABS(c->normal[1])){
    if(ABS(c->normal[2])<=ABS(c->normal[0])) { V[0] = 0.0f; V[1] = 0.0f; V[2] = 1.0f; }
    else { V[0] = 1.0f; V[1] = 0.0f; V[2] = 0.0f; }
  }
  else{
    if(ABS(c->normal[2])<=ABS(c->normal[1])) { V[0] = 0.0f; V[1] = 0.0f; V[2] = 1.0f; }
    else { V[0] = 0.0f; V[1] = 1.0f; V[2] = 0.0f; }
  }
 
  sceVu0OuterProduct(t, c->normal,V);
  sceVu0Normalize(t, t);
  sceVu0OuterProduct(r, c->normal, t);                   

  if (c->vrel>0) return;

  //micro_collision
  #define EPSILON_MAX 15.0
  if(c->dist<COLL_DIST && c->vrel > -0.4f) {
    help=1.0f-c->dist/COLL_DIST;
    epsilon=(EPSILON_MAX-epsilon)*help+epsilon;
  }  


  sceVu0ScaleVector(Aa,Aa,0.0f);
  sceVu0ScaleVector(Ba,Ba,0.0f);
  sceVu0ScaleVector(Ca,Ca,0.0f);
 
  if (c->bodyA->type!=PASSIVE) {    
    sceVu0OuterProduct(temp,c->ra,t);
    sceVu0ApplyMatrix(temp,c->bodyA->Iinv_t,temp);
    sceVu0OuterProduct(Aa,temp,c->ra);

    sceVu0OuterProduct(temp,c->ra,r);
    sceVu0ApplyMatrix(temp,c->bodyA->Iinv_t,temp);
    sceVu0OuterProduct(Ba,temp,c->ra);

    sceVu0OuterProduct(temp,c->ra,c->normal);
    sceVu0ApplyMatrix(temp,c->bodyA->Iinv_t,temp);
    sceVu0OuterProduct(Ca,temp,c->ra);
  }

  sceVu0ScaleVector(Ab,Ab,0.0f);
  sceVu0ScaleVector(Bb,Bb,0.0f);
  sceVu0ScaleVector(Cb,Cb,0.0f);
  
  if (c->bodyB->type!=PASSIVE) {
    sceVu0OuterProduct(temp2,c->rb,t);
    sceVu0ApplyMatrix(temp2,c->bodyB->Iinv_t,temp2);
    sceVu0OuterProduct(Ab,temp2,c->rb);

    sceVu0OuterProduct(temp2,c->rb,r);
    sceVu0ApplyMatrix(temp2,c->bodyB->Iinv_t,temp2);
    sceVu0OuterProduct(Bb,temp2,c->rb);

    sceVu0OuterProduct(temp2,c->rb,c->normal);
    sceVu0ApplyMatrix(temp2,c->bodyB->Iinv_t,temp2);
    sceVu0OuterProduct(Cb,temp2,c->rb);
  }

  sceVu0AddVector(A,Aa,Ab);
  sceVu0AddVector(B,Ba,Bb);
  sceVu0AddVector(C,Ca,Cb);


  invM=0.0f;
  if (c->bodyA->type!=PASSIVE) {
    invM+=c->bodyA->invM;
  }
  if (c->bodyB->type!=PASSIVE) {
    invM+=c->bodyB->invM;
  }
  

  
  m[0][0]=sceVu0InnerProduct(A,t)+invM;
  m[1][1]=sceVu0InnerProduct(B,r)+invM;
  alpha=m[2][2]=sceVu0InnerProduct(C,c->normal)+invM;


  m[0][1]=sceVu0InnerProduct(B,t);
  m[0][2]=sceVu0InnerProduct(C,t);

  m[1][0]=sceVu0InnerProduct(A,r);
  m[1][2]=sceVu0InnerProduct(C,r);

  m[2][0]=sceVu0InnerProduct(A,c->normal);
  beta=m[2][1]=sceVu0InnerProduct(B,c->normal);

  bvec[0]=-sceVu0InnerProduct(v,t);
  bvec[1]=-sceVu0InnerProduct(v,r);
  gamma=bvec[2]=-(1.0f+epsilon)*c->vrel;

  LU_fact(m,index);
  solveLU(m,result,bvec,index);

  sceVu0ScaleVector(impulse,t,result[0]);
  sceVu0ScaleVector(temp,r,result[1]);
  sceVu0ScaleVector(temp2,c->normal,result[2]);
  sceVu0AddVector(impulse,temp,impulse);
  sceVu0AddVector(impulse,temp2,impulse);

  // micro collision
  if(c->dist<COLL_DIST && c->vrel > -0.4f)
  {
    bvec[0]=-2.001f*sceVu0InnerProduct(v,t);
    bvec[1]=-2.001f*sceVu0InnerProduct(v,r);
    bvec[2]=-2.001f*sceVu0InnerProduct(v,c->normal);

    solveLU(m,result2,bvec,index);

    sceVu0ScaleVector(mimpulse,t,result2[0]);
    sceVu0ScaleVector(temp,r,result2[1]);
    sceVu0ScaleVector(temp2,c->normal,result2[2]);
    sceVu0AddVector(mimpulse,temp,mimpulse);
    sceVu0AddVector(mimpulse,temp2,mimpulse);
    
    sceVu0OuterProduct(t,mimpulse,c->normal);
    sceVu0OuterProduct(r,c->normal,t);
    

    slip=my_sqrtf(r[0]*r[0]+r[1]*r[1]*r[2]*r[3]);
    friction_limit=ABS(sceVu0InnerProduct(mimpulse,c->normal));
    friction_limit*=mu;

    if (slip<friction_limit) {
      if (c->bodyA->type!=PASSIVE) {    
	sceVu0AddVector(*(X+STATE_DIM*aID+IMP),*(X+STATE_DIM*aID+IMP),mimpulse);
	sceVu0OuterProduct(angimpulse,c->ra,mimpulse);
	sceVu0AddVector(*(X+STATE_DIM*aID+ANG),*(X+STATE_DIM*aID+ANG),angimpulse);
      }
    
      if (c->bodyB->type!=PASSIVE) {    
	sceVu0SubVector(*(X+STATE_DIM*bID+IMP),*(X+STATE_DIM*bID+IMP),mimpulse);
	sceVu0OuterProduct(angimpulse,c->rb,mimpulse);
	sceVu0SubVector(*(X+STATE_DIM*bID+ANG),*(X+STATE_DIM*bID+ANG),angimpulse);
      }
      return;
    }
  }


  sceVu0OuterProduct(t,impulse,c->normal);
  sceVu0OuterProduct(r,c->normal,t);


  slip=my_sqrtf(r[0]*r[0]+r[1]*r[1]*r[2]*r[3]);
  friction_limit=ABS(sceVu0InnerProduct(impulse,c->normal));
  friction_limit*=mu;




  if (slip>=friction_limit) {
    n=(gamma-beta*friction_limit);
    n/=alpha;
    
    sceVu0ScaleVector(impulse,c->normal,n);
    
    sceVu0Normalize(r,r);
    sceVu0ScaleVector(temp,r,friction_limit);
    sceVu0AddVector(impulse,impulse,temp);
  }

    if (c->bodyA->type!=PASSIVE) {    
      sceVu0AddVector(*(X+STATE_DIM*aID+IMP),*(X+STATE_DIM*aID+IMP),impulse);
      sceVu0OuterProduct(angimpulse,c->ra,impulse);
      sceVu0AddVector(*(X+STATE_DIM*aID+ANG),*(X+STATE_DIM*aID+ANG),angimpulse);
   }

   if (c->bodyB->type!=PASSIVE) {    
     sceVu0SubVector(*(X+STATE_DIM*bID+IMP),*(X+STATE_DIM*bID+IMP),impulse);
     sceVu0OuterProduct(angimpulse,c->rb,impulse);
     sceVu0SubVector(*(X+STATE_DIM*bID+ANG),*(X+STATE_DIM*bID+ANG),angimpulse);
   }
}


void add_nofriction_impulse(Contact *c,sceVu0FVECTOR v,sceVu0FVECTOR *X,int aID,int bID,u_int microc) 
{
  sceVu0FVECTOR temp,temp2;
  sceVu0FVECTOR impulse,angimpulse;
  float fact;
  float epsilon=0.7f;
  float help;

  if (microc) 
      //micro_collision
     #define EPSILON_MAX 15.0
     if(c->dist<COLL_DIST && c->vrel > -0.4f) {
       help=1.0f-c->dist/COLL_DIST;
       epsilon=(EPSILON_MAX-epsilon)*help+epsilon;
     } 

      
  if (c->bodyA->type!=PASSIVE) {
    sceVu0OuterProduct(temp,c->ra,c->normal);
    sceVu0ApplyMatrix(temp,c->bodyA->Iinv_t,temp);
    sceVu0OuterProduct(temp,temp,c->ra);
  }
 
  if (c->bodyB->type!=PASSIVE) {
    sceVu0OuterProduct(temp2,c->rb,c->normal);
    sceVu0ApplyMatrix(temp2,c->bodyB->Iinv_t,temp2);
    sceVu0OuterProduct(temp2,temp2,c->rb);
  }
 
 
   if (c->bodyA->type==PASSIVE) {
     fact=-(1+epsilon)*c->vrel/(sceVu0InnerProduct(temp2,c->normal)+c->bodyB->invM);
   } else if (c->bodyB->type==PASSIVE) {
     fact=-(1+epsilon)*c->vrel/(sceVu0InnerProduct(temp,c->normal)+c->bodyA->invM);
   } else {
     fact=-(1+epsilon)*c->vrel/(sceVu0InnerProduct(temp2,c->normal)+sceVu0InnerProduct(temp,c->normal)+c->bodyA->invM+c->bodyB->invM);
   }
 
   sceVu0ScaleVector(impulse,c->normal,fact);
 
   if (c->bodyA->type!=PASSIVE) {
     sceVu0AddVector(*(X+STATE_DIM*aID+IMP),*(X+STATE_DIM*aID+IMP),impulse);
     sceVu0OuterProduct(angimpulse,c->ra,impulse);
     sceVu0AddVector(*(X+STATE_DIM*aID+ANG),*(X+STATE_DIM*aID+ANG),angimpulse);
   }
 
   if (c->bodyB->type!=PASSIVE) {
     sceVu0SubVector(*(X+STATE_DIM*bID+IMP),*(X+STATE_DIM*bID+IMP),impulse);
     sceVu0OuterProduct(angimpulse,c->rb,impulse);
     sceVu0SubVector(*(X+STATE_DIM*bID+ANG),*(X+STATE_DIM*bID+ANG),angimpulse);
   }                                                                            
}

void tolerance_corr(Contact *c,sceVu0FVECTOR *X,int aID,int bID) 
{
  sceVu0FVECTOR temp,factA,factB;
  float f;


  if (c->bodyA->type!=PASSIVE) {    
    sceVu0ScaleVector(factA,c->normal,c->bodyA->invM);
  } else {
    sceVu0ScaleVector(factA,factA,0.0f);
  }

  if (c->bodyB->type!=PASSIVE) {    
    sceVu0ScaleVector(factB,c->normal,c->bodyB->invM);
  } else {
    sceVu0ScaleVector(factB,factB,0.0f);
  }

  sceVu0AddVector(temp,factA,factB);
  f=-(c->dist-COLL_DIST)/sceVu0InnerProduct(temp,c->normal);


  if (c->bodyA->type!=PASSIVE) {    
    sceVu0ScaleVector(temp,c->normal,f*c->bodyA->invM);
    sceVu0AddVector(*(X+aID*STATE_DIM+POS),*(X+aID*STATE_DIM+POS),temp);
  }

  if (c->bodyB->type!=PASSIVE) {    
    sceVu0ScaleVector(temp,c->normal,f*c->bodyB->invM);
    sceVu0SubVector(*(X+bID*STATE_DIM+POS),*(X+bID*STATE_DIM+POS),temp);
  }   
}
