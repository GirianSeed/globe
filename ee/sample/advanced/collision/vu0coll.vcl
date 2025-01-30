;******************************************************
; input:                                              *
;    simplex(minkowski)-points,number of points       *
;    simplex can consist of max 4 points              *
; output:                                             *
;    newSimpl:                                        *
;	x-field:number of points (can be 1 or 2)      *
;	yz: point indices                             *
;    closestP:                                        *
;	closet point to zero(0,0,0)		      *
;    minDistSqr:                                      *
;	closestP*closestP                             *
;    disjoint:                                        *
;       0 ... line-segment goest through zero         *
;       1 ... closet point/feature is found           *
;	2 ... closet point/feature could not be found *      
;******************************************************

minkpoints  .assigna 0 ;Vu0Mem-addr. to store the mink.-points 
minSimplex  .assigna 8 ;Vu0Mem-addr. to store the simplex with the min-distance to (0,0,0)

		 .init_vf_all
		 .init_vi_all
		 

		 .name vu0CheckSegments

		 .syntax	new			; new syntax for names ending in x/y/z/w etc
        --enter
        in_vf   simplex0(vf01)   
        in_vf   simplex1(vf02)
        in_vf   simplex2(vf03)
        in_vf   simplex3(vf04)
 	in_vi   numPoints(vi01) 
        --endenter   
		 ; init registers
		 sub	newSimpl,vf00,vf00 ;newSimpl.x=number_of_points,newSimpl.yzw=point_indices
		 sub	closestP,vf00,vf00 
		 sub	minNormal,vf00,vf00
		 sub	minPoint,vf00,vf00
		 sub	normal,vf00,vf00
	         sub     minDistSqr,vf00,vf00
	
		 ;save the points to memory
       		 SQ.xyz simplex0, \&minkpoints+0(vi00)
		 SQ.xyz simplex1, \&minkpoints+1(vi00) 
		 SQ.xyz simplex2, \&minkpoints+2(vi00)
		 SQ.xyz simplex3, \&minkpoints+3(vi00)

		 rinit 		 R,vf00[x]; new syntax requires this
		 rget		 one,R
        	 iaddiu one, vi00, 1
                           
		 loi 3.40282347e+38  ; max float
		 addi.x minDistSqr, vf00, I  
        
                                   
		 iaddiu ltzMask, vi00, 0x80  
		 iaddiu lezMask, vi00, 0x88  

		 iaddiu disjoint, vi00, 0

	 	 ;loop over segments(point-pairs) -> k_loop,l_oop           
  		 iadd k, vi00, vi00                  ; i=0
k_loop:                                              ; for (k=0;k<numPoints-1;k++)
		 lq.xyz pointk,\&minkpoints(k)  
		 SUB.xyz trans,vf00,pointk

		 iaddiu l,k,1		 		 		     
l_loop:		 		 		 ; for (l=k+1;l<numPoints;l++)
		 add.x t, vf00, vf00             ; t=0.0;
		 mfir.x newSimpl, one 		 ; one element
		 mfir.y newSimpl, k 		 ; first point-index


		 lq.xyz pointl,\&minkpoints(l)
		 SUB.xyz span,pointl,pointk

		 
		 MUL.xyz		 _sce_innerPtemp,span,span
		 ADDAy.x		 ACC,_sce_innerPtemp,_sce_innerPtemp
		 MADDz.x 		 spanspan,one,_sce_innerPtemp
		 
		 MUL.xyz		 _sce_innerPtemp,trans,span
		 ADDAy.x		 ACC,_sce_innerPtemp,_sce_innerPtemp
		 MADDz.x 		 spantrans,one,_sce_innerPtemp
		 		 
		 DIV Q,spantrans[x],spanspan[x]	   ; new syntax requires this
		 ADDQ.x t_temp, vf00, Q            ;is spantrans/spanspan < 0 ?	

        	 fmand tempi, lezMask              
        	 move.xyz normal,pointk           
		 ibne tempi, vi00, t_end           ; branch if t <= 0.0 ->pointk is the closest point
 
  		 sub.x vf00,t_temp,one             ; >= 1.0 ? 
                 fmand tempi, ltzMask              ; branch if t >= 1.0 -> pointl is the closest point
		 mfir.y newSimpl, l                ; first point-index
        	 add.x t, vf00, vf00[w] 	  		
		 move.xyz  normal,pointl 
        	 ibeq tempi, vi00, t_end
 
		 add.x t, vf00,t_temp 
		 iaddiu two, vi00, 2               ; closest point is between on the line-segement(from pointk to pointl)
		 mfir.x newSimpl, two ; 2 elemets
		 mfir.y newSimpl, k   ; 1st point-index
		 mfir.z newSimpl, l   ; 2nd point-index

	         
		 OPMULA.xyz 		 ACC,pointk,span
		 OPMSUB.xyz 		 tempv,span,pointk		      
		 OPMULA.xyz 		 ACC,span,tempv
		 OPMSUB.xyz 		 normal,tempv,span		 
t_end:


		 ; project all minkpoints onto normal
		 ; and check if any other point is closer than
		 ; than the current feature

		 iaddiu m,vi00,1		        
		 iadd index, vi00, vi00         
		 lq.xyz point0,\&minkpoints(vi00)

		 MUL.xyz		 _sce_innerPtemp,normal,point0
		 ADDAy.x		 ACC,_sce_innerPtemp,_sce_innerPtemp
		 MADDz.x dot1,one,_sce_innerPtemp

m_loop:		 		 		 		 ;for (m=1;m<numPoints;m++)
		 lq.xyz pointm,\&minkpoints(m)		 		 		 
		 MUL.xyz		 _sce_innerPtemp,normal,pointm
		 ADDAy.x		 ACC,_sce_innerPtemp,_sce_innerPtemp
		 MADDz.x dot,one,_sce_innerPtemp
		 
		 SUB.x dot_diff,dot1,dot		 
		 fmand result, lezMask
		 ibne result, vi00, inc_m
		 move.x dot1, dot             ; if (dot < dot1) { dot1 = dot; index = m; }
		 iadd index, m, vi00                
 

inc_m:
		 iaddiu m, m, 1
		 ibne m, numPoints,m_loop     


 		 ; calc distanceSqr and closest-point		 
		 MULx.xyz help,span,t
		 ADD.xyz closestP,pointk,help

		 MUL.xyz _sce_innerPtemp,closestP,closestP
		 ADDAy.x ACC,_sce_innerPtemp,_sce_innerPtemp
		 MADDz.x distSqr,one,_sce_innerPtemp

		 
		 add.x vf00, vf00, dot1                
	         fmand tempi, lezMask
 		 ibne tempi, vi00, NextPointSegment
		 ibeq index, k, exitP ;if index==k OR index==l then current feature is closest simplex
		 ibeq index, l, exitP
 

		 sub.x vf00, distSqr, minDistSqr  ;if index!=k AND index!=k store current simplex if
		 fmand result,ltzMask             ;it it-s closest point is the closest point found
		 ibeq result, vi00, NextPointSegment ; until now
		 add.x minDistSqr, distSqr, vf00       
		 iaddiu disjoint, vi00, 2		 
		 sub minNormal, vf00, normal  
		 sq newSimpl, \&minSimplex(vi00)
		 add minPoint, closestP, vf00

NextPointSegment: 
		 iaddiu l, l, 1
		 ibne l, numPoints, l_loop
		 iaddiu k, k, 1
		 iaddiu helpi, k, 1               ; k=0 ... numPoins-1;
		 ibne helpi, numPoints, k_loop  




		 lq newSimpl, \&minSimplex(vi00)
		 add closestP, minPoint, vf00 
		 --cont

        --exit
        out_vf   newSimpl(vf31)
        out_vf   closestP(vf30)
        out_vf   minDistSqr(vf29)
	out_vi   disjoint(vi15)

        --endexit   

exitP:
 
		 iaddiu disjoint, vi00, 1	
		 add.x minDistSqr, distSqr, vf00  		 
		 --cont


        --exit
        out_vf   newSimpl(vf31)	; specify the registers
        out_vf   closestP(vf30)			
        out_vf   minDistSqr(vf29) 
	out_vi   disjoint(vi15)

        --endexit   
; -------------------------------------------------------------------

.END











