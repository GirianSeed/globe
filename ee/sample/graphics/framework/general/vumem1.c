/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *               Graphics Framework Sample Program
 *                  
 *              ---- Scene Viewer Version 0.40 ----
 * 
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          Name : vumem1.c
 *
 *            Version   Date           Design      Log
 *  --------------------------------------------------------------------
 *            0.40      Jun,21,2000    kaneko
 */
#include <sys/types.h>
#include "vumem1.h"

static u_int _fwVumem1IsizeVal;
static u_int _fwVumem1OsizeVal;
static u_int _fwVumem1IrestVal;
static u_int _fwVumem1OrestVal;
static u_int _fwVumem1IptrVal;
static u_int _fwVumem1OptrVal;
static u_int _fwVumem1IstartVal;
static u_int _fwVumem1OstartVal;

extern void ErrPrintf(char *, ...);

void fwVumem1SetParam(u_int isize, u_int osize)
{
    _fwVumem1IsizeVal	= isize;
    _fwVumem1OsizeVal	= osize;
    _fwVumem1IstartVal	= 0;	// unpackR ‚ðŽg‚¤‚Æ‚«‚É‚Í 0
    _fwVumem1OstartVal	= 0;

    fwVumem1Reset();
}

void fwVumem1Reset()
{
    _fwVumem1IptrVal	= _fwVumem1IstartVal;
    _fwVumem1OptrVal	= _fwVumem1OstartVal;
    _fwVumem1IrestVal	= _fwVumem1IsizeVal;
    _fwVumem1OrestVal	= _fwVumem1OsizeVal;
}

int fwVumem1CheckRest(u_int isize, u_int osize)
{
    if (_fwVumem1IrestVal < isize)
	return VUMEM1_OVERFLOW;
    if (_fwVumem1OrestVal < osize)
	return VUMEM1_OVERFLOW;
    return VUMEM1_OK;
}

int fwVumem1Rest(int ioffset, int i_per_vertex, int ooffset, int o_per_vertex)
{
    int imax, omax;

    imax = (_fwVumem1IrestVal - ioffset) / i_per_vertex;
    omax = (_fwVumem1OrestVal - ooffset) /o_per_vertex;
    if (imax > omax)
	return omax;
    else
	return imax;
}

int fwVumem1IRest()
{
    return _fwVumem1IrestVal;
}

int fwVumem1ORest()
{
    return _fwVumem1OrestVal;
}

int fwVumem1ISize()
{
    return _fwVumem1IsizeVal;
}

int fwVumem1OSize()
{
    return _fwVumem1OsizeVal;
}


u_int fwVumem1GetIptr(u_int isize)
{
    u_int iptr;

    iptr = _fwVumem1IptrVal;
    if (isize > 0) {
	if (fwVumem1CheckRest(isize, 0) != VUMEM1_OK)
	    ErrPrintf("vumem1 input buffer overflow");
	_fwVumem1IrestVal -= isize;
	_fwVumem1IptrVal += isize;
    }
    return iptr;
}

u_int fwVumem1GetOptr(u_int osize)
{
    u_int optr;

    optr = _fwVumem1OptrVal;
    if (osize > 0) {
	if (fwVumem1CheckRest(0, osize) != VUMEM1_OK)
	    ErrPrintf("vumem1 output buffer overflow");
	_fwVumem1OrestVal -= osize;
	_fwVumem1OptrVal += osize;
    }
    return optr;
}
