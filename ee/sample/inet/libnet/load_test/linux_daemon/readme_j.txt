[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

UNIX���TCP��p���āA
�N���C�A���g-�T�[�o�ԂŃp�P�b�g�𑗎�M����T���v��
(�T�[�o�v���O����)


<�T���v���̉��>
        ���̃v���O������TCP��passive open���s�Ȃ��A
	�ڑ����Ă����N���C�A���g�ƃf�[�^�𑗎�M����T���v���v���O������
	UNIX�łł��Blinux��œ���m�F�����Ă��܂��B
	/usr/local/sce/ee/sample/inet/load_test/daemon�Ɠ���̓�������܂��B

<�t�@�C��>
        main.c        : ���C���v���O����


<�N�����@>

        $ make		: �R���p�C��
        $ ./load_testd  : �T�[�o�v���O�����̋N��

�T�[�o���N��������A�N���C�A���g�v���O��������R�l�N�V�������m����
�ʐM���J�n���܂��B
�N���C�A���g�v���O������ee��Aiop��œ��삷��v���O�������p�ӂ���Ă��܂��B
���ꂼ��A
/usr/local/sce/ee/sample/inet/load_test/client/readme_j.txt
/usr/local/sce/iop/sample/inet/load_test/client/readme_j.txt
���Q�Ƃ��ĉ������B

<���l>
        inet API��BSD socket API�̔�r��m�������ނ��߂�
	���p���ĉ������B
