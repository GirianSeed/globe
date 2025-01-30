/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - rautodma -
 *
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *
 *      Version    Date           Design   Log
 *  --------------------------------------------------------------------
 *      0.40       Jul,22,1999    morita   a mulfunction about interrupt fixed
 *      0.30       Jun,17,1999    morita   provisional version
 */

#include <eekernel.h>
#include <librspu2.h>
#include <rspu2cmd.h>

#define IS_LOOP		0
#define PCM_FILENAME	"host0:knot.int"
#define PCM_SIZE	1604888
#define ALLOC_IOP_HEAP_SIZE   1604888
#define PRINTF(x) printf x

int gCallbackThreadID;
int gMainThreadID;
static int gDataTopAddr;

int dataset(void)
{
	int i;
	sceSifInitIopHeap();
	PRINTF(("allocate IOP heap memory - " ));
	gDataTopAddr = sceSifAllocIopHeap(ALLOC_IOP_HEAP_SIZE);
	if( gDataTopAddr < 0 ) { printf( "\nCan't alloc heap \n"); return -1; }
	PRINTF(("alloced 0x%x  ", gDataTopAddr));

	i = sceSifLoadIopHeap( PCM_FILENAME, (void*)gDataTopAddr);
	if( i < 0 ) { printf( "\nCan't load PCM file to iop heap \n"); 
			return -1; }
	PRINTF(("- data loaded 0x%x \n", (int)gDataTopAddr ));

	return;
}


int IntFunc( void* common )
{
	PRINTF(("///// AutoDMA interrupt detected. /////\n"));
	return 0;
}


int rAutoDma( void )
{
	unsigned short flg1;
	static int thid;
	volatile int i, j, k;
	SpuCommonAttr c_attr;

	for( i = 0; i < 2; i++ )
	{
		// --- set commonn attribute
		sceSpu2Remote(1, rSpuSetCore, i );
		c_attr.mask = (SPU_COMMON_MVOLL |  SPU_COMMON_MVOLR);
		c_attr.mvol.left  = 0x3fff;	/* Master volume (left) */
		c_attr.mvol.right = 0x3fff;	/* Master volume (right) */
		sceSpu2Remote(1, rSpuSetCommonAttr, &c_attr );
	}

	// --- 準備・再生
	sceSpu2Remote(1, rSpuAutoDMASetCallback, IntFunc );
#if IS_LOOP
	sceSpu2Remote(1, rSpuAutoDMAWrite, gDataTopAddr
		, (PCM_SIZE/512)*512, SPU_AUTODMA_LOOP );
#else
	sceSpu2Remote(1, rSpuAutoDMAWrite, gDataTopAddr
		, PCM_SIZE, SPU_AUTODMA_ONESHOT );
#endif

	while(1){
		i = 1000; while( i-- ){}
		if( sceSpu2Remote(1, rSpuAutoDMAGetStatus ) == 0 ) break;
	}

	// --- 終了処理
	sceSpu2Remote(1, rSpuAutoDMAStop );
	DeleteThread( gCallbackThreadID );
	PRINTF(("Quit...\n"));

	return 0;
}


int main(void)
{
	//--- Callback Threadを動かすために、mainを最高プライオリティから下げる
	gMainThreadID = GetThreadId();
	ChangeThreadPriority(gMainThreadID , 10 );

	sceSpu2RemoteInit();

	//--- Callback Threadをmainよりも高いプライオリティで作る
	gCallbackThreadID = sceSpu2CallbackInit(5);
	if( dataset() < 0 ){ Exit(-1); }

	rAutoDma();

	Exit(0);
	return 0;
}
