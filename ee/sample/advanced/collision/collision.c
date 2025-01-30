/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : collision.c 
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            1.0     Nov 28, 2001     eder      Initial
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
#include "util.h"
#include "camera.h"
#include "collision.h"
#include <math.h>
#include "vector.h"

enum {OBJA,OBJB};

/* for calculating the minkowski-sum */
static sceVu0FVECTOR minkpoints[4];


extern unsigned int  vu0CodeStart __attribute__((section(".vudata")));
extern unsigned int  vu0CheckSegments_CodeStart[];

void collision_init()
{
  sceDmaChan* dmaVIF0;
 
  dmaVIF0 = sceDmaGetChan(SCE_DMA_VIF0);
  dmaVIF0->chcr.TTE = 1;
  sceDmaSync(dmaVIF0, 0, 0);
  sceDmaSend(dmaVIF0, &vu0CodeStart);
  sceDmaSync(dmaVIF0, 0, 0);
}


inline void
Vu0Reset()
{
  // Set the reset Vu0 bit of the FBRST register
 
  asm volatile ("
                cfc2 $2, $vi28;
                ori $2, $2, 2;
                ctc2 $2, $vi28;
                "
                : : : "$2");
}                         


void get_support_point_mesh(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points,int *hint)
{

  int           i, lastPointIndex, neighborPointIndex, moving;
  float         currentDot, neighborDot;
  sceVu0FVECTOR       *currentPoint, *neighborPoint;
  int index;

  index = lastPointIndex = *hint;
  currentPoint = gjkobj->vp+index;
  currentDot = CpuDotProduct(*currentPoint,*search_dir);

  do
  {
    moving = 0;
    for (i = 0; i < gjkobj->nneighbors[index]; i++)
    {
      neighborPointIndex = gjkobj->neighbors[index][i];
      neighborPoint = gjkobj->vp+neighborPointIndex;
      
      if ((neighborPointIndex != lastPointIndex) &&
          ((neighborDot = CpuDotProduct(*neighborPoint,*search_dir)) > currentDot))
	{
	  lastPointIndex = index;
	  index = neighborPointIndex;
	  currentPoint = neighborPoint;
	  currentDot = neighborDot;
	  
	  moving = 1;
	  break;
	}
    }
  } while (moving);  
  *hint=index;
  CpuCopyVector(*support_points,*(gjkobj->vp+index)); 
}           


void get_support_point_box(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points)
{

  if ((*search_dir)[0] >0) {
    (*support_points)[0]=gjkobj->half_box[0];
  } else {
    (*support_points)[0]=-gjkobj->half_box[0];
  }
  if ((*search_dir)[1] >0) {
    (*support_points)[1]=gjkobj->half_box[1];
  } else {
    (*support_points)[1]=-gjkobj->half_box[1];
  }
  if ((*search_dir)[2] >0) {
    (*support_points)[2]=gjkobj->half_box[2];
  } else {
    (*support_points)[2]=-gjkobj->half_box[2];
  }
}           

void get_support_point_sphere(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points)
{
  Vu0NormalizeVector(*support_points,*search_dir);
  Vu0ScaleVector(*support_points,*support_points,gjkobj->radius);
}           


void get_support_point_cone(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points)
{
  float sigma,lengthSq,radiusSq,help;

  radiusSq=gjkobj->radius*gjkobj->radius;
  lengthSq=Vu0DotProduct(*search_dir,*search_dir);
  if ((*search_dir)[1]>my_sqrtf((radiusSq*lengthSq)/(radiusSq+4*gjkobj->half_length*gjkobj->half_length))) {
    (*support_points)[0]=0.0f;
    (*support_points)[1]=gjkobj->half_length;
    (*support_points)[2]=0.0f;
  } else {
    if ((*search_dir)[0]!=0.0f || (*search_dir)[2] !=0.0f) {
      sigma=my_sqrtf((*search_dir)[0]*(*search_dir)[0]+(*search_dir)[2]*(*search_dir)[2]);
      help=gjkobj->radius/sigma;
      (*support_points)[0]=(*search_dir)[0]*help;
      (*support_points)[1]=-gjkobj->half_length;
      (*support_points)[2]=(*search_dir)[2]*help;
    } else {
      (*support_points)[0]=0.0f;
      (*support_points)[1]=-gjkobj->half_length;
      (*support_points)[2]=0.0f;
    }
  }
}           


void get_support_point_cylinder(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points)
{
  float sigma,help;

  if ((*search_dir)[0]!=0.0f || (*search_dir)[2] !=0.0f) {
    sigma=my_sqrtf((*search_dir)[0]*(*search_dir)[0]+(*search_dir)[2]*(*search_dir)[2]);
    help=gjkobj->radius/sigma;
    (*support_points)[0]=(*search_dir)[0]*help;
    if ((*search_dir)[1] > 0) {
      (*support_points)[1]=gjkobj->half_length;
    } else {
      (*support_points)[1]=-gjkobj->half_length;
    }
    (*support_points)[2]=(*search_dir)[2]*help;
  } else {
    (*support_points)[0]=0.0f;
    if ((*search_dir)[1] > 0) {
      (*support_points)[1]=gjkobj->half_length;
    } else {
      (*support_points)[1]=-gjkobj->half_length;
    }
    (*support_points)[2]=0.0f;
  }  
}           


void get_support_point(GJKObj *gjkobj,sceVu0FVECTOR *search_dir,sceVu0FVECTOR *support_points,int *hint)
{
  switch (gjkobj->type) {
  case GJK_MESH:
    get_support_point_mesh(gjkobj,search_dir,support_points,hint);
    break;
  case GJK_BOX:
    get_support_point_box(gjkobj,search_dir,support_points);
    break;
  case GJK_SPHERE:
    get_support_point_sphere(gjkobj,search_dir,support_points);
    break;
  case GJK_CONE:
    get_support_point_cone(gjkobj,search_dir,support_points);
    break;
  case GJK_CYLINDER:
    get_support_point_cylinder(gjkobj,search_dir,support_points);
    break;
  }

}



void closest_points(Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB,sceVu0FVECTOR *v)
{
  sceVu0FVECTOR lamda,span,help,help2,help3;
  float diff=0,invA,dot;
  u_int indexA=0,indexB=0,elem;

  if(simplex->Wk==3) {
		
    CpuSubVector(help,minkpoints[0],minkpoints[2]);
    CpuSubVector(help2,minkpoints[1],minkpoints[2]);
    CpuSubVector(help3,*v,minkpoints[2]);
    
		
    /* lamda*/		
    for (elem=0;elem<3;elem++) {
      if (help[elem]>0.0002f || help[elem]<-0.0002f   	) {   // error-bound
	//if (help[elem]!=0) { 
	indexA=elem;
	break;
      }
    } 
    
    invA=1.0f/help[indexA];
    for (elem=0;elem<3;elem++) {
      if (elem!=indexA) {
	diff=help2[elem]-help[elem]*help2[indexA]*invA;
	if (diff>0.0002f || diff<-0.0002f   	) {   // error-bound
	  indexB=elem;
	  break;
	}
      }
    }
    
    lamda[1]=help3[indexB]-help3[indexA]*help[indexB]*invA;
    lamda[1]/=diff;
    lamda[0]=(help3[indexA]-lamda[1]*help2[indexA])*invA;
    lamda[2]=1-lamda[0]-lamda[1];
    

    CpuScaleVector(simplex->closest_pointA,simplex->support_pointsA[0],lamda[0]);
    CpuScaleVector(help,simplex->support_pointsA[1],lamda[1]);		
    CpuScaleVector(help2,simplex->support_pointsA[2],lamda[2]);
    CpuAddVector(simplex->closest_pointA,simplex->closest_pointA,help);
    CpuAddVector(simplex->closest_pointA,simplex->closest_pointA,help2);
    
    
    CpuScaleVector(simplex->closest_pointB,simplex->support_pointsB[0],lamda[0]);
    CpuScaleVector(help,simplex->support_pointsB[1],lamda[1]);		
    CpuScaleVector(help2,simplex->support_pointsB[2],lamda[2]);
    CpuAddVector(simplex->closest_pointB,simplex->closest_pointB,help);
    CpuAddVector(simplex->closest_pointB,simplex->closest_pointB,help2);
  }

  if (simplex->Wk==2) {

    Vu0SubVector(span,minkpoints[0],minkpoints[1]);
    dot=CpuDotProduct(span,minkpoints[0])/CpuDotProduct(span,span);

    Vu0SubVector(span,simplex->support_pointsA[1],simplex->support_pointsA[0]);
    CpuScaleVector(help,span,dot);
    CpuAddVector(simplex->closest_pointA,simplex->support_pointsA[0],help);
    
    Vu0SubVector(span,simplex->support_pointsB[1],simplex->support_pointsB[0]);
    CpuScaleVector(help,span,dot);
    CpuAddVector(simplex->closest_pointB,simplex->support_pointsB[0],help);
  }    

  if (simplex->Wk==1) {
    CpuCopyVector(simplex->closest_pointA,simplex->support_pointsA[0]);
    CpuCopyVector(simplex->closest_pointB,simplex->support_pointsB[0]);
  }
}     

     

int  is_not_unique(Simplex *simplex) {
  u_char i;

  for (i=0;i<simplex->Wk;i++){   
    if(Vu0CompareVector(minkpoints[i],minkpoints[simplex->Wk])) return 1; 
  }
  return 0;
}    
 

enum {NO_COLL,COLL};

float DistanceSqr(sceVu0FVECTOR *normal,Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB,u_int npoints);

int is_collision(Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB)
{
  sceVu0FVECTOR v; 
  sceVu0FVECTOR mv; 
  sceVu0FVECTOR h1 ;
  sceVu0FVECTOR h2 ;
  sceVu0FMATRIX world_local[2];
  u_char i;
  float distSqr;
  
  if (simplex->cache_valid && simplex->Wk !=0 ) {    
    for (i=0;i<simplex->Wk;i++){
      Vu0TransformVector(h1,*(gjkobjA->lw),simplex->support_pointsA[i]);
      Vu0TransformVector(h2,*(gjkobjB->lw),simplex->support_pointsB[i]);
      Vu0SubVector(minkpoints[i],h1,h2); /* w:= S_{A-B}(-v) */      
    }
    distSqr=DistanceSqr(&v,simplex,gjkobjA,gjkobjB,simplex->Wk);
    if (distSqr-simplex->epsilon<=0.0f) return COLL;
  } else {
    if (gjkobjA->type==GJK_MESH) {
      CpuCopyVector(simplex->support_pointsA[0],*gjkobjA->vp);
      Vu0TransformVector(h1,*gjkobjA->lw,*gjkobjA->vp);
    } else {
      Vu0ScaleVector(simplex->support_pointsA[0],simplex->support_pointsA[0],0.0f);
      CpuCopyVector(h1,(*gjkobjA->lw)[3]);
    }
    if (gjkobjB->type==GJK_MESH) {
      CpuCopyVector(simplex->support_pointsB[0],*gjkobjB->vp);
      Vu0TransformVector(h2,*gjkobjB->lw,*gjkobjB->vp);         
    } else {
      Vu0ScaleVector(simplex->support_pointsB[0],simplex->support_pointsB[0],0.0f);
      CpuCopyVector(h2,(*gjkobjB->lw)[3]);
    }   
    Vu0SubVector(v,h1,h2);
    if (CpuDotProduct(v,v)==0.0f) return 0.0f;
    simplex->cache_valid=1;
    simplex->Wk=1;
    CpuCopyVector(minkpoints[0],v);
  }
  
  
  Vu0InvertRotation(world_local[OBJA],*(gjkobjA->lwrot));
  Vu0InvertRotation(world_local[OBJB],*(gjkobjB->lwrot));


  do {
    Vu0NegVector(mv,v);
    
    Vu0RotateVector(h1,world_local[OBJA],mv);
    get_support_point(gjkobjA,&h1,&(simplex->support_pointsA[simplex->Wk]),&(simplex->hintA));
    Vu0TransformVector(h1,*(gjkobjA->lw),simplex->support_pointsA[simplex->Wk]);

    Vu0RotateVector(h2,world_local[OBJB],v);
    get_support_point(gjkobjB,&h2,&(simplex->support_pointsB[simplex->Wk]),&(simplex->hintB));
    Vu0TransformVector(h2,*(gjkobjB->lw),simplex->support_pointsB[simplex->Wk]);

    Vu0SubVector(minkpoints[simplex->Wk],h1,h2); /* w:= S_{A-B}(-v) */

    if (CpuDotProduct(minkpoints[simplex->Wk],v) > 0) {
      CpuCopyVector(simplex->cache_vect,v);
      return NO_COLL;
    }

    /* check if new mink.point is unique */
    if (is_not_unique(simplex)) {
      CpuCopyVector(simplex->cache_vect,v);
      return COLL;
    }      
    
    distSqr=DistanceSqr(&v,simplex,gjkobjA,gjkobjB,simplex->Wk+1); /* v:=v'(conv(W u {w})); */

    /* W:="smalles X e W u{w} such that v e conv(X)";*/
  } while ( (simplex->Wk<15) && (distSqr -simplex->epsilon > 0));
  return COLL;
}





float calc_dist(Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB,u_int calc_points)
{
  sceVu0FVECTOR v; 
  sceVu0FVECTOR mv; 
  sceVu0FVECTOR h1 ;
  sceVu0FVECTOR h2 ;
  sceVu0FMATRIX world_local[2];
  u_char i,lastWk;
  float dist,last_dist;
  
  if (simplex->cache_valid && simplex->Wk) {
    for (i=0;i<simplex->Wk;i++){
      Vu0TransformVector(h1,*(gjkobjA->lw),simplex->support_pointsA[i]);
      Vu0TransformVector(h2,*(gjkobjB->lw),simplex->support_pointsB[i]);
      Vu0SubVector(minkpoints[i],h1,h2); /* w:= S_{A-B}(-v) */      
    }
    dist=DistanceSqr(&v,simplex,gjkobjA,gjkobjB,simplex->Wk);
    if (dist==0.0f) return 0.0f;
  } else {
    if (gjkobjA->type==GJK_MESH) {
      CpuCopyVector(simplex->support_pointsA[0],*gjkobjA->vp);
      Vu0TransformVector(h1,*gjkobjA->lw,*gjkobjA->vp);
    } else {
      Vu0ScaleVector(simplex->support_pointsA[0],simplex->support_pointsA[0],0.0f);
      CpuCopyVector(h1,(*gjkobjA->lw)[3]);
    }
    if (gjkobjB->type==GJK_MESH) {
      CpuCopyVector(simplex->support_pointsB[0],*gjkobjB->vp);
      Vu0TransformVector(h2,*gjkobjB->lw,*gjkobjB->vp);         
    } else {
      Vu0ScaleVector(simplex->support_pointsB[0],simplex->support_pointsB[0],0.0f);
      CpuCopyVector(h2,(*gjkobjB->lw)[3]);
    }   
    Vu0SubVector(v,h1,h2);
    if (CpuDotProduct(v,v)==0.0f) return 0.0f;
    simplex->cache_valid=1;
    simplex->Wk=1;
    CpuCopyVector(minkpoints[0],v);
    dist=CpuDotProduct(v,v);
  }


  Vu0InvertRotation(world_local[OBJA],*(gjkobjA->lwrot));
  Vu0InvertRotation(world_local[OBJB],*(gjkobjB->lwrot));

  last_dist = dist;
  lastWk = simplex->Wk;

  while ((simplex->Wk<4) && (dist !=0)) {

    Vu0NegVector(mv,v);
     
    Vu0RotateVector(h1,world_local[OBJA],mv);
    get_support_point(gjkobjA,&h1,&(simplex->support_pointsA[simplex->Wk]),&(simplex->hintA));
    Vu0TransformVector(h1,*(gjkobjA->lw),simplex->support_pointsA[simplex->Wk]);


    Vu0RotateVector(h2,world_local[OBJB],v);
    get_support_point(gjkobjB,&h2,&(simplex->support_pointsB[simplex->Wk]),&(simplex->hintB)); 
    Vu0TransformVector(h2,*(gjkobjB->lw),simplex->support_pointsB[simplex->Wk]);


    Vu0SubVector(minkpoints[simplex->Wk],h1,h2); /* w:= S_{A-B}(-v) */
    /* check if new mink.point is unique */
    if (is_not_unique(simplex)) {
      break;      
    }


    /*v:=v'(conv(W u {w})); */
    /* Wk+1:="smallest X e Wk u{w} such that v e conv(X)";*/
    dist=DistanceSqr(&v,simplex,gjkobjA,gjkobjB,simplex->Wk+1);
    if (dist-simplex->epsilon<0) {
      return 0.0f;
    }

    if (last_dist-dist-simplex->epsilon <= 0) {
	dist = last_dist;
	simplex->Wk = lastWk;
	break;
    }
    else {
      last_dist = dist;
      lastWk = simplex->Wk;
    }  
  } 
  
  if (calc_points) closest_points(simplex,gjkobjA,gjkobjB,&v);
  CpuCopyVector(simplex->cache_vect,v);
  return dist; 
}

#define QWSIZE 4
int alloc_gjkobj_neighbors(void *gjk_data_addr,GJKObj *gjkobj)
{
  int j;
  u_int numofV,tot_neighbor;
  u_int  *datap;

  numofV= *((u_int *)gjk_data_addr);
  tot_neighbor=*(u_int *)((u_int *)gjk_data_addr+4*numofV+4);


  gjkobj->vp=(sceVu0FVECTOR *) ((u_int *)gjk_data_addr+4);  
  gjkobj->neighbors= (u_int **) sceHiMemAlign(16, sizeof(u_int *) * numofV); 
  gjkobj->nneighbors= (u_int *) sceHiMemAlign(16, sizeof(u_int) * numofV);
  if (gjkobj->neighbors == NULL || gjkobj->nneighbors == NULL) return 0;  

  datap=((u_int *)gjk_data_addr+QWSIZE*numofV+QWSIZE+QWSIZE);
  for (j=0;j<numofV;j++) {
    gjkobj->nneighbors[j]=*datap;
    gjkobj->neighbors[j]=datap+1;
    datap+=gjkobj->nneighbors[j]+1;
  }
  return 1;
}

void free_gjkobj_neighbors(GJKObj *gjkobj) 
{
  if (gjkobj->type==GJK_MESH){
    sceHiMemFree(gjkobj->neighbors);
    sceHiMemFree(gjkobj->nneighbors);
  }
}


#define FLT_MAX 3.40282347e+38f  
float DistanceSqr(sceVu0FVECTOR *normal,Simplex *simplex,GJKObj *gjkobjA,GJKObj *gjkobjB,u_int npoints)
{
  sceVu0FVECTOR          help,help2,tempVector,pointsVector;
  sceVu0FVECTOR          edge, edgeNormal, triNormal,point;
  sceVu0FVECTOR          minpoint;
  sceVu0FVECTOR          minnormal;
  sceVu0IVECTOR indices;
  sceVu0FVECTOR closestP;
  u_int disjointVu0;
  u_int disjoint = 0;
  int    i, j, k;
  int    numPoints, offTriIndex, index1;
  float minDistSqr, distSqr;
  float dot,dot1,dot_tri;
  u_int   min_pointIndices[4];
  u_int   min_numPoints=0;
  float trihelp;
  float minDistSqrVu0;


  minDistSqr = FLT_MAX;
  numPoints = npoints;


  


  // point
  if (numPoints == 1) {
    CpuCopyVector(*normal,minkpoints[0]);
    distSqr=CpuDotProduct(*normal,*normal);
    simplex->Wk=1;
    return distSqr;
  }

  // line segment
  asm volatile ("
                lqc2 $vf01, 0x00(%0);                     # Load the points
                lqc2 $vf02, 0x10(%0);
                lqc2 $vf03, 0x20(%0);
                lqc2 $vf04, 0x30(%0);
                ctc2 %1, $vi01;                           # Load the number of points in each simplex
                vcallms  vu0CheckSegments_CodeStart      # Call microcode
                "
		    :
		    : "r" (&(minkpoints[0][0])), "r" (numPoints)
		);

  
  // Triangles

  for (i = 0; i < numPoints - 2; i++)
  {
    for (j = i + 1; j < numPoints - 1; j++)
    {
      for (k = j + 1; k < numPoints; k++)
      {
	CpuSubVector(help,minkpoints[j],minkpoints[i]);
	CpuSubVector(help2,minkpoints[k],minkpoints[i]);
	CpuCrossProduct(triNormal,help,help2);
	trihelp=CpuDotProduct(triNormal,triNormal);
	CpuScaleVector(triNormal,triNormal,my_rsqrtf(1.0f,trihelp));
	//CpuScaleVector(triNormal,triNormal,1.0f/sqrtf(trihelp));	
        if (CpuDotProduct(minkpoints[0],triNormal) < 0.0f)
        {
	  CpuCopyVector(*normal,triNormal);
	  CpuScaleVector(*normal,*normal,-1.0);
        }
        else
        {
	  CpuCopyVector(*normal,triNormal);
        }



        index1 = i;
        dot1 = dot_tri = CpuDotProduct(*normal,minkpoints[i]);
        offTriIndex = 6 - (i + j + k);
        if (offTriIndex < numPoints)
        {
	  dot =CpuDotProduct(*normal,minkpoints[offTriIndex]);
          if (dot < dot1)
          {
            index1 = offTriIndex;
            dot1 = dot;
          }
        }

        if (dot1 > 0)
        {

          CpuCopyVector(tempVector,minkpoints[i]);
	  CpuScaleVector(tempVector,tempVector,-1.0f);
	  CpuSubVector(edge,minkpoints[j],minkpoints[i]);
	  CpuCrossProduct(edgeNormal,triNormal,edge);
	
          if (CpuDotProduct(tempVector,edgeNormal) > 0.0f)
          {
            
	    CpuCopyVector(tempVector,minkpoints[j]);
	    CpuScaleVector(tempVector,tempVector,-1.0f);
	    CpuSubVector(edge,minkpoints[k],minkpoints[j]);
	    CpuCrossProduct(edgeNormal,triNormal,edge);
	    if (CpuDotProduct(tempVector,edgeNormal) > 0.0f)
            {

	      CpuCopyVector(tempVector,minkpoints[k]);
	      CpuScaleVector(tempVector,tempVector,-1.0f);
	      CpuSubVector(edge,minkpoints[i],minkpoints[k]);
	      CpuCrossProduct(edgeNormal,triNormal,edge);
	      
	      if (CpuDotProduct(tempVector,edgeNormal) > 0.0f)
              {            	              		                 	
                CpuScaleVector(pointsVector,*normal,dot_tri);
                CpuCopyVector(point,pointsVector);
		CpuCopyVector(*normal,pointsVector);
		distSqr=CpuDotProduct(pointsVector,pointsVector);
		

                if (index1 == i)
                {
                  
		  CpuCopyVector(minkpoints[0],minkpoints[i]);
		  CpuCopyVector(minkpoints[1],minkpoints[j]);
		  CpuCopyVector(minkpoints[2],minkpoints[k]);


		  CpuCopyVector(simplex->support_pointsA[0],simplex->support_pointsA[i]);
		  CpuCopyVector(simplex->support_pointsA[1],simplex->support_pointsA[j]);
		  CpuCopyVector(simplex->support_pointsA[2],simplex->support_pointsA[k]);

		  CpuCopyVector(simplex->support_pointsB[0],simplex->support_pointsB[i]);
		  CpuCopyVector(simplex->support_pointsB[1],simplex->support_pointsB[j]);
		  CpuCopyVector(simplex->support_pointsB[2],simplex->support_pointsB[k]);

		  simplex->Wk=3;
		  Vu0Reset();/* closest dist found ->reset Vu0*/

		  return distSqr;
		  
		}
		else
		  {             	
		    if (distSqr < minDistSqr){
                    disjoint = 1;
                    minDistSqr = distSqr;
		    CpuCopyVector(minpoint,point);
                    CpuCopyVector(minnormal,*normal);
                    min_numPoints = 3;
                    min_pointIndices[0] = i;
                    min_pointIndices[1] = j;
                    min_pointIndices[2] = k;
                  }
                }
              }
      	    }
      	  }
      	}
      }
    }
  }

  // check line segement results
  asm volatile ("
                cfc2.i %0, $vi15;                       
                sqc2 $vf31, 0x00(%2);                   
                sqc2 $vf30, 0x00(%3);                   
                qmfc2 %1, $vf29;                        
                "
                : "=&r" (disjointVu0), "=r" (minDistSqrVu0)
                : "r" (&indices[0]), "r" (&closestP[0])
                : "cc", "memory");


  if (disjointVu0 == 1)
  {
    CpuCopyVector(*normal,closestP);
    simplex->Wk=indices[0];
    for (i = 0; i < simplex->Wk; i++)
      {
	CpuCopyVector(minkpoints[i],minkpoints[indices[i+1]]);
	CpuCopyVector(simplex->support_pointsA[i],simplex->support_pointsA[indices[i+1]]);
	CpuCopyVector(simplex->support_pointsB[i],simplex->support_pointsB[indices[i+1]]);
      }
    return minDistSqrVu0;
  }
  else if (disjoint || (disjointVu0 == 2))
    {
      if (minDistSqrVu0 < minDistSqr)
	{
	  CpuCopyVector(*normal,closestP);
	  simplex->Wk=indices[0];
	  for (i = 0; i < simplex->Wk; i++)
	    {
	      CpuCopyVector(minkpoints[i],minkpoints[indices[i+1]]);
	      CpuCopyVector(simplex->support_pointsA[i],simplex->support_pointsA[indices[i+1]]);
	      CpuCopyVector(simplex->support_pointsB[i],simplex->support_pointsB[indices[i+1]]);
	    }

	  return minDistSqrVu0;
	}
      else
	{
	  CpuCopyVector(*normal,minnormal);
	  simplex->Wk=min_numPoints;
	  for (i = 0; i < min_numPoints; i++)
	    {
	      CpuCopyVector(minkpoints[i],minkpoints[min_pointIndices[i]]);
	      CpuCopyVector(simplex->support_pointsA[i],simplex->support_pointsA[min_pointIndices[i]]);
	      CpuCopyVector(simplex->support_pointsB[i],simplex->support_pointsB[min_pointIndices[i]]);
	    }
	  return minDistSqr;
	}
    }
  else {
    simplex->Wk=numPoints;
    return 0.0f;
  }                                                 
}
