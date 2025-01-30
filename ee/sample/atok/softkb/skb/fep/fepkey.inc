/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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

#ifndef __FEPKBD_INC__
#define __FEPKBD_INC__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

typedef struct
{
	u_int nowCode;
	u_int outCode;
} exchange_table_t;

//============================================================================
//  海外版
//============================================================================
// フランス語
exchange_table_t French_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t French_keyMap_diaeresis[] =
{
	{	'A',	0xc4U	},
	{	'E',	0xcbU	},
	{	'U',	0xdcU	},
	{	'I',	0xcfU	},
	{	'O',	0xd6U	},

	{	'a',	0xe4U	},
	{	'e',	0xebU	},
	{	'y',	0xffU	},
	{	'u',	0xfcU	},
	{	'i',	0xefU	},
	{	'o',	0xf6U	},
};

exchange_table_t French_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


exchange_table_t French_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t French_keyMap_acute[] =
{
};

exchange_table_t French_keyMap_dedilla[] =
{
};


// スペイン語
exchange_table_t Spanish_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t Spanish_keyMap_diaeresis[] =
{
	{	'A',	0xc4U	},
	{	'E',	0xcbU	},
	{	'U',	0xdcU	},
	{	'I',	0xcfU	},
	{	'O',	0xd6U	},

	{	'a',	0xe4U	},
	{	'e',	0xebU	},
	{	'y',	0xffU	},
	{	'u',	0xfcU	},
	{	'i',	0xefU	},
	{	'o',	0xf6U	},
};

exchange_table_t Spanish_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


exchange_table_t Spanish_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t Spanish_keyMap_acute[] =
{
	{	'A',	0xc1U	},
	{	'E',	0xc9U	},
	{	'U',	0xdaU	},
	{	'I',	0xcdU	},
	{	'O',	0xd3U	},
	{	'Y',	0xddU	},

	{	'a',	0xe1U	},
	{	'e',	0xe9U	},
	{	'u',	0xfaU	},
	{	'i',	0xedU	},
	{	'o',	0xf3U	},
	{	'y',	0xfdU	},
};

exchange_table_t Spanish_keyMap_dedilla[] =
{
};


// ドイツ語
exchange_table_t German_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t German_keyMap_diaeresis[] =
{
};

exchange_table_t German_keyMap_tilde[] =
{
};


exchange_table_t German_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t German_keyMap_acute[] =
{
	{	'A',	0xc1U	},
	{	'E',	0xc9U	},
	{	'U',	0xdaU	},
	{	'I',	0xcdU	},
	{	'O',	0xd3U	},
	{	'Y',	0xddU	},

	{	'a',	0xe1U	},
	{	'e',	0xe9U	},
	{	'u',	0xfaU	},
	{	'i',	0xedU	},
	{	'o',	0xf3U	},
	{	'y',	0xfdU	},
};

exchange_table_t German_keyMap_dedilla[] =
{
};


// イタリア語
exchange_table_t Italian_keyMap_circumflex[] =
{
};

exchange_table_t Italian_keyMap_diaeresis[] =
{
};

exchange_table_t Italian_keyMap_tilde[] =
{
};


exchange_table_t Italian_keyMap_grave[] =
{
};

exchange_table_t Italian_keyMap_acute[] =
{
};

exchange_table_t Italian_keyMap_dedilla[] =
{
};


// オランダ語
exchange_table_t Dutch_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t Dutch_keyMap_diaeresis[] =
{
	{	'A',	0xc4U	},
	{	'E',	0xcbU	},
	{	'U',	0xdcU	},
	{	'I',	0xcfU	},
	{	'O',	0xd6U	},

	{	'a',	0xe4U	},
	{	'e',	0xebU	},
	{	'y',	0xffU	},
	{	'u',	0xfcU	},
	{	'i',	0xefU	},
	{	'o',	0xf6U	},
};

exchange_table_t Dutch_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


exchange_table_t Dutch_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t Dutch_keyMap_acute[] =
{
	{	'A',	0xc1U	},
	{	'E',	0xc9U	},
	{	'U',	0xdaU	},
	{	'I',	0xcdU	},
	{	'O',	0xd3U	},
	{	'Y',	0xddU	},

	{	'a',	0xe1U	},
	{	'e',	0xe9U	},
	{	'u',	0xfaU	},
	{	'i',	0xedU	},
	{	'o',	0xf3U	},
	{	'y',	0xfdU	},
};

exchange_table_t Dutch_keyMap_dedilla[] =
{
	{	'C',	0xc7U	},

	{	'c',	0xe7U	},
};


// ポルトガル
exchange_table_t Portuguese_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t Portuguese_keyMap_diaeresis[] =
{
	{	'A',	0xc4U	},
	{	'E',	0xcbU	},
	{	'U',	0xdcU	},
	{	'I',	0xcfU	},
	{	'O',	0xd6U	},

	{	'a',	0xe4U	},
	{	'e',	0xebU	},
	{	'y',	0xffU	},
	{	'u',	0xfcU	},
	{	'i',	0xefU	},
	{	'o',	0xf6U	},
};

exchange_table_t Portuguese_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


exchange_table_t Portuguese_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t Portuguese_keyMap_acute[] =
{
	{	'A',	0xc1U	},
	{	'E',	0xc9U	},
	{	'U',	0xdaU	},
	{	'I',	0xcdU	},
	{	'O',	0xd3U	},
	{	'Y',	0xddU	},

	{	'a',	0xe1U	},
	{	'e',	0xe9U	},
	{	'u',	0xfaU	},
	{	'i',	0xedU	},
	{	'o',	0xf3U	},
	{	'y',	0xfdU	},
};

exchange_table_t Portuguese_keyMap_dedilla[] =
{
};


// イギリス
exchange_table_t UK_keyMap_circumflex[] =
{
};

exchange_table_t UK_keyMap_diaeresis[] =
{
};

exchange_table_t UK_keyMap_tilde[] =
{
};


exchange_table_t UK_keyMap_grave[] =
{
};

exchange_table_t UK_keyMap_acute[] =
{
};

exchange_table_t UK_keyMap_dedilla[] =
{
};


// カナディアンフレンチ
exchange_table_t CANADAFR_keyMap_circumflex[] =
{
	{	'A',	0xc2U	},
	{	'E',	0xcaU	},
	{	'U',	0xdbU	},
	{	'I',	0xceU	},
	{	'O',	0xd4U	},

	{	'a',	0xe2U	},
	{	'e',	0xeaU	},
	{	'u',	0xfbU	},
	{	'i',	0xeeU	},
	{	'o',	0xf4U	},
};

exchange_table_t CANADAFR_keyMap_diaeresis[] =
{
	{	'A',	0xc4U	},
	{	'E',	0xcbU	},
	{	'U',	0xdcU	},
	{	'I',	0xcfU	},
	{	'O',	0xd6U	},

	{	'a',	0xe4U	},
	{	'e',	0xebU	},
	{	'y',	0xffU	},
	{	'u',	0xfcU	},
	{	'i',	0xefU	},
	{	'o',	0xf6U	},
};

exchange_table_t CANADAFR_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


exchange_table_t CANADAFR_keyMap_grave[] =
{
	{	'A',	0xc0U	},
	{	'E',	0xc8U	},
	{	'U',	0xd9U	},
	{	'I',	0xccU	},
	{	'O',	0xd2U	},

	{	'a',	0xe0U	},
	{	'e',	0xe8U	},
	{	'u',	0xf9U	},
	{	'i',	0xecU	},
	{	'o',	0xf2U	},
};

exchange_table_t CANADAFR_keyMap_acute[] =
{
};

exchange_table_t CANADAFR_keyMap_dedilla[] =
{
};



typedef struct
{
	u_char				idiomCode;		// 発音記号

	exchange_table_t	*key[8];		// キー変換テーブル
	int					keyNum[8];		// キー変換テーブル要素数
} idiom_t;

idiom_t g_idiomTbl[] =
{
/*circumflex*/
	{
		'^',
		{
			French_keyMap_circumflex,
			Spanish_keyMap_circumflex,
			German_keyMap_circumflex,
			Italian_keyMap_circumflex,
			Dutch_keyMap_circumflex,
			Portuguese_keyMap_circumflex,
			UK_keyMap_circumflex,
			CANADAFR_keyMap_circumflex,
		},
		{
			sizeof(French_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(German_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_circumflex)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_circumflex)/sizeof(exchange_table_t),
		}
	},
/*diaeresis */
	{
		0xa8U,
		{
			French_keyMap_diaeresis,
			Spanish_keyMap_diaeresis,
			German_keyMap_diaeresis,
			Italian_keyMap_diaeresis,
			Dutch_keyMap_diaeresis,
			Portuguese_keyMap_diaeresis,
			UK_keyMap_diaeresis,
			CANADAFR_keyMap_diaeresis,
		},
		{
			sizeof(French_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(German_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_diaeresis)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_diaeresis)/sizeof(exchange_table_t),
		}
	},
/*tilde*/
	{
		'~',
		{
			French_keyMap_tilde,
			Spanish_keyMap_tilde,
			German_keyMap_tilde,
			Italian_keyMap_tilde,
			Dutch_keyMap_tilde,
			Portuguese_keyMap_tilde,
			UK_keyMap_tilde,
			CANADAFR_keyMap_tilde,
		},
		{
			sizeof(French_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(German_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_tilde)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_tilde)/sizeof(exchange_table_t),
		}
	},
/*grave*/
	{
		'`',
		{
			French_keyMap_grave,
			Spanish_keyMap_grave,
			German_keyMap_grave,
			Italian_keyMap_grave,
			Dutch_keyMap_grave,
			Portuguese_keyMap_grave,
			UK_keyMap_grave,
			CANADAFR_keyMap_grave,
		},
		{
			sizeof(French_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(German_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_grave)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_grave)/sizeof(exchange_table_t),
		}
	},
/*acute*/
	{
		0xb4U,
		{
			French_keyMap_acute,
			Spanish_keyMap_acute,
			German_keyMap_acute,
			Italian_keyMap_acute,
			Dutch_keyMap_acute,
			Portuguese_keyMap_acute,
			UK_keyMap_acute,
			CANADAFR_keyMap_acute,
		},
		{
			sizeof(French_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(German_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_acute)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_acute)/sizeof(exchange_table_t),
		}
	},
/*dedilla*/
	{
		0xb8U,
		{
			French_keyMap_dedilla,
			Spanish_keyMap_dedilla,
			German_keyMap_dedilla,
			Italian_keyMap_dedilla,
			Dutch_keyMap_dedilla,
			Portuguese_keyMap_dedilla,
			UK_keyMap_dedilla,
			CANADAFR_keyMap_dedilla,
		},
		{
			sizeof(French_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(Spanish_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(German_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(Italian_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(Dutch_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(Portuguese_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(UK_keyMap_dedilla)/sizeof(exchange_table_t),
			sizeof(CANADAFR_keyMap_dedilla)/sizeof(exchange_table_t),
		}
	},
};

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__FEPKBD_INC__
