[SCE CONFIDENTIAL DOCUMENT]
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

��{�O���t�B�b�N�X���C�u���� libgp �̂�����{�I�Ȏg�����T���v��

< �T���v���̉�� >

	��{�O���t�B�b�N�X���C�u���� libgp �́A�����Ƃ���{�I�Ȏg����
	���Љ�铱���p�T���v���ł��B

< �t�@�C�� >

	main.c		���C���v���O�����\�[�X
			��ʂ̐ؑւƃN���A�A�R���g���[������
	oneprim.c	��̃X�v���C�g�p�P�b�g������`�悷��
	prims.c		�`�F�C�����g�p���āA�R�̃p�P�b�g��`�悷��
	defines.h	���ʃw�b�_�t�@�C��
	Makefile	���C�N�t�@�C��

< �N�����@ >

        % make run

< �R���g���[���̑�����@ >

	SELECT�{�^��		�`�惋�[�`���̐ؑ�
	��������(�����L�[)	(prims.c �̂Ƃ��̂�) ����Ώۂ̐؂�ւ�
	�����~���{�^��		�p�P�b�g�ʒu����
	
< �T���v���̐��� >

       oneprim.c	
	�@�@�X�v���C�g�p�P�b�g��������`�悵�܂��B
    	    ���̃T���v���͈�̃p�P�b�g��P�Ƃ�DMA�]�����ĕ`�悵�A
	�@�@�`�F�C���͎g�p���܂���B

    	    oneprim_init()  �X�v���C�g�p�P�b�g�̐����Ɠ��e�̐ݒ��
			�@�@�����Ȃ��܂��B
    	    oneprim_draw()  �p�P�b�g���e�̍X�V�ƁA���ۂ̕`�� (�p�P�b�g��
			    �P�Ɠ]��)�������Ȃ��܂��B
    	    oneprim_exit()  �p�P�b�g�Ŏg�p�����������̈��j�����܂��B


       prims.c	
    	    �����̃p�P�b�g�𑱂��ē]������ꍇ�́A���̃T���v���̂悤��
	    �`�F�C�����g�p���܂��B
    	    Z �l�̈قȂ�3�̃p�P�b�g����̃`�F�C���ɂȂ��ē]�����܂��B
    
    	    prims_init()  3�̃p�P�b�g�̐����Ɠ��e�̐ݒ�������Ȃ��܂��B
    			  �`�F�C���̐����Ə������������Ȃ��A�p�P�b�g��
    			  �`�F�C���ɓo�^���܂��B

    	    prims_draw()  �p�P�b�g���e�̍X�V�ƁA���ۂ̕`�� (�`�F�C����
	        	    �]��) �������Ȃ��܂��B

    	    prims_exit()  �p�P�b�g�ƃ`�F�C���Ŏg�p�����������̈��
			  �j�����܂��B


       main.c
    	    pad_init() �R���g���[�����͂̂��߂̏����������������Ȃ��܂��B
    	    pad_read() �R���g���[�����͂�ǂݍ��݂܂��B

    	    db_init() GS �̃_�u���o�b�t�@�����O (�`��/�\���p) �̏����ݒ��
		�@�@�@���܂��B
    	    db_swap() GS �̃_�u���o�b�t�@�����O (�`��/�\���p) ��؂�ւ��܂��B

    	    ���ۂ� gp �ɂ��`��� while ���[�v���� 
    		  mode_list[mode].draw_func(paddata, padtrig);
    	    ����Ăяo���Ă��܂��B

       defines.h
    	    �e�t�@�C���ŋ��ʂɎg�p����萔��C�����C���֐���錾���Ă��܂��B
    	    convert_x(), convert_y() ���_���W��GS �v���~�e�B�u���W�ł̒l��
	�@�@�ϊ����܂��B

    	����ɏڍׂ̓v���O�������̃R�����g�ƁA���C�u�����}�j���A���i�T�v
	����у��t�@�����X�j���Q�Ƃ��Ă��������B
    
    	�e�N�X�`�����̕`��ⓧ���ϊ�����@�ɂ��ẮA�ق��̃T���v�� 
	(zsort�Ȃ�) ���Q�Ƃ��Ă��������B

< ���ӎ��� >

	�ǂ݂₷���ɏd�_���������T���v���̂��߁A��������ōœK�Ȃ��̂ł�
	����܂���B���Ƃ��΁Aindex �擾�֐��� type �������ɂ��Ĕėp�� 
	index �֐����g�p���Ă��܂����A��������̓_����́Atype �ʂ�
	��p�֐����g�p���ׂ��ł��B
