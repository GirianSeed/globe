/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */

#include <stdio.h>

#define	IS_KANJI(_c)		((((_c)>=0x81) && ((_c)<=0x9F)) || (((_c)>=0xE0) && ((_c)<=0xFC)))
#define	IS_HANKAKU(_c)		(((_c)>=0xA1) && ((_c)<=0xDF))
#define	IS_ASCII(_c)		(((_c)>=0x20) && ((_c)<=0x7F))

unsigned int CharHanToZen(int c);
unsigned int CharZenToHan(unsigned int sjis);
void StrHanToZen(char *pszDst, const char *pszSrc);
void StrZenToHan(char *pszDst, const char *pszSrc);


// 半角文字を全角文字に変換する
unsigned int CharHanToZen(int c)
{
	static const char szHan2Zen[] =
		"　！”＃＄％＆’（）＊＋，－．／"		// 0x20～0x2F
		"０１２３４５６７８９：；＜＝＞？"		// 0x30～0x3F
		"＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ"		// 0x40～0x4F
		"ＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿"		// 0x50～0x5F	0x5Cは円マークという場合もある
		"‘ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ"		// 0x60～0x6F
		"ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～  "		// 0x70～0x7F	0x7Eはアッパーラインという場合もある
		"                                "		// 0x80～0x8F
		"                                "		// 0x90～0x9F
		"  。「」、・ヲァィゥェォャュョッ"		// 0xA0～0xAF
		#ifdef LANG_CSJIS
		"ーアイウエオカキクケオサシスセソ"		// 0xB0～0xBF
		#else
		"ーアイウエオカキクケオサシスセソ\"	//"	// 0xB0～0xBF
		#endif
		"タチツテトナニヌネノハヒフヘホマ"		// 0xC0～0xCF
		"ミムメモヤユヨラリルレロワン゛゜"		// 0xD0～0xDF
		"                                "		// 0xE0～0xEF
		"                                ";		// 0xF0～0xFF
	unsigned int sjis;

	if(IS_KANJI(c)) {
		// SJISの2バイトのとき、そのままコピー
		sjis = c;
	} else if(IS_ASCII(c)) {
		// 半角ASCIIを全角に変換する場合
		sjis = (szHan2Zen[c*2]<<8) | szHan2Zen[c*2 + 1];
	} else if(IS_HANKAKU(c)) {
		// 半角カタカナだった場合
		sjis = (szHan2Zen[c*2]<<8) | szHan2Zen[c*2 + 1];
	} else {
		// 1バイト文字として
		sjis = c;
	}
	return(sjis);
}


// 全角文字を半角文字に変換する
unsigned int CharZenToHan(unsigned int sjis)
{
	unsigned char c=0;
	// ひらがな判定用
	static const char us829F_82F1[] =
		"ｧ ｱ ｨ ｲ ｩ ｳ ｪ ｴ ｫ ｵ ｶ ｶﾞｷ ｷﾞｸ ｸﾞ"
		"ｹ ｹﾞｺ ｺﾞｻ ｻﾞｼ ｼﾞｽ ｽﾞｾ ｾﾞｿ ｿﾞﾀ ﾀﾞ"
		"ﾁ ﾁﾞｯ ﾂ ﾂﾞﾃ ﾃﾞﾄ ﾄﾞﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾊﾞ"
		"ﾊﾟﾋ ﾋﾞﾋﾟﾌ ﾌﾞﾌﾟﾍ ﾍﾞﾍﾟﾎ ﾎﾞﾎﾟﾏ ﾐ ﾑ "
		"ﾒ ﾓ ｬ ﾔ ｭ ﾕ ｮ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ   ﾜ "
		"    ｦ ﾝ ｳﾞ    ";

	// カタカナ判定用
	static const char us8340_8396[] =
		"ｧ ｱ ｨ ｲ ｩ ｳ ｪ ｴ ｫ ｵ ｶ ｶﾞｷ ｷﾞｸ ｸﾞ"
		"ｹ ｹﾞｺ ｺﾞｻ ｻﾞｼ ｼﾞｽ ｽﾞｾ ｾﾞｿ ｿﾞﾀ ﾀﾞ"
		"ﾁ ﾁﾞｯ ﾂ ﾂﾞﾃ ﾃﾞﾄ ﾄﾞﾅ ﾆ ﾇ ﾈ ﾉ ﾊ ﾊﾞ"
		"ﾊﾟﾋ ﾋﾞﾋﾟﾌ ﾌﾞﾌﾟﾍ ﾍﾞﾍﾟﾎ ﾎﾞﾎﾟﾏ ﾐ   "
		"ﾑ ﾒ ﾓ ｬ ﾔ ｭ ﾕ ｮ ﾖ ﾗ ﾘ ﾙ ﾚ ﾛ   ﾜ "
		"    ｦ ﾝ ｳﾞ    ";

	if(sjis<0x10) {
		// 1バイト文字として、そのままコピー
		c = sjis;
	} else {
		// SJISの2バイト文字のとき、szZen2Han内に文字があるかどうか
		if(0x824F<=sjis && sjis<=0x8258) {
			// '0'～'9'
			return((sjis - 0x824F) + '0');
		} else if(0x8260<=sjis && sjis<=0x8279) {
			// 'A'～'Z'
			return((sjis - 0x8260) + 'A');
		} else if(0x8281<=sjis && sjis<=0x829A) {
			// 'a'～'z'
			return((sjis - 0x8281) + 'a');
		} else if(0x829F<=sjis && sjis<=0x82F1) {
			// 'あ'～'ん'
			unsigned char c1=0, c2=0;

			c1 = us829F_82F1[(sjis - 0x829F) * 2];
			c2 = us829F_82F1[(sjis - 0x829F) * 2 + 1];
			if(c1==' ') {
				// 変換できない文字(ゎ,ゐ,ゑ)
				return(0);
			} else if(c2==' ') {
				// 半角1文字に変換できる文字
				return(c1);
			} else {
				// 半角2文字に変換できる場合
				return((c1<<8) | c2);
			}
		} else if(0x8340<=sjis && sjis<=0x8396) {
			// 'ア'～'ン'
			unsigned char c1, c2;
			c1 = us8340_8396[(sjis - 0x8340) * 2];
			c2 = us8340_8396[(sjis - 0x8340) * 2 + 1];
			if(c1==' ') {
				// 変換できない文字(ヮ,ヰ,ヱ,ヵ,ヶ)
				return(0);
			} else if(c2==' ') {
				// 半角1文字に変換できる文字
				return(c1);
			} else {
				// 半角2文字に変換できる場合
				return((c1<<8) | c2);
			}
		}
	}
	return(c);
}


// 半角カタカナを全角カタカナに変換する
void StrHanToZen(char *pszDst, const char *pszSrc)
{
	unsigned char c;
	unsigned int sjis;

	do {
		c = *pszSrc++;
		sjis = CharHanToZen(c);
		if(sjis<0x100) {
			*pszDst++ = c;
		} else {
			*pszDst++ = sjis>>8;
			*pszDst++ = sjis & 0xFF;
		}
	} while(c!=0x00);
	return;
}



// 全角ひらがな,カタカナを半角カタカナに変換する
void StrZenToHan(char *pszDst, const char *pszSrc)
{
	unsigned char c;

	do {
		c = *pszSrc++;
		if(IS_KANJI(c)) {
			// SJISの2バイトのとき、そのままコピー
			// 
			*pszDst++ = c;
			*pszDst++ = *pszSrc++;
		} else if(IS_ASCII(c)) {
			// 半角ASCIIだったとき
			*pszDst++ = c;
		} else if(IS_HANKAKU(c)) {
			// 半角カタカナだった場合
			*pszDst++ = c;
		} else {
			// 1バイト文字として
			*pszDst++ = c;
		}
	} while(c!=0x00);

	return;
}


