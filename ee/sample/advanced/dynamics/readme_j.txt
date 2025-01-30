[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                      All Rights Reserved 

���̃_�C�i�~�N�X�T���v��

���T���v���̉����

	�{�v���O�����ł́A�������̍��̃_�C�i�~�N�X�V�~�����[�V�������s���܂��B
	�I�u�W�F�N�g���m�̏Փ˂����o���邽�߂ɁAadvanced/collision�T���v��
	�̏Փˌ��o�@�\���g���܂��B

	�g�p�����A���S���Y���Ƃ��̐��������̏ڍׂɊւ��ẮA
	overview_j.txt���Q�Ƃ��Ă��������B


���t�@�C����

        main.c          �F�T���v���v���O�����̃V�X�e���\�[�X�R�[�h
	sample*.c       �F�T���v���v���O�����̃\�[�X�R�[�h
	dynamics.c      �F���̃V�~�����[�V�����֐�
	collision.c     �F�Փˌ��o�֐�
	vector.h        �Fcollision.c�Ŏg����x�N�g��/�}�g���N�X
			  ���Z�֐����܂ރC�����C���֐�
        util.c          �F���[�e�B���e�B�v���O�����̃\�[�X�R�[�h
        camera.c       	�F�J�����ݒ�̂��߂̃v���O�����\�[�X
        light.c         �F�����ݒ�֘A�v���O�����\�[�X
        sample_func.tbl �F�o�^�ς݃T���v���v���O�����֐����܂�
			�@�t�@�C��
        vu0Coll.vcl     �F�Փ˃A���S���Y���Ŏg����Vu0�}�C�N��
			�@�֘AVCL (VU �R�}���h���C��)�\�[�X
        vu0Coll.vsm     �FVSM�\�[�X            
       	data/*.bin      �F�o�C�i���f�[�^�t�@�C��
	data/*.s        �FASCII�f�[�^�t�@�C��


	���f�[�^�t�@�C����
		data/block.bin          �F �`�惂�f��
		data/ellipsoid.bin      �F �`�惂�f��
		data/dpyramid.bin       �F �`�惂�f��
		data/plane1.bin         �F �`�惂�f��
		data/plane2.bin         �F �`�惂�f��
                data/plane3.bin         �F �`�惂�f��
                data/cube.bin           �F �`�惂�f��
                data/ball.bin           �F �`�惂�f��


(gjkobj-file�t�H�[�}�b�g�ɂ��ẮA advanced/collision/overview_j.txt��
�Q�Ƃ��Ă�������)
                data/cblock.bin         �F �Փˌ��o�pgjkobj-file 
                data/cellipsoid.bin     �F �Փˌ��o�pgjkobj-file
		data/cdpyramid.bin      �F �Փˌ��o�pgjkobj-file
		data/cplane1.bin        �F �Փˌ��o�pgjkobj-file
		data/cplane2.bin        �F �Փˌ��o�pgjkobj-file
		data/cplane3.bin        �F �Փˌ��o�pgjkobj-file
                data/ccube.bin          �F �Փˌ��o�pgjkobj-file
                data/cball.bin          �F �Փˌ��o�pgjkobj-file


���N�����@��

        % make run


���R���g���[���̑�����@��

	�v���O�������N������ƃT���v�����j���[���\������܂��B
	���̃��j���[�Ńv���O�������J�n���܂��B

	�����j���[�̑�����@��
	START�{�^���������ƃT���v����ʂ��烁�j���[��ʂɖ߂邱�Ƃ�
	�ł��܂��B

	���T���v�����[�h�̑�����@��
	 START�{�^���F���j���[�ɖ߂�܂�

	�{�T���v���͗l�X�ȑ��샂�[�h�ŗ��p���邱�Ƃ��ł��܂��B
	SELECT�{�^���������ă��[�h��ύX���܂��B����ƁA����
	�I������Ă��郂�[�h�Ǝg�p���@����ʂɕ\������܂��B
 
         �ȉ��̃��[�h��I�����邱�Ƃ��ł��܂��B
	��Camera Move Mode��   	�J�����p�����[�^�̐ݒ�
	��Light Ctrl Mode��     ���C�g�p�����[�^�̐ݒ�i�w�������C�g�j
	��Force Mode��          �I�������I�u�W�F�N�g�ɗ͂�������
	��Select Obj +Type Mode�� 	�I�u�W�F�N�g��I�����AACTIVE
					�܂���PASSIVE�ɐݒ肷��
			    ACTIVE...�I�u�W�F�N�g���S�Ă�
				�V�~�����[�V�����v���Z�X�ɎQ�����Ă���B 
                            PASSIVE...�I�u�W�F�N�g�ɏd�͂��������A
				�����县�ʂ������Ă��镨�Ƃ��Ĉ�����B


�����l��

	overview_j.txt���Q�Ƃ��Ă��������B
	�Փ˃A���S���Y���̉���ɂ��Ă�advanced/collision/*.txt���Q��
	���Ă��������B
	CPU��DMA��usage�ɂ́A�f�o�O���b�Z�[�W�̎g�p�����܂܂�Ă��܂��B
	
  
