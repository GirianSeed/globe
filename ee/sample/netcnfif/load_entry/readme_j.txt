[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	���ʃl�b�g���[�N�ݒ�C���^�t�F�[�X���C�u����
	�T���v���v���O����(load_entry)


< �T���v���̉�� >

	���̃v���O�����́A���ʃl�b�g���[�N�ݒ�C���^�t�F�[�X���C�u������
	���Ȃ��̃l�b�g���[�N�ݒ�t�@�C���̓��e�̎擾���s���T���v���v���O����
	�ł��B�܂��A�擾�������Ȃ��̃l�b�g���[�N�ݒ�t�@�C���̓��e���g�p���A
	�ڑ��e�X�g(Libnet ���C�u������p���Đڑ����A�A�h���X���擾������A
	�ؒf)���s���܂��B


< �t�@�C�� >

	Makefile	�F ���C�N�t�@�C��
	load_entry.c	�F ���C���v���O����
	load_module.c	�F ���[�h���W���[���v���O����
	load_module.h	�F ���[�h���W���[���v���O�����w�b�_


< �g�p���@ >

	1. /usr/local/sce/ee/sample/netcnfif/add_entry �����g�p����
	   �������[�J�[�h(PS2)�ɐ������ڑ��ł���ڑ��@��ݒ�A�ڑ��v���o�C�_
	   �ݒ���쐬���A�g�ݍ��킹1(Combination1)�ɓo�^����B
	   ���̃������[�J�[�h(PS2)��������1�ɑ}�����܂��B

	   ���{�T���v���v���O�����ł͐ڑ��@��Ƃ���
	     �uSCE/Ethernet (Network Adaptor)�v���g�p���邱�Ƃ�O��Ƃ���
	     IOP ���W���[���̃��[�h���s���Ă��܂��B���̑��̓��e�Őڑ���
	     �s���ꍇ�A�K�؂� IOP ���W���[�������[�h����悤�ɕύX����
	     �K�v������܂��B

	   ���e "PlayStation 2" ���ɕ��������ꂽ���Ȃ��̃l�b�g���[�N
	     �ݒ�t�@�C�����g�p����ꍇ�́A
	     sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);
	     ���R�����g�A�E�g���ĉ������B

	2. /usr/local/sce/ee/sample/netcnfif/load_entry �� make ����

	   > cd /usr/local/sce/ee/sample/netcnfif/load_entry
	   > make

	3. dsedb ���N������

	   > dsedb

	4. �ȉ��̂悤�Ɏ��s����

	   dsedb S> reset;run ./load_entry.elf

	5. /usr/local/sce/ee/sample/netcnfif/load_entry/load_entry.c ����
	   #define DEBUG ����`����Ă���Ȃ�΂��Ȃ��̃l�b�g���[�N�ݒ�
	   �t�@�C���̓��e���ȉ��̂悤�ɏo�͂��܂��B

	   -----------------------
	   attach_ifc       : "ifc000.cnf"
	   attach_dev       : "dev000.cnf"
	   dhcp_host_name   : "(null)"
	   address          : "192.168.0.1"
	   netmask          : "255.255.255.0"
	   gateway          : "192.168.0.2"
	   dns1_address     : "192.168.0.3"
	   dns2_address     : "192.168.0.4"
	   phone_numbers1   : "(null)"
	   phone_numbers2   : "(null)"
	   phone_numbers3   : "(null)"
	   auth_name        : "(null)"
	   auth_key         : "(null)"
	   peer_name        : "(null)"
	   vendor           : "SCE"
	   product          : "Ethernet (Network Adaptor)"
	   chat_additional  : "(null)"
	   outside_number   : "(null)"
	   outside_delay    : "(null)"
	   ifc_type         : "-1"
	   mtu              : "-1"
	   ifc_idle_timeout : "-1"
	   dev_type         : "3"
	   phy_config       : "1"
	   dialing_type     : "-1"
	   dev_idle_timeout : "-1"
	   dhcp             : "0"
	   dns1_nego        : "255"
	   dns2_nego        : "255"
	   f_auth           : "0"
	   auth             : "0"
	   pppoe            : "255"
	   prc_nego         : "255"
	   acc_nego         : "255"
	   accm_nego        : "255"
	   -----------------------
	   address: "192.168.0.1"
	   [load_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 
