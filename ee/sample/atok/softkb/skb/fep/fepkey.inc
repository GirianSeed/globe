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

#ifdef __GNUC__
#define ZERO_ARG 0
#else
#define ZERO_ARG
#endif

//============================================================================
//  海外版
//============================================================================
// フランス語
#define numof_French_keyMap_circumflex (sizeof(French_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_French_keyMap_diaeresis (sizeof(French_keyMap_diaeresis)/sizeof(exchange_table_t))
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

#define numof_French_keyMap_tilde (sizeof(French_keyMap_tilde)/sizeof(exchange_table_t))
exchange_table_t French_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


#define numof_French_keyMap_grave (sizeof(French_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_French_keyMap_acute (0)
exchange_table_t French_keyMap_acute[ZERO_ARG];

#define numof_French_keyMap_dedilla (0)
exchange_table_t French_keyMap_dedilla[ZERO_ARG];


// スペイン語
#define numof_Spanish_keyMap_circumflex (sizeof(Spanish_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_Spanish_keyMap_diaeresis (sizeof(Spanish_keyMap_diaeresis)/sizeof(exchange_table_t))
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

#define numof_Spanish_keyMap_tilde (sizeof(Spanish_keyMap_tilde)/sizeof(exchange_table_t))
exchange_table_t Spanish_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


#define numof_Spanish_keyMap_grave (sizeof(Spanish_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_Spanish_keyMap_acute (sizeof(Spanish_keyMap_acute)/sizeof(exchange_table_t))
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

#define numof_Spanish_keyMap_dedilla (0)
exchange_table_t Spanish_keyMap_dedilla[ZERO_ARG];


// ドイツ語
#define numof_German_keyMap_circumflex (sizeof(German_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_German_keyMap_diaeresis (0)
exchange_table_t German_keyMap_diaeresis[ZERO_ARG];

#define numof_German_keyMap_tilde (0)
exchange_table_t German_keyMap_tilde[ZERO_ARG];


#define numof_German_keyMap_grave (sizeof(German_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_German_keyMap_acute (sizeof(German_keyMap_acute)/sizeof(exchange_table_t))
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

#define numof_German_keyMap_dedilla (0)
exchange_table_t German_keyMap_dedilla[ZERO_ARG];


// イタリア語
#define numof_Italian_keyMap_circumflex (0)
exchange_table_t Italian_keyMap_circumflex[ZERO_ARG];

#define numof_Italian_keyMap_diaeresis (0)
exchange_table_t Italian_keyMap_diaeresis[ZERO_ARG];

#define numof_Italian_keyMap_tilde (0)
exchange_table_t Italian_keyMap_tilde[ZERO_ARG];


#define numof_Italian_keyMap_grave (0)
exchange_table_t Italian_keyMap_grave[ZERO_ARG];

#define numof_Italian_keyMap_acute (0)
exchange_table_t Italian_keyMap_acute[ZERO_ARG];

#define numof_Italian_keyMap_dedilla (0)
exchange_table_t Italian_keyMap_dedilla[ZERO_ARG];


// オランダ語
#define numof_Dutch_keyMap_circumflex (sizeof(Dutch_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_Dutch_keyMap_diaeresis (sizeof(Dutch_keyMap_diaeresis)/sizeof(exchange_table_t))
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

#define numof_Dutch_keyMap_tilde (sizeof(Dutch_keyMap_tilde)/sizeof(exchange_table_t))
exchange_table_t Dutch_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


#define numof_Dutch_keyMap_grave (sizeof(Dutch_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_Dutch_keyMap_acute (sizeof(Dutch_keyMap_acute)/sizeof(exchange_table_t))
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

#define numof_Dutch_keyMap_dedilla (sizeof(Dutch_keyMap_dedilla)/sizeof(exchange_table_t))
exchange_table_t Dutch_keyMap_dedilla[] =
{
	{	'C',	0xc7U	},

	{	'c',	0xe7U	},
};


// ポルトガル
#define numof_Portuguese_keyMap_circumflex (sizeof(Portuguese_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_Portuguese_keyMap_diaeresis (sizeof(Portuguese_keyMap_diaeresis)/sizeof(exchange_table_t))
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

#define numof_Portuguese_keyMap_tilde (sizeof(Portuguese_keyMap_tilde)/sizeof(exchange_table_t))
exchange_table_t Portuguese_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


#define numof_Portuguese_keyMap_grave (sizeof(Portuguese_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_Portuguese_keyMap_acute (sizeof(Portuguese_keyMap_acute)/sizeof(exchange_table_t))
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

#define numof_Portuguese_keyMap_dedilla (0)
exchange_table_t Portuguese_keyMap_dedilla[ZERO_ARG];


// イギリス
#define numof_UK_keyMap_circumflex (0)
exchange_table_t UK_keyMap_circumflex[ZERO_ARG];

#define numof_UK_keyMap_diaeresis (0)
exchange_table_t UK_keyMap_diaeresis[ZERO_ARG];

#define numof_UK_keyMap_tilde (0)
exchange_table_t UK_keyMap_tilde[ZERO_ARG];


#define numof_UK_keyMap_grave (0)
exchange_table_t UK_keyMap_grave[ZERO_ARG];

#define numof_UK_keyMap_acute (0)
exchange_table_t UK_keyMap_acute[ZERO_ARG];

#define numof_UK_keyMap_dedilla (0)
exchange_table_t UK_keyMap_dedilla[ZERO_ARG];


// カナディアンフレンチ
#define numof_CANADAFR_keyMap_circumflex (sizeof(CANADAFR_keyMap_circumflex)/sizeof(exchange_table_t))
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

#define numof_CANADAFR_keyMap_diaeresis (sizeof(CANADAFR_keyMap_diaeresis)/sizeof(exchange_table_t))
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

#define numof_CANADAFR_keyMap_tilde (sizeof(CANADAFR_keyMap_tilde)/sizeof(exchange_table_t))
exchange_table_t CANADAFR_keyMap_tilde[] =
{
	{	'A',	0xc3U	},
	{	'O',	0xd5U	},
	{	'N',	0xd1U	},

	{	'a',	0xe3U	},
	{	'o',	0xf5U	},
	{	'n',	0xf1U	},
};


#define numof_CANADAFR_keyMap_grave (sizeof(CANADAFR_keyMap_grave)/sizeof(exchange_table_t))
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

#define numof_CANADAFR_keyMap_acute (0)
exchange_table_t CANADAFR_keyMap_acute[ZERO_ARG];

#define numof_CANADAFR_keyMap_dedilla (0)
exchange_table_t CANADAFR_keyMap_dedilla[ZERO_ARG];



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
			numof_French_keyMap_circumflex,
			numof_Spanish_keyMap_circumflex,
			numof_German_keyMap_circumflex,
			numof_Italian_keyMap_circumflex,
			numof_Dutch_keyMap_circumflex,
			numof_Portuguese_keyMap_circumflex,
			numof_UK_keyMap_circumflex,
			numof_CANADAFR_keyMap_circumflex,
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
			numof_French_keyMap_diaeresis,
			numof_Spanish_keyMap_diaeresis,
			numof_German_keyMap_diaeresis,
			numof_Italian_keyMap_diaeresis,
			numof_Dutch_keyMap_diaeresis,
			numof_Portuguese_keyMap_diaeresis,
			numof_UK_keyMap_diaeresis,
			numof_CANADAFR_keyMap_diaeresis,
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
			numof_French_keyMap_tilde,
			numof_Spanish_keyMap_tilde,
			numof_German_keyMap_tilde,
			numof_Italian_keyMap_tilde,
			numof_Dutch_keyMap_tilde,
			numof_Portuguese_keyMap_tilde,
			numof_UK_keyMap_tilde,
			numof_CANADAFR_keyMap_tilde,
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
			numof_French_keyMap_grave,
			numof_Spanish_keyMap_grave,
			numof_German_keyMap_grave,
			numof_Italian_keyMap_grave,
			numof_Dutch_keyMap_grave,
			numof_Portuguese_keyMap_grave,
			numof_UK_keyMap_grave,
			numof_CANADAFR_keyMap_grave,
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
			numof_French_keyMap_acute,
			numof_Spanish_keyMap_acute,
			numof_German_keyMap_acute,
			numof_Italian_keyMap_acute,
			numof_Dutch_keyMap_acute,
			numof_Portuguese_keyMap_acute,
			numof_UK_keyMap_acute,
			numof_CANADAFR_keyMap_acute,
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
			numof_French_keyMap_dedilla,
			numof_Spanish_keyMap_dedilla,
			numof_German_keyMap_dedilla,
			numof_Italian_keyMap_dedilla,
			numof_Dutch_keyMap_dedilla,
			numof_Portuguese_keyMap_dedilla,
			numof_UK_keyMap_dedilla,
			numof_CANADAFR_keyMap_dedilla,
		}
	},
};

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__FEPKBD_INC__
