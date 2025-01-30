/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         if_id.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.02.05      tetsu       Change if_id_stat
 *                                                  Change search_if_id()
 *                                                  Add search_eth_if_id()
 *                                                  Add get_if_id_flag()
 *                                                  Change add_if_id()
 */

#include <string.h>
#include <kernel.h>
#include <inet/netdev.h>

#include "if_id.h"

typedef struct if_id_stat{
    int id;                    /* �C���^�t�F�[�X ID */
    int stat;                  /* �C���^�t�F�[�X ID �̏�� */
    int flag;                  /* �f�o�C�X���C���̎�ʂ�\���t���O */
    unsigned char bus_type;    /* �o�X��� */
    unsigned char bus_loc[31]; /* �f�o�C�X�ʒu��� */
    char vendor[256];          /* �x���_�� */
    char product[256];         /* �v���_�N�g�� */
} if_id_stat_t;

static if_id_stat_t *if_id; /* �C���^�t�F�[�X��Ԃ�ێ�����o�b�t�@�ւ̃|�C���^ */
static int if_id_num = 0;   /* �ێ����Ă���C���^�t�F�[�X��Ԃ̐� */

/* �C���^�t�F�[�X ID ���������� */
int search_if_id(int id)
{
    int i;

    /* �C���^�t�F�[�X ID �����݂���ꍇ�̓C���f�b�N�X��Ԃ� */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (i);
    }

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    return (-1);
}

/* PPPoE �p ppp I/F �ƁA�����Ɏg�p���Ă��� eth I/F �� id ���������� */
int search_eth_if_id(int id)
{
    int i, index;

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    if((index = search_if_id(id)) < 0) return (-1);

    for(i = 0; i < if_id_num; i++){
	if(if_id[index].bus_type == sceInetBus_USB){
	    if(memcmp(if_id[index].bus_loc, if_id[i].bus_loc, sizeof(if_id[index].bus_loc)) == 0 &&
	       memcmp(if_id[index].vendor, if_id[i].vendor, sizeof(if_id[index].vendor)) == 0 &&
	       memcmp(if_id[index].product, if_id[i].product, sizeof(if_id[index].product)) == 0 &&
	       if_id[i].flag == ARP) return (if_id[i].id);
	}else{
	    if(memcmp(if_id[index].vendor, if_id[i].vendor, sizeof(if_id[index].vendor)) == 0 &&
	       memcmp(if_id[index].product, if_id[i].product, sizeof(if_id[index].product)) == 0 &&
	       if_id[i].flag == ARP) return (if_id[i].id);
	}
    }

    /* �Y������ eth I/F �� id �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    return (-1);
}

/* �C���^�t�F�[�X ID �̏�Ԃ��擾���� */
int get_if_id_stat(int id)
{
    int i;

    /* �C���^�t�F�[�X ID �����݂���ꍇ�̓C���f�b�N�X��Ԃ� */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (if_id[i].stat);
    }

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    return (-1);
}

/* �f�o�C�X���C���̎�ʂ�\���t���O���擾���� */
int get_if_id_flag(int id)
{
    int i;

    /* �C���^�t�F�[�X ID �����݂���ꍇ�̓C���f�b�N�X��Ԃ� */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (if_id[i].flag);
    }

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    return (-1);
}

/* �C���^�t�F�[�X ID ��ǉ����� */
int add_if_id(int id, int stat, int flag, unsigned char bus_type, unsigned char *bus_loc, char *vendor, char *product)
{
    static int oldstat;
    if_id_stat_t *new_if_id;

    /* �C���^�t�F�[�X ID �����݂���ꍇ�� -1 ��Ԃ� */
    if(search_if_id(id) >= 0) return (-1);

    CpuSuspendIntr(&oldstat);
    if(if_id_num){
	/* �C���^�t�F�[�X ID �����ЂƂ��₷ */
	if_id_num++;

	/* �V�����̈���m�ہA�R�s�[ */
	new_if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);
	memcpy((void *)new_if_id, (void *)if_id, sizeof(if_id_stat_t) * (if_id_num - 1));
	FreeSysMemory(if_id);

	/* �C���^�t�F�[�X ID ��ǉ� */
	new_if_id[if_id_num - 1].id = id;
	new_if_id[if_id_num - 1].stat = stat;
	new_if_id[if_id_num - 1].flag = flag;
	new_if_id[if_id_num - 1].bus_type = bus_type;
	memcpy(new_if_id[if_id_num - 1].bus_loc, bus_loc, sizeof(new_if_id[if_id_num - 1].bus_loc));
	memcpy(new_if_id[if_id_num - 1].vendor, vendor, sizeof(new_if_id[if_id_num - 1].vendor));
	memcpy(new_if_id[if_id_num - 1].product, product, sizeof(new_if_id[if_id_num - 1].product));

	/* �|�C���^���X�V */
	if_id = new_if_id;
    }else{
	/* �C���^�t�F�[�X ID �����ЂƂ��₷ */
	if_id_num++;

	/* �V�����̈���m�� */
	if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);

	/* �C���^�t�F�[�X ID ��ǉ� */
	if_id[if_id_num - 1].id   = id;
	if_id[if_id_num - 1].stat = stat;
	if_id[if_id_num - 1].flag = flag;
	if_id[if_id_num - 1].bus_type = bus_type;
	memcpy(if_id[if_id_num - 1].bus_loc, bus_loc, sizeof(if_id[if_id_num - 1].bus_loc));
	memcpy(if_id[if_id_num - 1].vendor, vendor, sizeof(if_id[if_id_num - 1].vendor));
	memcpy(if_id[if_id_num - 1].product, product, sizeof(if_id[if_id_num - 1].product));
    }
    CpuResumeIntr(oldstat);

    return (0);
}

/* �C���^�t�F�[�X ID �̏�Ԃ�ύX���� */
int change_if_id(int id, int stat)
{
    int index;

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    if((index = search_if_id(id)) < 0) return (-1);

    /* �C���^�t�F�[�X ID �̏�Ԃ�ύX���� */
    if_id[index].stat = stat;

    return (0);
}

/* �C���^�t�F�[�X ID ���폜���� */
int delete_if_id(int id)
{
    static int oldstat;
    int index, i, j;
    if_id_stat_t *new_if_id;

    /* �C���^�t�F�[�X ID �����݂��Ȃ��ꍇ�� -1 ��Ԃ� */
    if((index = search_if_id(id)) < 0) return (-1);

    /* �C���^�t�F�[�X ID ���ЂƂ��炷 */
    if_id_num--;

    CpuSuspendIntr(&oldstat);
    if(if_id_num){
	/* �V�����̈���m�� */
	new_if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);

	/* �C���^�t�F�[�X ID ���폜 */
	for(i = 0, j = 0; i < if_id_num + 1; i++){
	    if(i != index){
		new_if_id[j].id   = if_id[i].id;
		new_if_id[j].stat = if_id[i].stat;
		new_if_id[j].flag = if_id[i].flag;
		new_if_id[j].bus_type = if_id[i].bus_type;
		memcpy(new_if_id[j].bus_loc, if_id[i].bus_loc, sizeof(new_if_id[j].bus_loc));
		memcpy(new_if_id[j].vendor, if_id[i].vendor, sizeof(new_if_id[j].vendor));
		memcpy(new_if_id[j].product, if_id[i].product, sizeof(new_if_id[j].product));
		j++;
	    }
	}

	/* ���̗̈����� */
	FreeSysMemory(if_id);

	/* �|�C���^���X�V */
	if_id = new_if_id;
    }else{
	/* �̈����� */
	FreeSysMemory(if_id);
    }
    CpuResumeIntr(oldstat);

    return (0);
}
