[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                  Copyright (C) 2000 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

UNIX���TCP��p���āA
�N���C�A���g-�T�[�o�ԂŃp�P�b�g�𑗎�M����T���v��
(�N���C�A���g�v���O����)


<�T���v���̉��>
        ���̃v���O������ee����libnet��p���ăT�[�o��TCP�R�l�N�V�������m��
	���A�w��T�C�Y�̃f�[�^�𑗎�M����T���v���v���O������UNIX�łł��B
	linux��œ�����m�F���Ă���܂��B
	�f�[�^�T�C�Y�Ɠ����N���R�l�N�V�������������Ŏw��ł��܂��B
	�Ή�����v���O���������炩���߃T�[�o���ŋN������Ă���K�v��
	����܂��B
	�{�v���O������
	/usr/local/sce/ee/sample/inet/load_test/client�Ɠ���̓�������܂��B

<�t�@�C��>
        main.c        : ���C���v���O����


<�N�����@>

        $ make                                   : �R���p�C��
        $ ./load_test <saddr> <th_num> <pktsize> : load_test�̎��s�B
        <saddr>: �T�[�o�A�h���X
	<th_num>: �����Ɋm������R�l�N�V������
	<pktsize>: ����M����f�[�^�T�C�Y

<���l>
        inet API �� BSD socket API �̔�r���ł��܂��B
