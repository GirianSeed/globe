/* SCE CONFIDENTIAL
   "PlayStation 2" Programmer Tool Runtime Library Libpfont Version 1.2
*/
/*
 *     Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                   All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>

#define __DEBUGPRINT__	0

// 複数のpbからpfファイルを作成。

#define __MAXNUM__	65536
#define __BORDER__	1

//#pragma pack(1)

typedef struct{
	struct{
		unsigned int id;
		unsigned int version;
		int size;
		int reserved;
	}tag;
	unsigned char name[32];
	unsigned char comment[32];
	short max_ascent;
	short max_descent;
	short max_width;
	short reserved;
	unsigned int nob;
	unsigned int block_offset[0];
}PFONT_HEADER;

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

static int pb2pf_main(FILE* fp_d, FILE* fp_s);
static int make_header(PFONT_HEADER* ph, char const* tag, char const* param);
static int create_pf(FILE* fp, char const* tag, char const* param);

static int get_param(char* work, size_t size, char** tag, char** param, FILE* fp);
static int _isalnumx(char c, char const* opt);
static int _isspace(char c);
static int _stricmp(char const* str0, char const* str1);
static int parse(char* buf, char** tag, char** param);
static int _align(int value, int count);
static int _falign(int count, FILE* fp);
static int _fcopy(size_t count, FILE* fp_s, FILE* fp_d);


int main(int argc, char const * const argv[]){

	if(3 > argc){
		printf(">pb2pf config.txt output.pf\n");
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

		pb2pf_main(fp_d, fp_s);

		fclose(fp_d);
		fclose(fp_s);
	}


	return 0;
}


static int pb2pf_main(FILE* fp_d, FILE* fp_s){

	char* tag;
	char* param;
	int nob = 0;
	size_t ph_size;
	PFONT_HEADER* ph = NULL;
	char work[1024];

	// まずはソース（ブロック）の数を数えてみる
	while(0 != get_param(work, sizeof(work), &tag, &param, fp_s)){
		if(0 == _stricmp(tag, "source")){
			nob++;
		}
	}

	// ブロックが無い
	if(0 == nob){
		printf("0 == nob\n");
		return 0;
	}

	// 元にもどして…
	fseek(fp_s, 0, SEEK_SET);

	// ヘッダを構築
	ph_size = sizeof(PFONT_HEADER) + nob * sizeof(unsigned int);
	ph = malloc(ph_size);
	if(NULL == ph){
		printf("malloc(%d) failed\n", ph_size);
	}
	memset(ph, 0, ph_size);
	ph->tag.id = 0x00000000;
	ph->tag.version = 0x00000000;
	ph->tag.size = _align(ph_size, 16);
	strcpy(ph->name, "unknown");
	strcpy(ph->comment, "unknown");

	// make_header でカウント
	ph->nob = 0;

	// ブロックから必要な情報を取り出す
	while(0 != get_param(work, sizeof(work), &tag, &param, fp_s)){
		if(0 == make_header(ph, tag, param)){
			printf("make_header(ph, %s, %s) failed\n", tag, param);
			free(ph);
			return 0;
		}
	}

	assert(nob == (int)ph->nob);

	// 元にもどして…
	fseek(fp_s, 0, SEEK_SET);

	// ヘッダを出力
	if(ph_size != fwrite(ph, 1, ph_size, fp_d)){
		printf("fwrite(&ph, 1, %d, fp_d) failed\n", ph_size);
	}
	_falign(16, fp_d);

	// ブロックを結合
	while(0 != get_param(work, sizeof(work), &tag, &param, fp_s)){
		if(0 == create_pf(fp_d, tag, param)){
			printf("create_pf(fp, %s, %s) failed\n", tag, param);
			free(ph);
			return 0;
		}
	}

	// メモリ捨てて
	free(ph);

	// 成功
	return 1;
}

static int make_header(PFONT_HEADER* ph, char const* tag, char const* param){

	if(0 == _stricmp(tag, "name")){
		strncpy(ph->name, param, sizeof(ph->name) - 1);
		ph->name[sizeof(ph->name) - 1] = '\0';

	}else if(0 == _stricmp(tag, "comment")){
		strncpy(ph->comment, param, sizeof(ph->comment) - 1);
		ph->comment[sizeof(ph->comment) - 1] = '\0';

	}else if(0 == _stricmp(tag, "source")){
		FILE* fp = fopen(param, "rb");
		BLOCK_INFO bf;
		if(NULL == fp){
			return 0;
		}
		if(sizeof(bf) != fread(&bf, 1, sizeof(bf), fp)){
			fclose(fp);
			return 0;
		}

		{
			short tmp;
			tmp = (short)(ceil((float)bf.max_ascent * bf.scale_y));
			if(ph->max_ascent < tmp){
				ph->max_ascent = tmp;
			}
			tmp = (short)(floor((float)bf.max_descent * bf.scale_y));
			if(ph->max_descent > tmp){
				ph->max_descent = tmp;
			}
			tmp = (short)(ceil((float)bf.max_width * bf.scale_x));
			if(ph->max_width < tmp){
				ph->max_width = tmp;
			}
		}
		ph->block_offset[ph->nob] = ph->tag.size;
		ph->tag.size = _align(ph->tag.size + bf.tag.size, 16);
		ph->nob++;

		fclose(fp);
	}

	return 1;
}

static int create_pf(FILE* fp_d, char const* tag, char const* param){
	if(0 == _stricmp(tag, "source")){
		FILE* fp_s = fopen(param, "rb");
		BLOCK_INFO bf;
		if(NULL == fp_s){
			return 0;
		}
		if(sizeof(bf) != fread(&bf, 1, sizeof(bf), fp_s)){
			fclose(fp_s);
			return 0;
		}

		if(sizeof(bf) != fwrite(&bf, 1, sizeof(bf), fp_d)){
			fclose(fp_s);
			return 0;
		}

		if(0 == _fcopy(bf.tag.size - sizeof(bf), fp_s, fp_d)){
			fclose(fp_s);
			return 0;
		}
		_falign(16, fp_d);

		fclose(fp_s);
	}

	return 1;
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

static int _fcopy(size_t count, FILE* fp_s, FILE* fp_d){
	while(count--){
		int c;
		if(EOF == (c = fgetc(fp_s))){
			return 0;
		}
		if(EOF == fputc(c, fp_d)){
			return 0;
		}
	}

	return 1;
}

// [eof]
