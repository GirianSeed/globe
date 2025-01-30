/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/*
 *	Inet Setting Application Sample
 *
 *                          Version 1.4
 *
 *      Copyright (C) 2000-2001 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                              util.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2000.12.22      tetsu      Initial
 *        1.2           2001.01.31      tetsu      Change Final Flow
 *        1.3           2001.03.11      tetsu      Change for HDD
 *        1.4           2001.03.16      tetsu      Change for PPPoE
 */
#include "setapp.h"

/* ëIëÇµÇƒÇ¢ÇÈï∂éöî‘çÜ */
int disp_char_no;

/* ëIëÇ≈Ç´ÇÈï∂éö */
char disp_char[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
		    'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
		    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		    'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7',
		    '8', '9', '.', ',', '#', '*', '-', '+', '(', ')',
		    '=', '~', '|', '!', '"', '^', '`', '@', '{', '}',
		    '[', ']', '%', ':', ';', '<', '>', '?', '/', '&',
		    '$', '_', '\\', '\'', ' '};	/* CMAX = 95 ï∂éö */

int
search_char(char c)
{
  int no;

  no = 0;

  while(disp_char[no] != c){
    no++;
    if(no == CMAX){
      printf("character not found (%c)\n", c);
      no = -1;
      break;
    }
  }

  return (no);
}

void
delete_char(char *p, int n)
{
  if(n < strlen(p)){
    p += n;

    do{
      p++;
      *(p - 1) = *p;
    }while(*p != '\0');
  }
}

void
insert_char(char *p, int n, char c, int cx_max, int cx_min)
{
  int i;

  if((i = strlen(p)) + 1 <= cx_max - cx_min + 1){
    *(p + i + 1) = '\0';
    while(i != n){
      *(p + i) = *(p + i - 1);
      i--;
    }
    *(p + i) = c;
  }
}

void
write_char(char *p, int n, char c, int cx_max)
{
  int len;

  len = strlen(p);

  if(n <= cx_max - 1){
    p += n;
    *p = c;
    if(len <= n) *(p + 1) = '\0';
  }
}
