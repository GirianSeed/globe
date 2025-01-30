/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
 */
/*
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <libccc - libccc.c>
 */

#include "libccc.h"

static sceCccJISCS	__error_char_sjis__		= 0x0000U;
static sceCccUCS4	__error_char_utf8__		= 0x00000000U;
static sceCccUCS4	__error_char_utf16__	= 0x00000000U;

static sceCccUCS2 const* __table_ptr_jis2ucs__ = NULL;
static sceCccJISCS const* __table_ptr_ucs2jis__ = NULL;

void sceCccSetTable(sceCccUCS2 const* jis2ucs, sceCccJISCS const* ucs2jis){
	__table_ptr_jis2ucs__ = jis2ucs;
	__table_ptr_ucs2jis__ = ucs2jis;
}

sceCccJISCS sceCccSetErrorCharSJIS(sceCccJISCS code){
	sceCccJISCS old = __error_char_sjis__;
	__error_char_sjis__ = code;
	return old;
}

sceCccUCS4 sceCccSetErrorCharUTF8(sceCccUCS4 code){
	sceCccUCS4 old = __error_char_utf8__;
	__error_char_utf8__ = code;
	return old;
}

sceCccUCS4 sceCccSetErrorCharUTF16(sceCccUCS4 code){
	sceCccUCS4 old = __error_char_utf16__;
	__error_char_utf16__ = code;
	return old;
}


int sceCccIsValidUCS4(sceCccUCS4 code){

	if(0x7fffffffU < code){
		return 0;
	}

	return 1;
}

int sceCccIsValidUCS2(sceCccUCS4 code){

	if(0x0000ffffU < code){
		return 0;
	}

	return 1;
}

int sceCccIsValidJIS(sceCccJISCS code){

	u_int m = (u_int)(code >> 15) & 0x01U;
	u_int k = (u_int)(code >> 8) & 0x7fU;
	u_int t = (u_int)(code & 0x007fU);

	if(((0x20 + 1) > k) || ((0x20 + 94) < k) || ((0x20 + 1) > t) || ((0x20 + 94) < t)){
		return 0;
	}

	if((0 != m) && (((0x20 + 2) == k) || ((0x20 + 6) == k) || ((0x20 + 7) == k) || (((0x20 + 9) <= k) && ((0x20 + 11) >= k)) || (((0x20 + 16) <= k) && ((0x20 + 77) >= k)))){
		return 0;
	}

	return 1;
}

int sceCccIsValidUTF8(sceCccUCS4 code){
	return sceCccIsValidUCS4(code);
}

int sceCccIsValidUTF16(sceCccUCS4 code){

	if(0x0010ffffU < code){
		return 0;
	}

	if((0x0000d800U <= code) && (0x0000dfffU >= code)){
		return 0;
	}

	return 1;
}

int sceCccIsValidSJIS(sceCccJISCS code){

	if((0x007fU >= code) || ((0x00a0 <= code) && (0x00df >= code))){
		return 1;
	}

	return sceCccIsValidJIS(code);
}

int sceCccStrlenUTF16(sceCccUTF16 const* str){
	int result = 0;

	while(0x00000000U != sceCccDecodeUTF16(&str)){
		result++;
	}

	return result;
}

int sceCccStrlenUTF8(sceCccUTF8 const* str){
	int result = 0;

	while(0x00000000U != sceCccDecodeUTF8(&str)){
		result++;
	}

	return result;
}

int sceCccStrlenSJIS(sceCccSJISTF const* str){
	int result = 0;

	while(0x0000U != sceCccDecodeSJIS(&str)){
		result++;
	}

	return result;
}

sceCccUCS4 sceCccDecodeUTF16(sceCccUTF16 const** str){
	sceCccUTF16 const* ptr = *str;
	sceCccUTF16 c;
	sceCccUCS4 code;

	c = *ptr++;

	if((0xd800U <= c) && (0xdbffU >= c)){

		sceCccUTF16 c2 = *ptr++;

		if((0xdc00U <= c2) && (0xdfffU >= c2)){
			code = ((sceCccUCS4)c << 16) | ((sceCccUCS4)c2);

		}else{
			code = __error_char_utf16__;
		}


	}else if((0xdc00U <= c) && (0xdfffU >= c)){
		code = __error_char_utf16__;

	}else{
		code = (sceCccUCS4)c;
	}

	*str = ptr;

	return code;
}

void sceCccEncodeUTF16(sceCccUTF16** dst, sceCccUCS4 code){
	sceCccUTF16* ptr = *dst;

	if(!sceCccIsValidUTF16(code)){
		code = __error_char_utf16__;
	}

	if(code <= 0x0000ffffU){
		*ptr++ = (sceCccUTF16)code;

	}else{
		code -= 0x00010000U;
		*ptr++ = 0xd800U + (sceCccUTF16)(code >> 10);
		*ptr++ = 0xdc00U + (sceCccUTF16)(code & 0x000003ffU);
	}

	*dst = ptr;
}

sceCccUCS4 sceCccDecodeUTF8(sceCccUTF8 const** str){
	sceCccUTF8 const* ptr = *str;
	sceCccUTF8 c;
	sceCccUCS4 code;

	c = *ptr++;

	if(0x80U > c){
		code = (sceCccUCS4)c;

	}else if((0xc2U > c) || (0xf4U < c)){
		code = __error_char_utf8__;

	}else{
		sceCccUCS4 max = 0x00000040U;
		code = (sceCccUCS4)c & (0x00000080U - 1);

		for(;;){
			code &= max - 1;
			max <<= 5;
#if 0
			if(0 == max){
				code = __error_char_utf8__;
				break;
			}
#endif
			code = (code << 6) | (sceCccUCS4)(*ptr++ & 0x3fU);
			if(code < max){
				break;
			}
		}
	}

	*str = ptr;

	return code;
}

void sceCccEncodeUTF8(sceCccUTF8** dst, sceCccUCS4 code){
	sceCccUTF8* ptr = *dst;

	if(!sceCccIsValidUTF8(code)){
		code = __error_char_utf8__;
	}

	if(0x00000080U > code){
		*ptr++ = (sceCccUTF8)code;

	}else{

		sceCccUCS4 max = 0x00000800U;
		int count = 1;
		while(code >= max){
			count++;
			max <<= 5;
#if 0
			if(0 == max){
				*dst = ptr;
				return;
			}
#endif
		}

		ptr += count + 1;
		{
			sceCccUTF8* _ptr = ptr;
			sceCccUTF8 const msk = (1 << (7 - count)) - 1;

			while(count--){
				_ptr--;
				*_ptr = 0x80U | (sceCccUTF8)(code & 0x0000003fU);
				code >>= 6;
			}

			*--_ptr = (0xffU & ~msk) | (code & (msk >> 1));
		}
	}

	*dst = ptr;
}

sceCccJISCS sceCccDecodeSJIS(sceCccSJISTF const** str){
	sceCccSJISTF const* ptr = *str;
	sceCccSJISTF c;
	sceCccJISCS code;

	c = *ptr++;

	if((0x7fU >= c) || ((0xa0U <= c) && (0xdfU >= c)) || (0xfdU <= c)){
		code = (sceCccJISCS)c;

	}else{
		u_int m = 0;
		u_int k = 0;
		u_int t = 0;

		// 第一バイト
		if((0x81U <= c) && (0x9fU >= c)){
			k = ((u_int)c - 0x81U) * 2 + 1;

		}else if((0xe0U <= c) && (0xefU >= c)){
			k = ((u_int)c - 0xe0U) * 2 + 63;

		}else if((0xf0U <= c) && (0xfcU >= c)){
			m = 1;
			k = ((u_int)c - 0xf0U) * 2 + 7;
			if(0xf4 <= c){
				k += 77 - ((4 * 2) + 7);
			}
		}

		// 第二バイト
		if(0 != k){
			c = *ptr++;

			if((0x40U <= c) && (0x7eU >= c)){
				t = (u_int)c - 0x40U + 1;

			}else if((0x80U <= c) && (0x9eU >= c)){
				t = (u_int)c - 0x80U + 64;

			}else if((0x9fU <= c) && (0xfcU >= c)){
				t = (u_int)c - 0x9fU + 1;
				k += 1;
			}

			if(0 != m){
				if(7 == k){
					k = 1;
				}else if(11 == k){
					k = 5;
				}else if(77 == k){
					k = 15;
				}
			}
		}

		if((0 == k) || (0 == t)){
			code = __error_char_sjis__;

		}else{
			code = (sceCccJISCS)((m << 15) | ((k + 0x20U) << 8) | (t + 0x20U));
		}
	}

	*str = ptr;

	return code;
}

void sceCccEncodeSJIS(sceCccSJISTF** dst, sceCccJISCS code){
	sceCccSJISTF* ptr = *dst;

	if(!sceCccIsValidSJIS(code)){
		code = __error_char_sjis__;
	}

	if((0x007fU >= code) || ((0x00a0U <= code) && (0x00dfU >= code)) || ((0x00fdU <= code) && (0x00ffU >= code))){
		*ptr++ = (sceCccSJISTF)code;

	}else{
		u_int m = ((u_int)code >> 15) & 0x01U;
		u_int k = (((u_int)code >> 8) & 0x7fU) - 0x20U;
		u_int t = ((u_int)code & 0x007fU) - 0x20U;

		// 第一バイト
		if(0 == m){
			*ptr++ = (sceCccSJISTF)((k + ((62 >= k) ? 0x0101U : 0x0181U)) / 2);

		}else{

			if(15 >= k){
				*ptr++ = (sceCccSJISTF)((k + 0x01dfU) / 2 - (k / 8) * 3);

			}else{
				*ptr++ = (sceCccSJISTF)((k + 0x019bU) / 2);
			}
		}

		// 第二バイト
		if(0 == (k & 1)){
			*ptr++ = (sceCccSJISTF)(t + 0x009eU);

		}else{
			*ptr++ = (sceCccSJISTF)(t + ((63 >= t) ? 0x003fU : 0x0040U));
		}
	}

	*dst = ptr;
}

sceCccJISCS sceCccUCStoJIS(sceCccUCS4 code, sceCccJISCS chr){

	sceCccJISCS result;

	if(0x00010000 <= code){
		result = 0;

	}else{
		result = __table_ptr_ucs2jis__[code];
	}

	return (0 == result) ? chr : result;
}


sceCccUCS4 sceCccJIStoUCS(sceCccJISCS code, sceCccUCS4 chr){

	sceCccUCS4 result;

	result = (sceCccUCS4)__table_ptr_jis2ucs__[code];

	return (0 == result) ? chr : result;
}

int sceCccUTF8toUTF16(sceCccUTF16* dst, size_t count, sceCccUTF8 const* str){
	int result = 0;
	sceCccUCS4 c;
	sceCccUTF16 work[__MAXCHRSIZE_UTF16__ / sizeof(sceCccUTF16)];

	if(sizeof(sceCccUTF16) > count){
		return result;
	}
	count -= sizeof(sceCccUTF16);

	while(0x00000000U != (c = sceCccDecodeUTF8(&str))){
		size_t size;
		int len;
		{
			sceCccUTF16* ptr = work;
			sceCccEncodeUTF16(&ptr, c);
			len = ptr - work;
			size = len * sizeof(sceCccUTF16);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccUTF16 const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x0000U;

	return result;
}

int sceCccUTF8toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF8 const* str){
	int result = 0;
	sceCccUCS4 c;
	sceCccSJISTF work[__MAXCHRSIZE_SJIS__ / sizeof(sceCccSJISTF)];

	if(sizeof(sceCccSJISTF) > count){
		return result;
	}
	count -= sizeof(sceCccSJISTF);

	while(0x00000000U != (c = sceCccDecodeUTF8(&str))){
		size_t size;
		int len;
		{
			sceCccSJISTF* ptr = work;
			sceCccJISCS const code = sceCccUCStoJIS(c, __error_char_sjis__);
			if(0x0000U == code){
				break;
			}
			sceCccEncodeSJIS(&ptr, code);
			len = ptr - work;
			size = len * sizeof(sceCccSJISTF);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccSJISTF const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x00U;

	return result;
}

int sceCccSJIStoUTF8(sceCccUTF8* dst, size_t count, sceCccSJISTF const* str){
	int result = 0;
	sceCccJISCS c;
	sceCccUTF8 work[__MAXCHRSIZE_UTF8__ / sizeof(sceCccUTF8)];

	if(sizeof(sceCccUTF8) > count){
		return result;
	}
	count -= sizeof(sceCccUTF8);

	while(0x0000U != (c = sceCccDecodeSJIS(&str))){
		size_t size;
		int len;
		{
			sceCccUTF8* ptr = work;
			sceCccUCS4 const code = sceCccJIStoUCS(c, __error_char_utf8__);
			if(0x0000U == code){
				break;
			}
			sceCccEncodeUTF8(&ptr, code);
			len = ptr - work;
			size = len * sizeof(sceCccUTF8);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccUTF8 const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x00U;

	return result;
}

int sceCccSJIStoUTF16(sceCccUTF16* dst, size_t count, sceCccSJISTF const* str){
	int result = 0;
	sceCccJISCS c;
	sceCccUTF16 work[__MAXCHRSIZE_UTF16__ / sizeof(sceCccUTF16)];

	if(sizeof(sceCccUTF16) > count){
		return result;
	}
	count -= sizeof(sceCccUTF16);

	while(0x0000U != (c = sceCccDecodeSJIS(&str))){
		size_t size;
		int len;
		{
			sceCccUTF16* ptr = work;
			sceCccUCS4 const code = sceCccJIStoUCS(c, __error_char_utf16__);
			sceCccEncodeUTF16(&ptr, code);
			len = ptr - work;
			size = len * sizeof(sceCccUTF16);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccUTF16 const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x0000U;

	return result;
}

int sceCccUTF16toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF16 const* str){
	int result = 0;
	sceCccUCS4 c;
	sceCccSJISTF work[__MAXCHRSIZE_SJIS__ / sizeof(sceCccSJISTF)];

	if(sizeof(sceCccSJISTF) > count){
		return result;
	}
	count -= sizeof(sceCccSJISTF);

	while(0x00000000U != (c = sceCccDecodeUTF16(&str))){
		size_t size;
		int len;
		{
			sceCccSJISTF* ptr = work;
			sceCccJISCS const code = sceCccUCStoJIS(c, __error_char_sjis__);
			if(0x0000U == code){
				break;
			}
			sceCccEncodeSJIS(&ptr, code);
			len = ptr - work;
			size = len * sizeof(sceCccSJISTF);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccSJISTF const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x00U;

	return result;
}

int sceCccUTF16toUTF8(sceCccUTF8* dst, size_t count, sceCccUTF16 const* str){
	int result = 0;
	sceCccUCS4 c;
	sceCccUTF8 work[__MAXCHRSIZE_UTF8__ / sizeof(sceCccUTF8)];

	if(sizeof(sceCccUTF8) > count){
		return result;
	}
	count -= sizeof(sceCccUTF8);

	while(0x00000000U != (c = sceCccDecodeUTF16(&str))){
		size_t size;
		int len;
		{
			sceCccUTF8* ptr = work;
			sceCccEncodeUTF8(&ptr, c);
			len = ptr - work;
			size = len * sizeof(sceCccUTF8);
		}

		if(count < size){
			break;
		}
		count -= size;

		{
			sceCccUTF8 const* ptr = work;
			while(len--){
				*dst++ = *ptr++;
			}
		}

		result++;
	}
	*dst++ = 0x00U;

	return result;
}

//[eof]
