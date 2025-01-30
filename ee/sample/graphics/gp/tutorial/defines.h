/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine Library Sample Program
 *
 *			- libgp tutorial -
 *
 *                         Version 1.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            defines.h
 *			 common header file
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            May,15,2002      aoki
 */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 224
#define OFFX (2048*16)
#define OFFY (2048*16)


/* convert X into GS primitive coordinate */
static inline int convert_x(float v)
{
    return (int)(v*16.0f)+OFFX;
}

/* convert Y into GS primitive coordinate */
static inline int convert_y(float v)
{
    return (int)(v*16)+OFFY;
}

/* convert Z into GS Z buffer value */
static inline int convert_z(float v)
{
    return (int)(v);
}
