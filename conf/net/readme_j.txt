[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.5
                Copyright (C) 2001 Sony Computer Entertainment Inc.
                                                   All Rights Reserved

�u�l�b�g���[�N�ݒ�t�@�C���T���v���ɂ��āv

/usr/local/sce/conf/net �ɂ͈ȉ��̃l�b�g���[�N�ݒ�t�@�C���T���v����
�܂܂�Ă��܂��B

net000.cnf - ifc000.cnf �� dev000.cnf �̑g�ݍ��킹
net001.cnf - ifc001.cnf �� dev001.cnf �̑g�ݍ��킹
net002.cnf - ifc002.cnf �� dev002.cnf �̑g�ݍ��킹
net003.cnf - ifc003.cnf �� dev003.cnf �̑g�ݍ��킹
net004.cnf - ifc004.cnf �� dev003.cnf �̑g�ݍ��킹
net005.cnf - ifc005.cnf �� dev002.cnf �̑g�ݍ��킹
net006.cnf - ifc004.cnf �� dev002.cnf �̑g�ݍ��킹

ifc000.cnf - type eth �T���v��
ifc001.cnf - type ppp �T���v��
ifc002.cnf - type nic �T���v��
ifc003.cnf - type eth �T���v��(use dhcp)
ifc004.cnf - pppoe �T���v��
ifc005.cnf - type nic �T���v��(use dhcp)

dev000.cnf - type eth �T���v��
dev001.cnf - type ppp �T���v��
dev002.cnf - type nic �T���v��
dev003.cnf - type eth �T���v��

�� ???000.cnf �` ???001.cnf �ɂ��Ă� dev �t�@�C������ vendor, product
   ��K�؂Ȗ��̂ɕύX���Aifc �t�@�C������ IP Address, User ID, Password ��
   ��K�؂Ȓl���ɕύX���邱�Ƃɂ��g�p�\�ƂȂ�܂��B

�� ???002.cnf �ɂ��Ă� ifc �t�@�C������ IP Address ����K�؂Ȓl�ɕύX
   ���邱�Ƃɂ��g�p�\�ƂȂ�܂��B

�� ???003.cnf,???005.cnf �ɂ��Ă̓T���v�����ł��̂܂܎g�p�\�ł��B

�� ???004.cnf,???006 �ɂ��Ă� ifc �t�@�C������ User ID, Password ����K�؂�
   �l���ɕύX���邱�Ƃɂ��g�p�\�ƂȂ�܂��B

<net.db�ɂ���>
�ݒ�Ǘ��t�@�C�� net.db ���̍\���� sceNetCnfList �\���̂Ŏ擾�ł���
�f�[�^�� ',' �ŕ��ׂ����e�ł��B
(������ type, stat �� ASCII <-> �o�C�i���l �̕ϊ�����݂��܂�)
sceNetCnfList �\���̂ɂ��Ă� IOP���C�u���胊�t�@�����X ->
���ʃl�b�g���[�N�ݒ胉�C�u����(ntcnf_rf) ���Q�Ɖ������B
�܂����ۂ̐ݒ�Ǘ��t�@�C���̎g�����i�G���g���ǉ��Ȃǁj�ɂ��܂��Ă�
sce/iop/sample/inet/setapp ���Q�l�ɂ��Ă��������B

