[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                     All Rights Reserved  

�Փˌ��o�T���v��


���T���v���̉����

�{�v���O�����́A�I�u�W�F�N�g�̏Փ˂����o���邽�߂�GJK�A���S���Y�����g�p
���Ă��܂��B�܂��A�����_�����O�ɂ�HiG/HiP���C�u�������g�p���Ă��܂��B


���t�@�C����	
        main.c          	�T���v���v���O�����V�X�e���\�[�X�R�[�h
        sample.c        	�T���v���v���O�����̃\�[�X�R�[�h
	collision.c     	�Փˌ��o�̂��߂̊֐�
	vector.h        	collision.c�Ŏg����x�N�g��/�}�g���N�X
				���Z�֐����܂ރC�����C���֐�
        util.c          	���[�e�B���e�B�v���O�����̃\�[�X�R�[�h
        camera.c       		�J�����ݒ�̂��߂̃v���O�����\�[�X
        light.c         	�����ݒ�̂��߂̃v���O�����\�[�X
        sample_func.tbl 	�o�^�ς݃T���v���v���O�����֐����܂�
				�t�@�C��
        vu0Coll.vcl     	�Փ˃A���S���Y���Ŏg����Vu0�}�C�N��
				�̂��߂�VCL (VU �R�}���h���C��)�\�[�X
        vu0Coll.vsm     	VSM�\�[�X            
       	data/*.bin      	�o�C�i���f�[�^�t�@�C��
	data/*.s        	ASCII�f�[�^�t�@�C��

	���f�[�^�t�@�C����

		data/block.bin          �����_�����O���f��
		data/ellipsoid.bin      �����_�����O���f��
              	data/cblock.bin         �Փˌ��o�̂��߂�gjkobj-file
              	data/cellipsoid.bin     �Փˌ��o�̂��߂�gjkobj-file
 

���N�����@ ��

        % make run


�� �R���g���[���̑�����@ ��

	�v���O�������N������ƃT���v�����j���[���\������܂��B
	���̃��j���[�Ńv���O�������J�n���܂��B	
	
	�����j���[�̑�����@��
	START�{�^���������ƃT���v����ʂ��烁�j���[��ʂɖ߂邱�Ƃ�
	�ł��܂��B

	���T���v�����[�h�̑�����@��
	 START�{�^���F���j���[�ɖ߂�܂�

	�{�T���v���͗l�X�ȑ��샂�[�h�ŗ��p���邱�Ƃ��ł��܂��B
	SELECT�{�^���������ă��[�h��I�����ĕύX���܂��B����ƁA����
	�I������Ă��郂�[�h�Ǝg�p���@����ʂɕ\������܂��B

        �ȉ��̃��[�h��I�����邱�Ƃ��ł��܂��B
	<Camera Move Mode>   	�J�����p�����[�^�̐ݒ�
	<Light Ctrl Mode>      	�Ɩ��p�����[�^�̐ݒ�i�w�����Ɩ��j
	<Obj Ctrl Mode>        	�I�u�W�F�N�g0�̈ړ��E��]
	<Coll Ctrl Mode>        �Փ˃��[�h�̑I���F
	COLLISION CHECK : 
		�I�u�W�F�N�g���������Ă��邩�ǂ������`�F�b�N����
		�i�֐�is_collision()���g�p�j
       	CALC. DISTANCE : 
		�I�u�W�F�N�g�Ԃ̍ŒZ�������v�Z����icalc_dist()���g�p�j
	FOLLOW SHORTEST DISTANCE : 
		�I�u�W�F�N�g�Ԃ̋�����0.01�ȉ��ɂȂ�܂ŃI�u�W�F�N�g1��
		�ŒZ�����Ɉړ�������icalc_dist()���g�p�j

			
���Փˌ��o�̊֐��̎g�p�ɂ��ā�

  alloc_gjkobj_neighbors()   	�Փ˃I�u�W�F�N�g�̗אړ_���m�ۂ���

  init_collision()              Vu0�}�C�N���R�[�h��Vu0�v���O����������
				�Ƀ��[�h����

  gjk_init()                    �T���v���Ŏg����GJKObj��Simplex�\����
				������������

  is_collision()                �Փ˂̔������m�F����

  calc_dist()                   �I�u�W�F�N�g�Ԃ̍ŒZ�������v�Z���� 
				      
  free_gjkobj_neighbors()       �אړ_�Ɏg��ꂽ�������[���������   


�����ӎ�����

	�{�T���v���Ŏg�p����A���S���Y���̏ڍׂɂ��Ă�
	overview_j.txt���Q�Ƃ��Ă��������B

	�\�������CPU��DMA�̎g�p�ʂɂ́A�f�o�O���b�Z�[�W��I�u�W�F�N�g�Ԃ�
	�ŒZ�������������߂̃f�o�O�{�b�N�X�\�����Ԃ��܂܂�Ă��܂��B


���f�[�^�̃��[�h��

	�{�v���O�����ł́A�z�X�g����f�[�^�����[�h���邱�Ƃ��ł��܂��B
	
	�f�[�^�����[�h����O�ɏ�ŏq�ׂ��K�v�ȃf�[�^���z�X�g���ɍ쐬����
	�����Ă��������B�f�[�^�p�X�ɂ��ẮAsample���s�t�@�C��main.elf
	�̂���f�B���N�g���̑��΃p�X�Ő�������Ă��܂��B
	
	�{�T���v���ł́A�o�C�i���f�[�^��main.elf�̂���f�B���N�g����
	data�f�B���N�g���ȉ��ɓ���Ă��������B


���t�^ ��

	* ���[�U�̃T���v���ǉ��ɂ���
	�{�T���v���v���O�����Q�͈ȉ��̃T���v���v���O�����ɊȒP�ɒǉ�
	���邱�Ƃ��ł��܂��B
	
	DEFFUNC( ... ) ��sample_func.tbl�ŉ������Ă��܂��B

	���ʓ��ɒǉ��������T���v��������͂��܂��B	

	���Ƃ��΁ADEFFUNC(USER_PROG)����͂���ƁA�ȉ��̍��ڂ��v���O����
	�̃��C�����j���[�ɒǉ�����܂��B	

			�� USER_PROG
	
	��������{�^���őI�����܂��B����ƁA�ȉ��̊֐�����x�Ăяo����
	�܂��B
			
			USER_PROG_init()
	
	�����āA�ȉ��̊֐����e�t���[���ɌĂяo����܂��B

			USER_PROG_main()
	
	USER_PROG_main()��0��Ԃ����������AUSER_PROG_main()���Ăяo
	����܂��B
	
	USER_PROG_main()����0�̒l��Ԃ����ꍇ�AUSER_PROG_exit()������
	�t���[���ɌĂяo����A�I�����������s����܂��B
	

	MY_PROGRAM�Ƃ������O�œƎ��̃v���O������ǉ��������ꍇ�A�ȉ���
	�菇�ŊȒP�Ɏ��s���邱�Ƃ��ł��܂��B

	1. �܂��Asample_func.tbl��DEFFUNC(MY_PROGRAM,comment)��ǉ����܂��B

	2. ���ɁA�ȉ��̂��̂��܂񂾃t�@�C�����쐬���܂��B

			void MY_PROGRAM_init()
			int MY_PROGRAM_main()
			void MY_PROGRAM_exit()

	3. 2�ō쐬�����t�@�C����make file�̃\�[�X��`�����ɒǉ����܂��B

	4. make file�����s���܂��B
		

