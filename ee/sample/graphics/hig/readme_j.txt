[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved


�����x���O���t�B�b�N�X���C�u�����T���v��

<�T���v���̉��>
	�����x���O���t�B�b�N�X���C�u�����̃��[�U�[�C���^�[�t�F�[�X���g�p
	���ēo�^���ꂽ�v���O�C�������s����T���v���ł��B
	�v���O�C����SCE�񋟂̃v���O�C����O��ɂ��Ă��܂��B

<�t�@�C��>
	main.c			SAMPLE PROGRAM�̃V�X�e�������\�[�X
	sample0~32.c		SAMPLE0~32 PROGRAM�\�[�X
	util.c			Utility PROGRAM�\�[�X
	camera.c		�J�����ݒ�v���O�����\�[�X
	light.c			�����ݒ�֘A�v���O�����\�[�X
	sample_func.tbl		SAMPLE PROGRAM FUNCTION�o�^�t�@�C��
	micro.dsm		DVP�\�[�X
	data/*.bin		�f�[�^�o�C�i���t�@�C��
	micro/*.vcl		VCL (VU Command Line)�\�[�X
	micro/*.vsm		VSM�\�[�X

<�N�����@>

        % make run

<�R���g���[���̑�����@>
	���̃T���v���́AHiG�̋@�\���Љ�邽�߂̃T���v���v���O�����Q
	�ɂ���č\������Ă��܂��B
	�N����T���v���̃��j���[������܂��B
	���̃��j���[�ɂ����āA�e�T���v����I���E�N�����邱�Ƃ��ł��܂��B

	�����j���[����{�^����
		��/���i�����{�^���j	���j���[�J�[�\���̈ړ�
		�� �{�^��		���j���[����i�T���v���N���j

	�܂��A�e�T���v�����烁�j���[��ʂɖ߂�ɂ�START�{�^����
	���������Ŕ��ʂ��Ă��܂��B

	���T���v�����[�h����{�^����
		START	���j���[�ɖ߂�

	�ȉ��A�����̃T���v���ɋ��ʂ̑���������܂��B�ڍׂ̓T���v�����Ƃ̐����A
	����ыN����̉�ʕ\��������񂭂������B

	�ꕔ�̃T���v���̓��[�h�̊T�O�������A�e���[�h�� SELECT�{�^���Ő؂�ւ��܂��B
	���ݑ��쒆�̃��[�h�͉�ʂɕ\������܂��B

	CAMERA CTRL:			�J�������䃂�[�h
		��/�� (�����{�^��)	x����]
		��/�� (�����{�^��)	y����]
		L1/L2 �{�^��		z����]
		��/�� �{�^��		x�����ړ�
		��/�~ �{�^��		y�����ړ�
		R1/R2 �{�^��		z�����ړ�
		�J�����̉�]�́A���[���h���W���_�𒆐S�Ƃ�����]�ƂȂ��Ă��܂��B

	MICRO CHANGE:			�}�C�N���ؑւ����[�h
		��/�� �{�^��		�}�C�N���v���O�����̐ؑ�

	LIGHT CTRL:			���C�g���䃂�[�h

		��/�� (�����{�^��)	���C�g�ԍ��̐ؑւ�
		��/�� (�����{�^��)	�p�����[�^�̐ؑւ�
					(DIRECTION, POS/ANGLE, COL/INTENS)

		�EDIRECTION
			��/�� �{�^��	x��������
			��/�~ �{�^��	y��������
			R1/R2 �{�^��	z��������
		�EPOS/ANGLE
			��/�� �{�^��	x�ʒu����
			��/�~ �{�^��	y�ʒu����
			R1/R2 �{�^��	z�ʒu����
			L1/L2 �{�^��	�p�x����
		�ECOL/INTENS
			��/�� �{�^��	R�J���[����
			��/�~ �{�^��	G�J���[����
			R1/R2 �{�^��	B�J���[����
			L1/L2 �{�^��	���x����

���T���v���̐�����

		SAMPLE0		
			es2hig�̏o�̓f�[�^�𗘗p�����P�̃I�u�W�F�N�g
			�\���̃T���v��
			�f�[�^�͓�����HOST�}�V�����烍�[�h����܂��B
			�E�֘A�t�@�C��
				sample0.c
				data/dino.bin

		SAMPLE1
			es2hig�̏o�̓f�[�^�𗘗p�����Q�̃I�u�W�F�N�g��
			�����ɕ\������T���v��
			�f�[�^�͓�����HOST�}�V�����烍�[�h����܂��B
			�E�֘A�t�@�C��
				sample1.c
				data/dino.bin
				data/shuttle.bin

		SAMPLE2
			es2hig�̏o�̓f�[�^�𗘗p�����Q�̃I�u�W�F�N�g��
			BLOCK����֐��ɂ�����ւ����s���T���v��
			�f�[�^�͓�����HOST�}�V�����烍�[�h����܂��B
			�E�֘A�t�@�C��
				sample2.c
				data/dino.bin
				data/shuttle.bin
			������{�^����
				SELECT �{�^��		OBJECT�̓���ւ�

		SAMPLE3
			�ePlugin���g�p�ł���raw data(���f�[�^)����
			Plugin Block/Data Block��BLOCK����֐��ɂ����
			�\�z����T���v��
			�f�[�^�͓�����HOST�}�V�����烍�[�h����܂��B
			�E�֘A�t�@�C��
				sample3.c 
				data/ball_basem.bin
				data/ball_hrchy.bin
				data/ball_micro.bin
				data/ball_anim.bin
				data/ball_key.bin
				data/ball_val.bin
				data/ball1_tex2d.bin
				data/ball1_shape.bin

		SAMPLE4
			�ePlugin���g�p�ł���raw data(���f�[�^)����
			Plugin Block/Data Block��BLOCK����֐��ɂ����
			�\�z���A�J��������̈ʒu�ɂ���ă��f����؂�ւ���
			LOD�̓���������� �T���v��
			�E�֘A�t�@�C��
				sample4.c 
				data/ball_basem.bin
				data/ball_hrchy.bin
				data/ball_micro.bin
				data/ball_anim.bin
				data/ball_key.bin
				data/ball_val.bin
				data/ball1_shape.bin
				data/ball1_tex2d.bin
				data/ball2_shape.bin
				data/ball2_tex2d.bin
				data/ball3_shape.bin
				data/ball3_tex2d.bin
				data/ball4_shape.bin
				data/ball4_tex2d.bin

		SAMPLE5
			HiP�ŕ`��ł���v���~�e�B�u�̃T���v��
			point/line/line strip/triangle/triangle strip/
			triangle fan�̊e�v���~�e�B�u��Texture�̂���/�Ȃ�
			�̌v�P�Q�R�̃��f����\�����܂��B
			�E �֘A�t�@�C��
				sample5.c
				data/point.bin
				data/line.bin
				data/linestrip.bin
				data/triangle.bin
				data/tristrip.bin
				data/trifan.bin
				data/point_t.bin
				data/line_t.bin
				data/linestrip_t.bin
				data/triangle_t.bin
				data/tristrip_t.bin
				data/trifan_t.bin

		SAMPLE6
			Micro Code�̐ؑւ��s���T���v��
			�E �֘A�t�@�C��
				sample6.c
				data/ballS.bin

		SAMPLE7
			es2raw�f�[�^(framework�f�[�^)��HiG�ŗ��p����
			�T���v��
			�E�֘A�t�@�C��
				sample7.c
				data/mbox_raw.bin

		SAMPLE8
			HiG �f�[�^�̓����𑀍삷��T���v��
			�A�N�Z�X�֐����g�p���āAHiG �f�[�^�̓����̒l��ύX���邱��
			�ɂ��A�A�j���[�V������e�N�X�`���̗L���𑀍삵�܂��B
			�E�֘A�t�@�C��
				sample8.c
				data/APEWALK39.bin
				data/dino.bin

		SAMPLE9
			TIM2�T���v��
			�E�֘A�t�@�C��
				sample9.c
				data/compmip.tm2
				data/abc-24.tm2
				data/abc-32.tm2
				data/info8-16.tm2
				data/info8-32-128.tm2
				data/Lysol1.tm2
				data/planeclut4.tm2
				data/tim2_raw.bin

		SAMPLE10
			CPU������DMA�]�����_�u���o�b�t�@�����O���� SAMPLE1
			�A�j���[�V�����̃t���[���R���g���[�����s�Ȃ��܂��B
			�E�֘A�t�@�C��
				sample10.c
				data/APEWALK39.bin
			������{�^����
				L1 �{�^��: �������Đ�
				L2 �{�^��: �t�����Đ�

		SAMPLE11
			PCRTC���g���ĂQ��ʍ���������T���v��
			�E�֘A�t�@�C��
				sample11.c
				data/dino.bin
				data/subwin.raw
			������{�^����
				��/��/��/�� (�����{�^��): �T�u�E�B���h�E�̈ړ�
				��/��/�~/��/L1/L2 �{�^��: �T�u�E�B���h�E�̊g�嗦����


		SAMPLE12
			�e�N�X�`���̏풓�E��풓�̃T���v��
			�E�֘A�t�@�C��
				sample12.c
				data/restex.bin
			������{�^����
				SELECT �{�^��: �풓�E��풓�̐ؑ�

		
		SAMPLE13
			GS �T�[�r�X�𗘗p�����T�u�E�B���h�E �i�}���`�E�B���h�E) �̍쐬�T���v��
			�E�֘A�t�@�C��
				sample13.c 
				data/shuttle.bin 
				data/curtain.bin
			������{�^����
				 ��/��/��/�� (�����{�^��): �E���̃T�u�E�B���h�E�̈ړ�
				 ��/��/�~/��/R1/R2 �{�^��: �I�u�W�F�N�g#2 �̈ړ�
		
		
		SAMPLE14
			GS �T�[�r�X�𗘗p�����A�e�N�X�`���̈�ւ̕`��T���v��
			�E�֘A�t�@�C��
				sample14.c 
				data/APEWALK39.bin 
				data/curtain.bin
			������{�^����
				 ��/��/��/��(�����{�^��)/L1/L2: �e�N�X�`���I�u�W�F�N�g�̉�]
				 ��/��/�~/��/R1/R2 �{�^��: �e�N�X�`���I�u�W�F�N�g�̈ړ�
		
		SAMPLE15
			���[�V�����u���[�̃T���v��
			�E�֘A�t�@�C��
				sample15.c
				data/APEWALK39.bin
			������{�^����
				 L1/L2: �u���[�̋����̕ύX
				 ��/��/��/��(�����{�^��): �J�����̉�]
				 ��/��/�~/���{�^��: �J�����̈ړ�

		SAMPLE16
			�G���{�X�o���v�}�b�v�T���v��
			�E�֘A�t�@�C��
				sample16.c
				micro/vu1embossPoint.vsm
				data/emboss0.bin 
				data/emboss1.bin
			������{�^����
				ALPHA CTRL:
				   ��/��: �x�[�X�I�u�W�F�N�g ���l
				   �~/��: �o���v�I�u�W�F�N�g ���l
				BUMP CTRL:
				   ��/��: �G���{�X�V�t�g��

		SAMPLE17
			CLUT�o���v�}�b�v�T���v��
			�E�֘A�t�@�C��
				sample17.c
				data/clutbump0.bin 
				data/clutbump1.bin
			������{�^����
				��/��: ambient ���l
				�~/��: diffuse ���l
				R1/R2: specular ���l
				L1/L2: shininess �l
				CLUT �� = ambient �� + diffuse �� * (light.normal) + specular �� * (light.normal)^shininess
				 �� (light.normal) ���C�g�x�N�g���ƃm�[�}���x�N�g���Ƃ̓���

		SAMPLE18
			�V���h�[�}�b�v�T���v��
			�E�֘A�t�@�C��
				sample18.c
				micro/vu1shadowTex.vsm
				micro/vu1shadowSTQCull.vsm
				data/shadowmap0.bin 
				data/shadowmap1.bin
			������{�^����
				��/��/��/��/L1/L2: �I�u�W�F�N�g�̉�]
				��/��/�~/��/R1/R2: �I�u�W�F�N�g�̈ړ�
				��/��: �V���h�[���l

		SAMPLE19
			���C�g�}�b�v�T���v��
			�E�֘A�t�@�C��
				sample19.c
				micro/vu1lightmapSTQCull.vsm
				data/wall.bin 
				data/litmap.bin 
				data/spot.raw
			������{�^����
				Light Mode ����

		SAMPLE20
			���t���N�V�����E���t���N�V�����T���v��
			�E�֘A�t�@�C��
				sample20.c
				micro/vu1reflectS.vsm 
				micro/vu1refractS.vsm
				data/reftorus.bin 
				data/refback.raw
			������{�^����
				MICRO CHANGE:
				   ��/��: ���t���N�V�����E���t���N�V�����̐ؑ�
				PARAMETER CTRL: �i���t���N�V�����̂݁j
				   ��/��: ���ܗ�
				   �~/��: �g�嗦

		SAMPLE21
			���჌���Y���g�����_�C�i�~�b�N�ȃ��t���N�V�����E���t���N�V�����T���v��
			�E�֘A�t�@�C��
				sample21.c
				micro/vu1reflectR.vsm
				micro/vu1refractR.vsm
				micro/vu1fisheye.vsm
				data/reflect.bin
				data/reflect_fish.bin
				data/refract.bin
				data/refract_fish.bin
				data/refroom.bin
				data/refobj.bin
			������{�^����
				OBJ CTRL:
				   ��/��/��/��/L1/L2: �I�u�W�F�N�g�̉�]
				   ��/��/�~/��/R1/R2: �I�u�W�F�N�g�̈ړ�
				REFRACT CTRL:
				   ��/��: ���ܗ�
				   �~/��: �g�嗦
				   R1/R2: Z�V�t�g��
				REFLECT CTRL:
				   �~/��: �g�嗦
				   R1/R2: Z�V�t�g��
				Blending CTRL:
				   ��/��: ���t���N�V�������l
				   �~/��: ���t���N�V�������l

		SAMPLE22
			�X�y�L�����[���C�g�}�b�v�T���v��
			�E�֘A�t�@�C��
				sample22.c
				micro/vu1reflectS.vsm
				data/teapot.bin
			������{�^����
				PARAMETER CTRL:
				   �~/��: �g�嗦

		SAMPLE23
			�t���X�N���[���A���`�G�C���A�X�̃T���v��
			�E�֘A�t�@�C��
				sample23.c
				data/APEWALK39.bin
				data/dino.bin
			������{�^����
				   ��: �A���`�G�C���A�X�� ON/OFF
				   �~/���{�^��: �`��o�b�t�@�̐[���̕ύX
				   L1/L2: �u���[�̋����̕ύX
				   ��/��/��/��(�����{�^��): �J�����̉�]
				   R1/R2: �J�����̈ړ�
		SAMPLE24
                        ���჌���Y���g���ĕ`�悷��T���v��
                        �E �֘A�t�@�C��
                                sample24.c
                                micro/vu1cullVo.vsm     �ʏ�̓����ϊ��p�}�C�N���v���O����
                                micro/vu1fisheye.vsm    ���჌���Y�ϊ��p�}�C�N���v���O����
                                data/refroom.bin
                
                        ������{�^����
                                MICRO CHANGE:
                                   ��/�� �{�^��: ���჌���Y�ϊ��ƒʏ�̓����ϊ��̐ؑ�

                                PARAMETER CTRL: 
                                   ��/�� �{�^��: �`�悷��C���[�W�̑傫���ύX
                

		SAMPLE25
			��̃��f���ɕ����̃A�j���[�V���������ĕ\������T���v��
			�V�F�C�v�v���O�C���ق��K�v�ȃv���O�C���u���b�N�E�f�[�^�u���b
			�N�𑽏d�����Ďg�p����B
			�E�֘A�t�@�C��
				sample25.c 
				data/cubeman.bin
				data/animation.bin

		SAMPLE26
			Object��Clipping���s���AClip Plug�̃T���v��
			�J�����̃X�N���[����ʏ��(640, 448)�ł͂Ȃ�
			(320, 224)�Ƃ��Ĉ����A�X�N���[���g�Ƃ���GP
			�ŐԂ����C����\�����Ă��܂��B

			�܂��AVu1��Primitive�P�ʂł�Clipping��Clip�����
			���܂��ƃT���v���Ƃ��ĉ���Â炭�Ȃ�̂ŁA
			�}�C�N���R�[�h��Primitive Clip�Ȃ���basicVo��
			���Ă��܂��B
			�I�u�W�F�N�g���X�N���[���O�i�Ԙg�O�j�Ɋ��S��
			�o�����ꍇ�A�\�����ꂸ�A�܂�DMA�]��������Ȃ�����
			DMA���ׂ��ጸ���Ă���̂��m�F�ł��܂��B
			�E�֘A�t�@�C��
				sample26.c
				data/clip.bin
				
		SAMPLE27
			1/60fps �����x���t���[�����[�g�ł̕\���T���v��
			�x���t���[�����[�g��ł̕`��T���v���ł��B
			3�̃��[�h���f�����X�g���[�V�������܂��B

			INTERLACE, V448 �v���O���b�V�u�𑜓x�ŕ`�悵�A�C���^���[�X��
				�\�����܂��B�c�̉𑜓x�������܂���B


			NON-INTERLACE, V224 �m���C���^���[�X�ŕ\�����܂��B
				�c�̕\���𑜓x�������ɗ����܂����AVRAM �̏���͏��Ȃ���
				�ς݂܂��B
 			
			INTERLACE, V224  1/60fps �Ɠ����悤�ɕ`��E�\�����܂��B
				�\�����c�ɗh��Ă��܂��܂��B

			�E�֘A�t�@�C��
				sample27.c
				data/APEWALK39.bin
			������{�^����
				SELECT �\�����@�̕ύX

		SAMPLE28
			���W�I�V�e�B�[�T���v��
			�E�֘A�t�@�C��
				sample28.c
				data/radtea.bin
				micro/vu1noshade.vsm
				micro/vu1vertexCspec.vsm
				micro/vu1cullSo.vsm
			������{�^����
				MICRO CHANGE:
				   ��/�~:	�}�C�N���R�[�h�̐ؑ�

		SAMPLE29
			�N���[���I�u�W�F�N�g�T���v���B
			tree.bin�f�[�^�̃N���[�����쐬���Ă��܂��B
			�E�֘A�t�@�C��
				sample29.c
				data/tree.bin

		SAMPLE30
			VU1�ɂ��X�L���f�t�H���[�V�����T���v���ł��B
			�ő�4�{�[���A4�E�F�C�g�̃f�[�^�����������ł��܂��B
			�E�֘A�t�@�C��
				sample30.c
				micro/vu1skin.vsm
				data/skin1.bin
				data/under.tm2
				data/over.tm2
                        ������{�^����
                                SELECT: ���[�h�ؑ�
                                Bone Ctrl Mode:
                                   ��/��/��/��: �{�[���̉�]
                                   ��/��: ����{�[���ύX
				Root Ctrl Mode:
				   ��/��/�~/��/R1/R2 �{�^��:
				   �I�u�W�F�N�g�̉�]�E�ړ�

		SAMPLE31
			VU0�ɂ��X�L���f�t�H���[�V�����T���v���ł��B
			�����{�[���A4�E�F�C�g�̃f�[�^�����������ł��܂��B
                        �E�֘A�t�@�C��
                                sample31.c
                                mtxdisp.c
				data/skin0.bin
                        ������{�^����
                                SELECT: ���[�h�ؑ�
                                Bone Ctrl Mode:
                                   ��/��/��/��: �{�[���̉�]
                                   L1/L2/R1/R2: �{�[���̈ړ�
                                   ��/��: ����{�[���ύX

		SAMPLE32
			�����̃s�N�`���[�f�[�^������Tim2 Texture���g����
			Texture Change Animation������T���v��
			�E�֘A�t�@�C��
				sample32.c
				data/water.bin
				data/ani_w.tm2
			������{�^����
			    �Đ���
				SELECT : Animation�̒�~
				��/��  : Change Direction
				��/�~  : Skip Ratio
			    ��~��
				SELECT : Animation�̍ĊJ
				��/��  : Change Direction
				��/�~  : Skip Ratio
				��/��  : �R�}����

���f�[�^�̃��[�h�ɂ��ā�
	���̃T���v���v���O�����̓f�[�^���z�X�g���烍�[�h����`�����Ƃ���
	���܂��B���������ăz�X�g���� ��ŋL�����K�v�f�[�^���쐬���Ă�����
	���������Bsample���s�t�@�C�� main.elf�̂���f�B���N�g���̑��΃p�X
	�ŋL�q����Ă��܂��B���̃T���v���̏ꍇ�Amain.elf�̂���f�B���N�g��
	�ɂ���data�f�B���N�g���ȉ��Ƀf�[�^�o�C�i����u���Ă��������B

���f�[�^�ɂ��ā�
        �T���v���Ŏg�p���Ă��郂�f���f�[�^�́A�č� NewTek�Ђ�
        LightWave 3D�ɂ��z�z����Ă���f�[�^�����ɂ��āA
        ������Ѓf�B�E�X�g�[���ЊJ����eS Export for LightWave 3D��
        �G�N�X�|�[�g���܂����B

���t�^��
	�� ���[�U�[�̃T���v���ǉ��ɂ���
	���̃T���v���v���O�����Q�� �ȒP�ɃT���v���v���O������ǉ��ł���l
	�ɂȂ��Ă��܂��B
		sample_func.tbl
	�����ŋL�q����Ă���
		DEFFUNC( ... )
	�� ... �̕����� �e�T���v���̓o�^�ƂȂ��Ă��܂��B
	���Ƃ���
		DEFFUNC(USER_PROG)
	�Ɠo�^����ƁA�T���v���̃��C�����j���[�ɂ�
		> USER_PROG
	�Ƃ������ڂ��ǉ������͂��ł��B���ꂪ���{�^���őI��������
		USER_PROG_init()
	���P��Ăяo����A���̌㖈�t���[��
		USER_PROG_main()
	���Ăяo����܂��BUSER_PROG_main()�� �O��Ԃ����������
	USER_PROG_main()���Ăё����܂��B
	USER_PROG_main()����O�Ȓl��Ԃ��Ƃ��̎��̃t���[����
		USER_PROG_exit()
	���Ăяo����܂��B�����ŏI���������s�Ȃ��܂��B

	���Ȃ킿�A���[�U��MY_PROGRAM�Ƃ������̂̃v���O������ǉ����������

		1. sample_func.tbl��
			DEFFUNC(MY_PROGRAM,comment)
		   ��ǉ�

		2. �ȉ��̂��̂��܂ރt�@�C�����쐬
			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

		3. 2�ō쐬�����t�@�C����Makefile�̃\�[�X��`�����ɒǉ�����

		4. make����

	�� �Ǝ��̃v���O�������ȒP�ɒǉ��ł��܂��B

�����W�Ɋւ��钍�ӏ�����
�@�@�@�@LightWave 3D(TM)�́A�č�NewTek�Ђ̏��W�ł��B
