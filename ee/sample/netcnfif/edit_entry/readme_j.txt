[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
                       Copyright (C) 2002 Sony Computer Entertainment Inc.
                                                       All Rights Reserved


	���ʃl�b�g���[�N�ݒ�C���^�t�F�[�X���C�u����
	�T���v���v���O����(edit_entry)


< �T���v���̉�� >

	���̃v���O�����́A���ʃl�b�g���[�N�ݒ�C���^�t�F�[�X���C�u������
	���Ȃ��̃l�b�g���[�N�ݒ�t�@�C���̕ҏW���s���T���v���v���O����
	�ł��B


< �t�@�C�� >

	Makefile		�F ���C�N�t�@�C��
	edit_entry.c		�F ���C���v���O����
	load_module.c		�F ���[�h���W���[���v���O����
	load_module.h		�F ���[�h���W���[���v���O�����w�b�_
	set_configuration.c	�F �ݒ�v���O����
	set_configuration.h	�F �ݒ�v���O�����w�b�_


< �g�p���@ >

	1. /usr/local/sce/ee/sample/netcnfif/add_entry/readme_j.txt ��
	   �]���āAadd_entry �T���v���v���O���������s����B

	2. �ڑ��@��ݒ�̏��̓v���O�������擾����̂ŁA
	   �g�p����ڑ��@��݂̂�ڑ�����B

	   ���{�T���v���v���O�����ł͐ڑ��@��Ƃ���
	     an986.irx ���h���C�o�Ƃ���ڑ��@����g�p���邱�Ƃ�O��Ƃ���
	     IOP ���W���[���̃��[�h���s���Ă��܂��B���̑��̓��e�Őڑ����s��
	     �ꍇ�A�K�؂� IOP ���W���[�������[�h����悤�ɕύX����K�v��
	     ����܂��B

	   ��an986.irx �� /usr/local/sce/iop/modules �ɂ�����̂Ƃ��܂��B

	3. /usr/local/sce/ee/sample/netcnfif/edit_entry �� make ����

	   > cd /usr/local/sce/ee/sample/netcnfif/edit_entry
	   > make

	4. dsedb ���N������

	   > dsedb

	5. �ȉ��̂悤�Ɏ��s����

	   dsedb S> reset;run ./edit_entry.elf

	6. �ȉ��̂悤�ɕ\�����ꂽ��ҏW�����ł��B

	   [edit_entry.c] complete
	   # TLB spad=0 kernel=1:12 default=13:36 extended=37:47
	   *** Unexpected reply - type=BREAKR result=PROGEND
	   dsedb S> 

< �m�F���@ >

	�ݒ���e���m�F����ɂ͗Ⴆ�Έȉ��̂悤�ȕ��@������܂��B

	1. /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c ����
	   sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on); ��
	   ���s����Ă��邱�Ƃ��m�F����B
	   (���ꂪ���s����Ă��Ȃ��ꍇ�A���Ȃ��̃l�b�g���[�N�ݒ�t�@�C����
	   ����������邽�ߓ��e�̊m�F���s���܂���)

	2. /usr/local/sce/ee/sample/hdd/shell �� make ����

	   > cd /usr/local/sce/ee/sample/hdd/shell
	   > make

	3. dsecons ���N������

	   > dsecons

	4. dsedb ���N������

	   > dsedb

	5. �ȉ��̂悤�Ɏ��s����

	   dsedb S> reset;run ./main.elf

	6. dsecons ��ʂł��Ȃ��̃l�b�g���[�N�ݒ�t�@�C����ۑ�����
	   �f�o�C�X�Ɉړ����A���e���m�F����

	   [host1:]/ $ mc0:
	   [mc0:]/ $ ls
	   d------rwx 0     0     0x0027     0         8 Jan 24  5:36 .
	   d------rw- 0     0     0x0026     0         0 Nov 30 16:14 ..
	   d------rwx 0     0     0x0027     0         9 Jan 24  6:32 BWNETCNF
	   [mc0:]/ $ cd BWNETCNF
	   [mc0:]/BWNETCNF $ ls
	   d------rwx 0     0     0x0027     0         0 Jan 24  5:36 .
	   d------rwx 0     0     0x0027     0         0 Nov 30 16:14 ..
	   -------rwx 0     0     0x0017     0     33688 Jan 24  5:36 SYS_NET.ICO
	   -------rwx 0     0     0x0017     0       964 Jan 24  5:36 icon.sys
	   -------rwx 0     0     0x0017     0       115 Jan 24  6:32 dev000.cnf
	   -------rwx 0     0     0x0017     0       108 Jan 24  6:32 BWNETCNF
	   -------rwx 0     0     0x0017     0       133 Jan 24  6:32 ifc000.cnf
	   -------rwx 0     0     0x0017     0       100 Jan 24  5:36 net000.cnf
	   [mc0:]/BWNETCNF $ 

	7. ��L�̃f�B���N�g���y�уt�@�C�������݂��Ă��邱�Ƃ��m�F����B

	8. �ݒ�Ǘ��t�@�C��(BWNETCNF)�̓��e���m�F����B
	   ���e�� /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c ����
	   �ݒ肳���ȉ��̒�`�ɂ���ĕς��B

	   #define IFC_NEW_USR_NAME "Ethernet Setting"

	   [mc0:]/BWNETCNF $ cat BWNETCNF
	   1,1,ifc000.cnf,Ethernet Setting
	   2,1,dev000.cnf,ethernet-vendor/ethernet-product
	   0,1,net000.cnf,Combination1

	   [mc0:]/BWNETCNF $ 

	9. �ڑ��@��ݒ�(dev000.cnf)�Ɛڑ��v���o�C�_�ݒ�(ifc000.cnf)�̓��e��
	   �m�F����B
	   ���e�� /usr/local/sce/ee/sample/netcnfif/edit_entry/edit_entry.c ����
	   �ݒ肳���ȉ��̒�`�ɂ���ĕς��B

	   #define DEV_PATTERN (0)
	   #define IFC_PATTERN (0)

	   - dev000.cnf �ɂ���

	     < #define DEV_PATTERN (0) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[���g�p����ݒ�) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     vendor "SCE"
	     product "Ethernet (Network Adaptor)"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < #define DEV_PATTERN (1) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[�ȊO�̃C�[�T�l�b�g�A�_
	       �v�^���g�p����ݒ�) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     vendor "ethernet-vendor"
	     product "ethernet-product"
	     phy_config auto

	     [mc0:]/BWNETCNF $ 

	     < #define DEV_PATTERN (2) �̏ꍇ:
	       (�ڑ��@��Ƃ��ă��f���E TA ���g�p����ݒ�) >

	     [mc0:]/BWNETCNF $ cat dev000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     vendor "modem-vendor"
	     product "modem-product"
	     chat_additional "\"\" ATM0 OK \\c"
	     outside_number "0"
	     outside_delay ",,,"
	     dialing_type tone
	     idle_timeout 600

	     [mc0:]/BWNETCNF $ 

	   - ifc000.cnf �ɂ���

	     < #define IFC_PATTERN (0) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[���g�p���ADHCP ���g�p
		���Ȃ��ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (1) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[���g�p���ADHCP ���g�p����
		�ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type nic
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (2) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[�ȊO�̃C�[�T�l�b�g�A�_
	       �v�^���g�p���ADHCP ���g�p���Ȃ��ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     -dhcp
	     address "192.168.0.1"
	     netmask "255.255.255.0"
	     route add -net 0.0.0.0 gw 192.168.0.2 netmask 0.0.0.0
	     nameserver add 192.168.0.3
	     nameserver add 192.168.0.4

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (3) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăl�b�g���[�N�A�_�v�^�[�ȊO�̃C�[�T�l�b�g�A�_
	       �v�^���g�p���ADHCP ���g�p����ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type eth
	     dhcp
	     dhcp_host_name "host_name"
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (4) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăC�[�T�l�b�g�A�_�v�^���g�p���APPPoE���g�p���A
	       DNS�T�[�o�A�h���X�������擾����ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (5) �̏ꍇ:
	       (�ڑ��@��Ƃ��ăC�[�T�l�b�g�A�_�v�^���g�p���APPPoE���g�p���A
	       DNS�T�[�o�A�h���X�������擾���Ȃ��ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     idle_timeout 0
	     -want.accm_nego
	     -want.prc_nego
	     -want.acc_nego
	     allow.auth chap/pap
	     pppoe
	     mtu 1454
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (6) �̏ꍇ:
	       (�ڑ��@��Ƃ��ă��f���E TA ���g�p���ADNS �T�[�o�A�h���X��
	       �����擾����ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     want.dns1_nego
	     want.dns2_nego
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0

	     [mc0:]/BWNETCNF $ 

	     < #define IFC_PATTERN (7) �̏ꍇ:
	       (�ڑ��@��Ƃ��ă��f���E TA ���g�p���ADNS �T�[�o�A�h���X��
	       �����擾���Ȃ��ݒ�) >

	     [mc0:]/BWNETCNF $ cat ifc000.cnf
	     # <Sony Computer Entertainment Inc.>

	     type ppp
	     -dhcp
	     auth_name "userid"
	     auth_key "password"
	     peer_name "*"
	     allow.auth chap/pap
	     phone_number0 "11-1111-1111"
	     phone_number1 "22-2222-2222"
	     phone_number2 "33-3333-3333"
	     route add -net 0.0.0.0 netmask 0.0.0.0
	     nameserver add 192.168.0.1
	     nameserver add 192.168.0.2

	     [mc0:]/BWNETCNF $ 
