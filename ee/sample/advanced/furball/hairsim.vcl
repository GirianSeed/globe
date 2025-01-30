

	.init_vf_all
        .init_vi_all
	.syntax	new
		 

	.name hair 


	.macro	doRopeSegment	offset
 
        lq.xyz pos,\offset(posp):TAG\@		; tag because at the end of the loop, posp is now pointing to where newnextpos was stored on previous pass..
        lq.xyz nextpos,\offset+1(posp)

        sub.xyz diff,nextpos,pos		; npos-pos

        MUL.xyz temp,diff,diff
        ADDAy.x ACC,temp,temp
        MADDz.x lengthsqr,CONST,temp    	; len_sqr

        add.x   help,lengthsqr,parameter[y]		; len_sqr+rl_sqr
        div   	Q,parameter[y],help[x]	   	; rl_sq/(len_sq+rl_sq)
        subq.z 	fact,CONST,q 			; 0.5 - rl_sq/(len_sq+rl_sq) 
 
        mul.xyz delta,diff,fact[z]	

        add.xyz newpos,pos,delta
        sub.xyz newnextpos,nextpos,delta
 
        sq.xyz newpos,\offset(posp)
        sq.xyz newnextpos,\offset+1(posp):TAG\@
	.endm         
 



        --enter                                                         ; entry point for vcl
        in_vi   numofparticle(vi01)
        in_vf   accel(vf31)
        in_vf   origin(vf30)
        in_vf   parameter(vf29)  ;x=hair_tension,y=restlength*restlength_sqr,z=tstep
        --endenter
 
       iadd new_posp,vi00,vi00
       iadd old_posp,vi00,numofparticle
       iadd num,vi00,numofparticle
 
 
 
calcforce:
       ibeq  numofparticle,vi00,next
       lq.xyz pos,0(old_posp)
       sub.xyz help,pos,origin
       mulx.xyz force,help,parameter[x] ;parameter.x=hair_tension
       add.xyz faccel,accel,force
 
 
       iaddiu hairlength,vi00,4
loop:
 
        --LoopCS        1,1
 
        isubiu,numofparticle,numofparticle,1
        isubiu,hairlength,hairlength,1
 
        lq.xyz current_pos,0(new_posp)
        lq.xyz old_pos,0(old_posp)
 
 
 
 
        suba.xyz ACC,current_pos,old_pos
        maddaw.xyz ACC,current_pos,vf00
        maddz.xyz new_pos,faccel,parameter[z]       ;new_pos=2*current_pos-old_pos+faccel*tstep*tstep
 
 
        sqi.xyz current_pos,(old_posp++)
        sqi.xyz new_pos,(new_posp++)
 
        ibeq            hairlength,vi00,calcforce
        ibne            numofparticle,vi00,loop
 

; #####################################################################################
next:
        iadd  	posp,vi00,vi00 
        iaddiu 	nextposp,vi00,1

        iaddiu  nconstraint,vi00,3
        iaddiu  nrope8,vi00,4	   	; number of ropes(128/4)/8 

        LOI 0.5  		
        addi.z 	CONST,vf00,I		; CONST.z = 0.5
	loi 1.0 			
	addi.x	CONST,vf00,i		; CONST.x = 1.0

constraintloop:



        --LoopCS 	1,0	; can be called as little as 2 times, and don't over-write the end
	--LoopExtra	4

;	do 8 ropes at once

	doRopeSegment	0
	doRopeSegment  4	
	doRopeSegment  8	
	doRopeSegment  12	

	doRopeSegment 16	
	doRopeSegment 20	
	doRopeSegment 24	
	doRopeSegment 28	

	iaddiu	posp,posp,1
        isubiu 	nconstraint,nconstraint,1
        ibne   	nconstraint,vi00,constraintloop
          
        isubiu nrope8,nrope8,1		; done all the ropes?
        ibeq   nrope8,vi00,exit  


        iaddiu nconstraint,vi00,3	
        iaddiu  posp,posp,29		; onto next block
        b constraintloop
 
exit:

;**************************************************************************************

        --exit 
        out_vf   accel(vf31)
        out_vf   origin(vf30)
        out_vf   parameter(vf29)  ;x=hair_tension,y=restlength*restlength_sqr,z=tstep           
        --endexit




