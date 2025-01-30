/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

//#define __DEBUGPRINT__	1
#define __DEBUGPRINT__	0

static int bmp2gf_main(FILE* fp_d, FILE* fp_s);


static int get_param(char* work, size_t size, char** tag, char** param, FILE* fp);
static int _isalnumx(char c, char const* opt);
static int _isspace(char c);
static int _stricmp(char const* str0, char const* str1);
static int parse(char* buf, char** tag, char** param);

typedef unsigned char	u_int8;
typedef unsigned short	u_int16;
typedef unsigned int	u_int32;

typedef struct{
	u_int pixelDepth;
	u_int width;
	u_int height;
	u_int paletteCount;
	u_int lineLength;
	u_int8* pPalette;
	u_int8* pImage;
}BMP_INFO;

static int load_bmp(BMP_INFO* pBmp, FILE* fp);
static void _linecopy4(void* pDst, void const* pSrc, int count);
static void _linecopy8(void* pDst, void const* pSrc, int count);
static void _linecopy16(void* pDst, void const* pSrc, int count);
static void _linecopy24(void* pDst, void const* pSrc, int count);
static void _linecopy32(void* pDst, void const* pSrc, int count);

static void output_chr(FILE* fpd, BMP_INFO* pBmp, int cw, int ch, int x, int y, int neg);

int main(int argc, char const * const argv[]){


	if(3 > argc){
		printf(">bmp2gf config.txt output.gf\n");
		return -1;
	}

	{
		FILE* fp_s;
		FILE* fp_d = NULL;

		fp_s = fopen(argv[1], "rb");
		if(NULL == fp_s){
			printf("fopen(\"%s\", \"rb\") failed\n", argv[1]);
			return -1;
		}

		fp_d = fopen(argv[2], "wb");
		if(NULL == fp_d){
			printf("fopen(\"%s\", \"wb\") failed\n", argv[2]);
			return -1;
		}

		bmp2gf_main(fp_d, fp_s);

		fclose(fp_d);
		fclose(fp_s);
	}

	return 0;
}

static int bmp2gf_main(FILE* fp_d, FILE* fp_s){


	char bmppath[1024];
	int cwidth = -1;
	int cheight = -1;
	int mode = 0;
	int negative = 0;

	{
		char* tag;
		char* param;
		char work[1024];
		while(0 != get_param(work, sizeof(work), &tag, &param, fp_s)){

			if(0 == _stricmp(tag, "source")){
				strcpy(bmppath, param);

			}else if(0 == _stricmp(tag, "csize")){
				sscanf(param, "%d %d", &cwidth, &cheight);

			}else if(0 == _stricmp(tag, "mode")){
				mode = strtol(param, NULL, 10);

			}else if(0 == _stricmp(tag, "negative")){
				negative = strtol(param, NULL, 10);
			}
		}
	}

	if((0 > cwidth) || (0 > cheight)){
		printf("illegal parameter\n");
		return 0;
	}

	{
		BMP_INFO bi;
		int res;
		FILE* fp;
		fp = fopen(bmppath, "rb");
		if(NULL == fp){
			printf("%s open failed\n", bmppath);
		}

		res = load_bmp(&bi, fp);

		fclose(fp);
		if(0 == res){
			printf("%s laod filed\n", bmppath);
			return 0;
		}

		if(8 < bi.pixelDepth){
			printf("not support pixel depth %d\n", bi.pixelDepth);

		}else{
			// main
			int x;
			int y;
			if(0 == mode){
				for(x = 0; x < (bi.width / cwidth); x++){
					for(y = 0; y < (bi.height / cheight); y++){
						output_chr(fp_d, &bi, cwidth, cheight, x, y, negative);
					}
				}
			}else{
				for(y = 0; y < (bi.height / cheight); y++){
					for(x = 0; x < (bi.width / cwidth); x++){
						output_chr(fp_d, &bi, cwidth, cheight, x, y, negative);
					}
				}
			}
		}


		if(NULL != bi.pImage){
			free(bi.pImage);
		}
		if(NULL != bi.pPalette){
			free(bi.pPalette);
		}
	}


	return 1;
}

static void output_chr(FILE* fpd, BMP_INFO* pBmp, int cw, int ch, int x, int y, int neg){

	int _x;
	int _y;
	int cx = x * cw;
	int cy = y * ch;
	unsigned char* pWork;

	int ww = (cw + 1) / 2;
	pWork = malloc(ww);

#if __DEBUGPRINT__
	printf("%d %d (%d %d)\n", x, y, cw, ch);
#endif
	if(4 == pBmp->pixelDepth){
		// 4 bpp
		for(_y = 0; _y < ch; _y++){
			unsigned char* pLine = &pBmp->pImage[(cy + _y) * pBmp->lineLength];
			for(_x = 0; _x < cw; _x++){
				unsigned char data = pLine[(cx + _x) / 2];
				unsigned char pix = (0 == ((cx + _x) & 1)) ? (data & 0x0f) : (data >> 4);
				if(0 != neg){
					pix ^= 0xf;
				}
				pWork[_x / 2] = (0 == (_x & 1)) ? pix : ((pWork[_x / 2] & 0x0fU) | (pix << 4));
#if __DEBUGPRINT__
				printf("%x", pix);
#endif
			}
			fwrite(pWork, 1, ww, fpd);
#if __DEBUGPRINT__
			printf("\n");
#endif
		}
	}else{
		// 8 bpp
		for(_y = 0; _y < ch; _y++){
			unsigned char* pLine = &pBmp->pImage[(cy + _y) * pBmp->lineLength];
			for(_x = 0; _x < cw; _x++){
				unsigned char pix = pLine[cx + _x] >> 4;
				if(0 != neg){
					pix ^= 0xf;
				}
				pWork[_x / 2] = (0 == (_x & 1)) ? pix : ((pWork[_x / 2] & 0x0fU) | (pix << 4));
#if __DEBUGPRINT__
				printf("%x", pix);
#endif
			}
			fwrite(pWork, 1, ww, fpd);
#if __DEBUGPRINT__
			printf("\n");
#endif
		}
	}

	free(pWork);
}


//////////////////////////////////////////////

static int get_param(char* work, size_t size, char** tag, char** param, FILE* fp){
	do{
		if(NULL == fgets(work, size, fp)){
			return 0;
		}
	}while(0 == parse(work, tag, param));
	return 1;
}

// tag="param"形式を抽出。
// 簡易
static int parse(char* buf, char** tag, char** param){
	char c;

	// 頭のホワイトスペースを飛ばす
	while(0 != _isspace(c = *buf)){
		buf++;
	}

	// タグ部分抽出
	*tag = buf;
	while(0 != _isalnumx(c = *buf, NULL)){
		buf++;
	}
	if(*tag == buf){
		return 0;
	}

	*buf++ = '\0';

	// '=' を検索
	if('=' != c){
		while(0 != _isspace(c = *buf)){
			buf++;
		}
		if('=' != c){
			return 0;
		}
		buf++;
	}

	// 頭のホワイトスペースを飛ばす
	while(0 != _isspace(c = *buf)){
		buf++;
	}

	// パラメータは ""で囲われている
	if('\"' != c){
		return 0;
	}
	buf++;

	// タグ部分抽出
	*param = buf;
	while(0 != _isalnumx(c = *buf, "\t ,./?_\\|][@=)(&%$!#*+-;:<>{}")){
		buf++;
	}
	if((*param == buf) || ('\"' != c)){
		return 0;
	}
	*buf = '\0';

	return 1;
}

static int _stricmp(char const* str0, char const* str1){
	int result;
	char c0;
	char c1;
	do{
		c0 = *str0++;
		c1 = *str1++;
		result = c0 - c1;
	}while((0 == result) && ('\0' != c0)/* && ('\0' != c1) */);
	return result;
}

static int _isspace(char c){
	if(' ' == c){
		return 1;
	}
	if('\t' == c){
		return 1;
	}
	return 0;
}

static int _isalnumx(char c, char const* opt){
	if(('a' <= c) && ('z' >= c)){
		return 1;
	}
	if(('A' <= c) && ('Z' >= c)){
		return 1;
	}
	if(('0' <= c) && ('9' >= c)){
		return 1;
	}
	if(NULL != opt){
		char const* ptr = opt;
		char _c;
		while('\0' != (_c = *ptr++)){
			if(_c == c){
				return 1;
			}
		}
	}

	return 0;
}

typedef struct tagBITMAPFILEHEADER{
	u_int16 bfType;
	u_int16 bfSizeL;
	u_int16 bfSizeH;
	u_int16 bfReserved1;
	u_int16 bfReserved2;
	u_int16 bfOffBitsL;
	u_int16 bfOffBitsH;
}BITMAPFILEHEADER;			//	14 bytes

typedef struct tagBITMAPINFOHEADER{
	u_int32 biSize;
	u_int32 biWidth;
	u_int32 biHeight;
	u_int16 biPlanes;
	u_int16 biBitCount;
	u_int32 biCompression;
	u_int32 biSizeImage;
	u_int32 biXPelsPerMeter;
	u_int32 biYPelsPerMeter;
	u_int32 biClrUsed;
	u_int32 biClrImportant;
}BITMAPINFOHEADER;			//	40 bytes

typedef struct tagBITMAPINFO{
	BITMAPINFOHEADER bmih;
	u_int32 biColor[0];
}BITMAPINFO;

typedef enum{
	BI_RGB			= 0,
	BI_RLE8			= 1,
	BI_RLE4			= 2,
	BI_BITFIELDS	= 3,
}BMPCOMPRESSIONMODE;

static int load_bmp(BMP_INFO* pInfo, FILE* fp){


	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int len;
	int offset;
	int lineLength;
	int gsLLen;
	int w;
	int h;

	memset(pInfo, 0, sizeof(BMP_INFO));


	offset = 0;
	len = sizeof(bmfh);
	if(len != fread(&bmfh, 1, len, fp)){
		printf("load_bmp: read failed BITMAPFILEHEADER\n");
		return 0;
	}
	offset += len;

	if(0x4d42U != bmfh.bfType){
		printf("load_bmp: not BMP file\n");
		return 0;
	}

	len = sizeof(bmih);
	if(len != fread(&bmih, 1, len, fp)){
		printf("load_bmp: read failed BITMAPINFOHEADER\n");
		return 0;
	}
	offset += len;

	if(1 != bmih.biPlanes){
		printf("load_bmp: planes != 1\n");
		return 0;
	}
	if(BI_RGB != bmih.biCompression){
		printf("load_bmp: compression != BI_RGB\n");
		return 0;
	}

	w = bmih.biWidth;
	h = bmih.biHeight;

	switch(bmih.biBitCount){
	case 4:
		gsLLen = (w + 1) / 2;
		lineLength = (((w + 7) & ~7) / 8) * 4;
		if(0 == bmih.biClrUsed){
			bmih.biClrUsed = 4;
		}
		break;

	case 8:
		gsLLen = w;
		lineLength = (((w + 3) & ~3) / 4) * 4;
		if(0 == bmih.biClrUsed){
			bmih.biClrUsed = 256;
		}
		break;

	case 16:
		gsLLen = w * 2;
		lineLength = (((w + 1) & ~1) / 2) * 4;
		break;

	case 24:
		gsLLen = w * 3;
		lineLength = (w * 3 + 3) & ~3;
		break;

	case 32:
		gsLLen = w * 4;
		lineLength = w * 4;
		break;

	default:
		printf("load_bmp: unknown pixel mode\n");
		return 0;
	}

	pInfo->pixelDepth = bmih.biBitCount;
	pInfo->width = w;
	pInfo->height = h;
	pInfo->paletteCount = bmih.biClrUsed;
	pInfo->lineLength = gsLLen;
	pInfo->pPalette = NULL;
	pInfo->pImage = NULL;

	if(0 != bmih.biClrUsed){
		len = bmih.biClrUsed * sizeof(u_int32);
		pInfo->pPalette = malloc(len);
		if(NULL == pInfo->pPalette){
			printf("load_bmp: malloc failed\n");
			return 0;
		}
		if(len != fread(pInfo->pPalette, 1, len, fp)){
			printf("load_bmp: read failed palette\n");
			return 0;
		}
		_linecopy32(pInfo->pPalette, pInfo->pPalette, bmih.biClrUsed);
		offset += len;
	}

	{
		int const offBits = (int)bmfh.bfOffBitsL | ((int)bmfh.bfOffBitsH << 16);
		if(offBits != offset){
			if(offBits < offset){
				return 0;
			}
			len = offBits - offset;

			if(0 != fseek(fp, len, SEEK_CUR)){
				printf("load_bmp: seek failed\n");
				return 0;
			}
			offset += len;
		}

		pInfo->pImage = malloc(pInfo->lineLength * h);
		if(NULL == pInfo->pImage){
			printf("load_bmp: malloc failed - image\n");
			return 0;
		}

		{
			int y;
			u_int8* pLineBuf = malloc(lineLength);
			u_int8* pImage = pInfo->pImage + gsLLen * h;
			assert(NULL != pLineBuf);

			for(y = 0; y < h; y++){
				if(lineLength != fread(pLineBuf, 1, lineLength, fp)){
					printf("load_bmp: read failed - image\n");
					return 0;
				}
				pImage -= gsLLen;
				switch(pInfo->pixelDepth){
				case 4:
					_linecopy4(pImage, pLineBuf, w);
					break;

				case 8:
					_linecopy8(pImage, pLineBuf, w);
					break;

				case 16:
					_linecopy16(pImage, pLineBuf, w);
					break;

				case 24:
					_linecopy24(pImage, pLineBuf, w);
					break;

				case 32:
					_linecopy32(pImage, pLineBuf, w);
					break;
				}
			}
			offset += len;
		}
	}

#if __DEBUGPRINT__
	printf("load_bmp:\n");
	printf("  pixel = %d\n", pInfo->pixelDepth);
	printf("  width = %d\n", pInfo->width);
	printf("  height = %d\n", pInfo->height);
	printf("  palette = %d[%08x]\n", pInfo->paletteCount, (u_int)pInfo->pPalette);
	printf("  image = %d[%08x]\n", pInfo->lineLength, (u_int)pInfo->pImage);
#endif

	return 1;
}


static void _linecopy4(void* pDst, void const* pSrc, int count){
	memcpy(pDst, pSrc, (count + 1 ) / 2);
}

static void _linecopy8(void* pDst, void const* pSrc, int count){
	memcpy(pDst, pSrc, count);
}

static void _linecopy16(void* pDst, void const* pSrc, int count){
	u_int16* pDstImg = (u_int16*)pDst;
	u_int16 const * pSrcImg = (u_int16 const*)pSrc;

	while(count--){
		u_int16 data = *pSrcImg++;
		data = ((data & 0x001fU) << 10) | ((data & 0x7c00U) >> 10) | (data & 0x803e0U);
		*pDstImg++ = data;
	}
}

static void _linecopy24(void* pDst, void const* pSrc, int count){
	u_int8* pDstImg = (u_int8*)pDst;
	u_int8 const * pSrcImg = (u_int8 const*)pSrc;

	while(count--){
		u_int8 b = *pSrcImg++;
		u_int8 g = *pSrcImg++;
		u_int8 r = *pSrcImg++;
		*pDstImg++ = r;
		*pDstImg++ = g;
		*pDstImg++ = b;
	}
}

static void _linecopy32(void* pDst, void const* pSrc, int count){
	u_int32* pDstImg = (u_int32*)pDst;
	u_int32 const * pSrcImg = (u_int32 const*)pSrc;

	while(count--){
		u_int32 data = *pSrcImg++;
		data = ((data & 0x000000ffU) << 16) | ((data & 0x00ff0000U) >> 16) | (data & 0xff00ff00U);
		*pDstImg++ = data;
	}
}

//[eof]
