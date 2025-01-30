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

#define __DEBUGPRINT__	0
//#define __DEBUGPRINT__	0

#define __MAXNUM__	65536
//#define __REACH__	16
#define __REACH__	256
//#define __REACH__	65535


#define bulkof(a)	(sizeof(a) / sizeof(a[0]))

typedef struct{
	struct{
		unsigned int id;
		unsigned int version;
		int size;
		int reserved;
	}tag;

	struct{
		unsigned int color:3;
		unsigned int prop:1;
		unsigned int reserved:28;
	}attr;

	float scale_x;
	float scale_y;
	short image_width;
	short image_height;
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;

	struct{
		int chr_num;
		int chr_offset;

		int codeindex_num;
		int codeindex_offset;

		int codemap_num;
		int codemap_offset;

		int proportional_num;
		int proportional_offset;

		int kerning_num;
		int kerning_offset;

		int clut_num;
		int clut_offset;
	}info;

}BLOCK_INFO;

typedef struct{
	short base_x;
	short base_y;
	short l_bearing;
	short r_bearing;
	short ascent;
	short descent;
	short width;
	short kerning;
}PROPORTIONAL_INFO;

typedef struct{
	int start;
	int end;
	int map_index;
	int chr_index;
}CODEINDEX;

typedef struct{
	unsigned char const* pImage;
	PROPORTIONAL_INFO const* pProp;
}GLYPH;

#define NA	(128/(6-1))
#define NC	(255/(6-1))
static unsigned char const defFontClut[16][4] = {
	//	 R     G     B     A
	{0*NC, 0*NC, 0*NC, 0*NA},
	{0*NC, 0*NC, 0*NC, 1*NA},
	{0*NC, 0*NC, 0*NC, 2*NA},
	{0*NC, 0*NC, 0*NC, 3*NA},
	{0*NC, 0*NC, 0*NC, 4*NA},
	{1*NC, 1*NC, 1*NC, 2*NA},
	{1*NC, 1*NC, 1*NC, 3*NA},
	{1*NC, 1*NC, 1*NC, 4*NA},
	{1*NC, 1*NC, 1*NC, 5*NA},
	{2*NC, 2*NC, 2*NC, 3*NA},
	{2*NC, 2*NC, 2*NC, 4*NA},
	{2*NC, 2*NC, 2*NC, 5*NA},
	{3*NC, 3*NC, 3*NC, 4*NA},
	{3*NC, 3*NC, 3*NC, 5*NA},
	{4*NC, 4*NC, 4*NC, 5*NA},
	{5*NC, 5*NC, 5*NC, 5*NA}
};


typedef struct{
	unsigned int code;
	int image;
	int index;
	int mode;
}CODE_MAP;


static int gf2pb_main(FILE* fpd, FILE* fps);
static void create_block(FILE* fp, CODE_MAP* pMap, int num, unsigned char const* pImage, int width, int height, int base_x, int base_y, int border, int fix);
static void check_image(PROPORTIONAL_INFO* pPropInfo, unsigned char const* pImage, int width, int height, int base_x, int base_y, int border, int fixw);


static int get_param(char* work, size_t size, char** tag, char** param, FILE* fp);
static int _isalnumx(char c, char const* opt);
static int _isspace(char c);
static int _stricmp(char const* str0, char const* str1);
static int parse(char* buf, char** tag, char** param);
static int _align(int value, int count);
static int _falign(int count, FILE* fp);

int main(int argc, char const * const argv[]){


	if(3 > argc){
		printf(">gf2pb config.txt output.pb\n");
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

		gf2pb_main(fp_d, fp_s);

		fclose(fp_d);
		fclose(fp_s);
	}

	return 0;
}

static int gf2pb_main(FILE* fpd, FILE* fps){


	CODE_MAP map[__MAXNUM__];
	unsigned char* pImage = NULL;
	char gfpath[256];
	int width = -1;
	int height = -1;
	int base_x = -1;
	int base_y = -1;
	int border = 1;
	int fix = 0;
	int num = 0;

	gfpath[0] = '\0';
	memset(&map, 0, sizeof(map));

	{
		int index = 0;
		char* tag;
		char* param;
		char work[256];
		while(0 != get_param(work, sizeof(work), &tag, &param, fps)){

			if(0 == _stricmp(tag, "source")){
				strcpy(gfpath, param);

			}else if(0 == _stricmp(tag, "csize")){
				sscanf(param, "%d %d", &width, &height);

			}else if(0 == _stricmp(tag, "base")){
				sscanf(param, "%d %d", &base_x, &base_y);

			}else if(0 == _stricmp(tag, "border")){
				border = strtol(param, NULL, 10);

			}else if(0 == _stricmp(tag, "fix")){
				fix = strtol(param, NULL, 10);

			}else{
				char tmp[64];
				int ucs;
				int count;
				int mode;

				if(0 != _stricmp(tag, "_")){
					index = strtol(tag, NULL, 16);
				}
				sscanf(param, "%s %d %d", tmp, &count, &mode);
				ucs = strtol(&tmp[2], NULL, 16);

				while(0 != count--){
#if __DEBUGPRINT__
					printf("%04x=%04x(%d)\n", ucs, index, mode);
#endif
					if(0 <= mode){
						assert(__MAXNUM__ > num);
						map[num].code = ucs;
						map[num].image = index;
						map[num].mode = mode;
						num++;
					}
					ucs++;
					index++;
				}
			}
		}
	}
#if __DEBUGPRINT__
	printf("%s %d %d %d %d %d %d\n", gfpath, width, height, base_x, base_y, border, fix);
#endif

	// base_xが負であっても許可
	// assert((0 == fix) || (0 < base_x));

	{
		size_t size;
		FILE* fp;

		fp = fopen(gfpath, "rb");
		if(NULL == fp){
			printf("fopen(\"%s\", \"rb\") failed\n", gfpath);
			return -1;
		}

		fseek(fp, 0, SEEK_END);
		size = (size_t)ftell(fp);
		fseek(fp, 0, SEEK_SET);

		pImage = malloc(size);
		assert(NULL != pImage);

		if(0 == fread(pImage, 1, size, fp)){
			printf("fread(image, 1, %d) failed\n", size);
			fclose(fp);
			free(pImage);
			return -1;
		}

		fclose(fp);
	}

	create_block(fpd, map, num, pImage, width, height, base_x, base_y, border, fix);

	free(pImage);

	return 0;
}

static void create_block(FILE* fp, CODE_MAP* pMap, int num, unsigned char const* pImage, int width, int height, int base_x, int base_y, int border, int fix){

	BLOCK_INFO block_info;

	PROPORTIONAL_INFO* pPropInfo;
	CODEINDEX* pCodeIndex;
	unsigned short* pCodeMap;
	GLYPH* pGlyph;

	pPropInfo = malloc(sizeof(PROPORTIONAL_INFO) * num);
	pCodeIndex = malloc(sizeof(CODEINDEX) * num);
	pCodeMap = malloc(sizeof(unsigned short) * __MAXNUM__);
	pGlyph = malloc(sizeof(GLYPH) * num);

	assert(NULL != pPropInfo);
	assert(NULL != pCodeIndex);
	assert(NULL != pCodeMap);
	assert(NULL != pGlyph);

	memset(&block_info, 0, sizeof(block_info));
	memset(pPropInfo, 0, sizeof(PROPORTIONAL_INFO) * num);
	memset(pCodeIndex, 0, sizeof(CODEINDEX) * num);
	memset(pCodeMap, 0xffU, sizeof(unsigned short) * num);

	{
		block_info.tag.id = 0x00000000;
		block_info.tag.version = 0x00000000;
		//block_info.tag.size = ;

		block_info.scale_x = 1.0f;
		block_info.scale_y = 1.0f;

		block_info.image_width = width;
		block_info.image_height = height;

		block_info.attr.color = 0;		// 4bit index color
		block_info.attr.prop = (0 == fix) ? 1 : 0;
	}

	{
		int i;
		int index = 0;
		for(i = 0; i < num; i++){
			int image = pMap[i].image;
			if(0 == pMap[i].code){
				// スキップ
				continue;
			}

			// イメージから情報取得
			check_image(&pPropInfo[index], pImage + image * (width * height * 4) / 8, width, height, base_x, base_y, border, pMap[i].mode);


			if(0 >= (pPropInfo[index].r_bearing - pPropInfo[index].l_bearing)){
				// イメージが空白
				pMap[i].code = 0;	// 登録解除
				continue;
			}

			if(block_info.max_width < pPropInfo[index].width){
				block_info.max_width = pPropInfo[index].width;
			}
			if(block_info.max_ascent < pPropInfo[index].ascent){
				block_info.max_ascent = pPropInfo[index].ascent;
			}
			if(block_info.max_descent > pPropInfo[index].descent){
				block_info.max_descent = pPropInfo[index].descent;
			}


			if(0 == fix){
				int pi;
				for(pi = 0; pi < i; pi++){
					if((0 != pMap[pi].code) && (pMap[i].image == pMap[pi].image) && (pMap[i].mode == pMap[pi].mode)){
						break;
					}
				}

				if(pi != i){
					pMap[i].index = pMap[pi].index;

				}else{
					pMap[i].index = index;
					block_info.info.chr_num++;
					pGlyph[index].pImage = pImage + image * (width * height * 4) / 8;
					pGlyph[index].pProp = &pPropInfo[index];
					index++;
				}

			}else{
				pMap[i].index = index;
				block_info.info.chr_num++;
				pGlyph[index].pImage = pImage + image * (width * height * 4) / 8;
				pGlyph[index].pProp = &pPropInfo[index];
				index++;
			}
		}
	}

	// コード順にソートする
	{
		int i;
		for(i = 0; i < (num - 1); i++){
			int k;
			int s = i;
			int code = pMap[i].code;
			for(k = i + 1; k < num; k++){
				if(code > pMap[k].code){
					code = pMap[k].code;
					s = k;
				}
			}
			if(i != s){
				CODE_MAP map;
				memcpy(&map, &pMap[s], sizeof(CODE_MAP));
				memcpy(&pMap[s], &pMap[i], sizeof(CODE_MAP));
				memcpy(&pMap[i], &map, sizeof(CODE_MAP));
			}
		}
	}

	// codeidnex、codemap作成
	{
		int i;
		int prev = -1;
		int ci_no = -1;
		int map_index = 0;
		for(i = 0; i < num; i++){
			int code = pMap[i].code;
			if(0 == code){
				// 文字が割り当てられていない
				continue;
			}

			if((0 > ci_no) || (0 > prev) || (__REACH__ < (code - prev)) || (65535 <= (code - pCodeIndex[ci_no].start))){
				++ci_no;
				pCodeIndex[ci_no].start = code;
				pCodeIndex[ci_no].map_index = map_index;
				pCodeIndex[ci_no].chr_index = 0;

			}else{
				int count = code - prev - 1;
				while(count--){
					pCodeMap[map_index++] = 0;
				}
			}
			pCodeIndex[ci_no].end = code;
			pCodeMap[map_index++] = pMap[i].index;
			assert(code > prev);
			prev = code;
		}

		block_info.info.codeindex_num = ci_no + 1;
		block_info.info.codemap_num = map_index;
		block_info.info.proportional_num = (0 != fix) ? 1 : block_info.info.chr_num;
		block_info.info.kerning_num = 0;
		block_info.info.clut_num = 16;
	}

	// オフセット、サイズ計算
	{
		block_info.info.chr_offset = (sizeof(block_info) + 15) & ~15;
		block_info.info.codeindex_offset = block_info.info.chr_offset + block_info.info.chr_num * ((width * height * 4 / 8 + 15) & ~15);
		block_info.info.codemap_offset = (block_info.info.codeindex_offset + block_info.info.codeindex_num * sizeof(CODEINDEX) + 15) & ~15;
		block_info.info.proportional_offset = (block_info.info.codemap_offset + block_info.info.codemap_num * sizeof(unsigned short) + 15) & ~15;
		block_info.info.kerning_offset = (block_info.info.proportional_offset + block_info.info.proportional_num * sizeof(PROPORTIONAL_INFO) + 15) & ~15;
		block_info.info.clut_offset = (block_info.info.kerning_offset + block_info.info.kerning_num * 0 + 15) & ~15;
		block_info.tag.size = (block_info.info.clut_offset + block_info.info.clut_num * sizeof(unsigned int) + 15) & ~15;
	}

	{
		printf("[block_info]\n");
		printf("  ID=%08x\n", block_info.tag.id);
		printf("  VER=%08x\n", block_info.tag.version);
		printf("  size=%d(%08x)\n", block_info.tag.size, block_info.tag.size);
		printf("\n");
		printf("  attr color=%d prop=%d\n", block_info.attr.color, block_info.attr.prop);
		printf("  image width=%d height=%d\n", block_info.image_width, block_info.image_height);
		printf("  max ascent=%d decent=%d width=%d\n", block_info.max_ascent, block_info.max_descent, block_info.max_width);
		printf("  chr num=%d offset=%08x\n", block_info.info.chr_num, block_info.info.chr_offset);
		printf("  codeindex num=%d offset=%08x\n", block_info.info.codeindex_num, block_info.info.codeindex_offset);
		printf("  codemap num=%d offset=%08x\n", block_info.info.codemap_num, block_info.info.codemap_offset);
		printf("  prop num=%d offset=%08x\n", block_info.info.proportional_num, block_info.info.proportional_offset);
		printf("  kerning num=%d offset=%08x\n", block_info.info.kerning_num, block_info.info.kerning_offset);
		printf("  clut num=%d offset=%08x\n", block_info.info.clut_num, block_info.info.clut_offset);
		printf("\n");
	}

	// ファイルへ出力
	{
		int i;

		fwrite(&block_info, sizeof(block_info), 1, fp);
		_falign(16, fp);

		for(i = 0; i < block_info.info.chr_num; i++){
			fwrite(pGlyph[i].pImage, sizeof(char), width * height * 4 / 8, fp);
			_falign(16, fp);
		}

		fwrite(pCodeIndex, sizeof(CODEINDEX), block_info.info.codeindex_num, fp);
		_falign(16, fp);

		fwrite(pCodeMap, sizeof(unsigned short), block_info.info.codemap_num, fp);
		_falign(16, fp);

		if(0 != block_info.attr.prop){
			for(i = 0; i < block_info.info.chr_num; i++){
				fwrite(pGlyph[i].pProp, sizeof(PROPORTIONAL_INFO), 1, fp);
			}

		}else{
			PROPORTIONAL_INFO prop;
			prop.base_x = base_x;
			prop.base_y = base_y;
			prop.ascent = block_info.max_ascent;
			prop.descent = block_info.max_descent;
			prop.l_bearing = -base_x;
			prop.r_bearing = width;
			prop.width = prop.r_bearing - border - base_x;
			fwrite(&prop, sizeof(PROPORTIONAL_INFO), 1, fp);
		}
		_falign(16, fp);

		fwrite(defFontClut, sizeof(unsigned int), block_info.info.clut_num, fp);
		_falign(16, fp);

		assert(ftell(fp) == block_info.tag.size);
	}

	free(pGlyph);
	free(pCodeMap);
	free(pCodeIndex);
	free(pPropInfo);
}


static void check_image(PROPORTIONAL_INFO* pPropInfo, unsigned char const* pImage, int width, int height, int base_x, int base_y, int border, int fixw){

	{
		unsigned char const* const end = &pImage[width * height * 4 / 8];
		unsigned char const* ptr;
		for(ptr = pImage; ptr < end; ptr++){
			if(0 != *ptr){
				break;
			}
		}
		if(ptr == end){
			return;
		}
	}

	{
		int top = height;
		int bottom = 0;
		int left = width;
		int right = 0;

		int y;
		for(y = 0; y < height; y++){

			int _left = width;
			int _right = 0;

			int x;
			for(x = 0; x < width; x++){
				unsigned char data = pImage[(y * width + x) / 2];
				unsigned char pixel = (0 == (x & 1)) ? (data & 0x0f) : (data >> 4);
#if __DEBUGPRINT__
				printf("%c", ((pixel < 10) ? '0' : ('A' - 10)) + pixel);
#endif
				if(0 == pixel){
					continue;
				}
				if(x < _left){
					_left = x;
				}
				if((x + 1) > _right){
					_right = x + 1;
				}
			}
#if __DEBUGPRINT__
			printf("\n");
#endif

			if(0 < (_right - _left)){
				if(y < top){
					top = y;
				}
				if((y + 1) > bottom){
					bottom = y + 1;
				}
				if(_left < left){
					left = _left;
				}
				if(_right > right){
					right = _right;
				}
			}
		}

		if(0 > base_x){
			if(0 < fixw){
				left -= (fixw - (right - left - border * 2)) / 2;
				right += left + fixw + border * 2;
			}
			base_x = left + border;

		}else{
			if(0 < fixw){
				right = base_x + fixw + border;
			}
		}

		if(base_y > bottom){
			bottom = base_y;
		}
		if(base_y < top){
			top = base_y;
		}
		if(base_x < left){
			left = base_x;
		}
		if(base_x > right){
			right = base_x;
		}

		pPropInfo->base_x = base_x;
		pPropInfo->base_y = base_y;
		pPropInfo->ascent = -(top - base_y);
		pPropInfo->descent = -(bottom - base_y);
		pPropInfo->l_bearing = left - base_x;
		pPropInfo->r_bearing = right - base_x;
		pPropInfo->width = (right - base_x - border);
		pPropInfo->kerning = 0;

#if __DEBUGPRINT__
		printf("base_x=%d\n", pPropInfo->base_x);
		printf("base_y=%d\n", pPropInfo->base_y);
		printf("ascent=%d\n", pPropInfo->ascent);
		printf("descent=%d\n", pPropInfo->descent);
		printf("l_bearing=%d\n", pPropInfo->l_bearing);
		printf("r_bearing=%d\n", pPropInfo->r_bearing);
		printf("width=%d\n", pPropInfo->width);
		printf("\n");
#endif
	}
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
	while(0 != _isalnumx(c = *buf, "_")){
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

	// パラメータ部分抽出
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

static int _align(int value, int count){
	int tmp = value + count - 1;
	return tmp - (tmp % count);
}

static int _falign(int count, FILE* fp){
	int current = ftell(fp);
	int aligned = _align(current, count);
	int diff = aligned - current;
	if(0 != diff){
		while(diff--){
			if(EOF == fputc(0, fp)){
				return 0;
			}
		}
	}
	return 1;
}


// [eof]
