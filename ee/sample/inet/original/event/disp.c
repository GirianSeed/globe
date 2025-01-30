/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         disp.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.02.18      tetsu       Change directory
 */

#include <string.h>
#include <devfont.h>

#define WIDTH_MAX  (75)
#define HEIGHT_MAX (18)

static char line[HEIGHT_MAX][WIDTH_MAX];
static char *disp[HEIGHT_MAX];
static int sort_flag, lh, lw;

/* ディスプレイの初期化 */
void init_disp(void)
{
    int i;

    for(i = 0; i < HEIGHT_MAX; i++){
	strcpy(line[i], "");
	disp[i] = line[i];
    }
    lh = lw = 0;
    sort_flag = 0;
}

/* ディスプレイの並べ変え */
static void sort_disp(int no)
{
    int i, j;

    for(i = HEIGHT_MAX - 1, j = no; i >= 0; i--){
	disp[i] = line[j];
	j = (j == 0) ? HEIGHT_MAX - 1 : j - 1;
    }
}

/* 文字列を画面に追加 */
void add_disp(char *str_top)
{
    int i;

    for(i = 0; *(str_top + i) != '\0'; i++){
	switch(*(str_top + i)){
	case '\n':
	    {
	CHANGE_LINE:
		line[lh][lw] = '\0';
		if(sort_flag) sort_disp(lh);
		lw = 0;
		if(lh == HEIGHT_MAX - 1){
		    lh = 0;
		    sort_flag = 1;
		}else{
		    lh++;
		}
	    }
	    break;
	default:
	    {
		line[lh][lw] = *(str_top + i);
		if( lw == WIDTH_MAX - 2 ){
		    lw++;
		    line[lh][lw] = '\0';
		    goto CHANGE_LINE;
		}else{
		    lw++;
		}
		line[lh][lw] = '\0';
	    }
	    break;
	}
    }
}

/* 画面を描画 */
void print_disp(int console)
{
    int i;

    for(i = 0; i < HEIGHT_MAX; i++){
	sceDevConsPrintf(console, "%s\n", disp[i]);
    }
}
