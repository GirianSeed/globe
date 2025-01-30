/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         ball_game - ntohf.c
 */

#include <stdio.h>

union U4byte {
	float Fdata;
	int   Idata;
	char  data[4];
};

union U8byte {
	double Ddata;
	char data[8];
};

float htonf(float hostfloat) {

	char tmp[4];
	int i;
	union U4byte bytedata;
	bytedata.Fdata = hostfloat;
	for (i=0;i<4;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[3];
	bytedata.data[1] = tmp[2];
	bytedata.data[2] = tmp[1];
	bytedata.data[3] = tmp[0];

	return(bytedata.Fdata);
}

int htoni(int hostint) {

	char tmp[4];
	int i;
	union U4byte bytedata;
	bytedata.Idata = hostint;
	for (i=0;i<4;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[3];
	bytedata.data[1] = tmp[2];
	bytedata.data[2] = tmp[1];
	bytedata.data[3] = tmp[0];

	return(bytedata.Idata);
}

double htond(double hostdouble) {

	char tmp[8];
	int i;
	union U8byte bytedata;
	bytedata.Ddata = hostdouble;
	for (i=0;i<8;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[7];
	bytedata.data[1] = tmp[6];
	bytedata.data[2] = tmp[5];
	bytedata.data[3] = tmp[4];
	bytedata.data[4] = tmp[3];
	bytedata.data[5] = tmp[2];
	bytedata.data[6] = tmp[1];
	bytedata.data[7] = tmp[0];
	return(bytedata.Ddata);
}

int ntohi(int netint) {

	char tmp[4];
	int i;
	union U4byte bytedata;
	bytedata.Idata = netint;
	for (i=0;i<4;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[3];
	bytedata.data[1] = tmp[2];
	bytedata.data[2] = tmp[1];
	bytedata.data[3] = tmp[0];

	return(bytedata.Idata);
}


float ntohf(float netfloat) {
	char tmp[4];
	int i;
	union U4byte bytedata;
	bytedata.Fdata = netfloat;
	for (i=0;i<4;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[3];
	bytedata.data[1] = tmp[2];
	bytedata.data[2] = tmp[1];
	bytedata.data[3] = tmp[0];

	return(bytedata.Fdata);
}

double ntohd(double netdouble) {

	char tmp[8];
	int i;
	union U8byte bytedata;
	bytedata.Ddata = netdouble;
	for (i=0;i<8;i++) tmp[i] = bytedata.data[i];
	bytedata.data[0] = tmp[7];
	bytedata.data[1] = tmp[6];
	bytedata.data[2] = tmp[5];
	bytedata.data[3] = tmp[4];
	bytedata.data[4] = tmp[3];
	bytedata.data[5] = tmp[2];
	bytedata.data[6] = tmp[1];
	bytedata.data[7] = tmp[0];
	return(bytedata.Ddata);
}
