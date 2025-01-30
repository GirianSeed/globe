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

//=====================================================================================
// スレッド同期つきFIFOパイプ
//
// このモジュールは、スレッドセーフな単方向FIFOパイプの実装です。
// リングバッファを1個のセマフォによって同期制御します。
//
// バッファがフル/エンプティのとき、
// 送信側または受信側のどちらがスレッド待ち状態に入るかで2種類の使い方があります。
//               セマフォ待ち   動作
// FifoPostData()    なし      FIFOにデータを置く
//                             バッファ空き容量が足りなかった場合はエラー終了
// FifoRecvData()    あり      FIFOからデータを得る
//                             指定バイト数だけデータが得られるまでウェイト
// FifoSendData()    あり      FIFOにデータを置く
//                             指定バイト数だけデータが置けるようになるまでウェイト
// FifoPollData()    なし      FIFOからデータを得る
//                             バッファ内に指定バイト数だけデータがなかった場合エラー終了
//=====================================================================================
//	Send側             FIFO              Recv側
//
//	pData           +----------+
//	                | nQueHead |
//	                |          |
//	                +----------+
//	                | nInBytes |->Recv先
//	                |          |
//	                +----------+ 
//	       Send先 ->|          |
//	                |          |
//	                |          |
//	pData+nBufSize	+----------+
//
//	SendがWaitせずにFIFOに送り込める条件は、nBufsize-nInByte >= 送信要求サイズ
//	RecvがWaitせずにFIFOに送り込める条件は、nInByte >= 受信要求サイズ



#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include "mtfifo.h"


static void CheckWaitRequest(FIFO *pFifo);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//	FifoCreate												ref: mtfifo.h
//
//	マルチスレッドFIFOキューの作成
//
//	FIFO *pFifo : FIFO構造体のポインタ
//	void *pBuf  : キューバッファのポインタ
//	int nBufSize : キューバッファのサイズ
//
//	戻り値 : int : 0:成功/-1:失敗
int FifoCreate(FIFO *pFifo, void *pBuf, int nBufSize)
{
	struct SemaParam sp;
	// FIFOキューのためのセマフォを作成
	sp.maxCount  = 1;							// セマフォ最大値
	sp.initCount = 1;							// セマフォ初期値
	sp.option    = 0;							// セマフォ付加情報
	pFifo->FifoSema   = CreateSema(&sp);		// MFIFOの排他アクセス用のセマフォ生成
	
	if(pFifo->FifoSema<0)
	{
		// セマフォ生成に失敗したのでエラー終了
		return(pFifo->FifoSema);
	}
	WaitSema(pFifo->FifoSema);

	pFifo->pData    = pBuf;						// キューバッファのアドレスを設定
	pFifo->nBufSize = nBufSize;					// キューに登録可能なエレメントの最大個数
	pFifo->nQueHead = 0;						// 先頭インデクス番号を初期化
	pFifo->nInBytes = 0;						// 現在キュー内にあるエレメント数
	pFifo->nSendReqSize = 0;					// Recive時にWait解除の条件の初期値
	pFifo->nRecvReqSize = 0;					// Send時のWait解除の条件の初期値
	
	sp.initCount = 0;
	pFifo->SendSema = CreateSema(&sp);
	if(pFifo->SendSema<0)
	{
		DeleteSema(pFifo->FifoSema);
		return(pFifo->SendSema);
	}
	pFifo->RecvSema = CreateSema(&sp);
	if(pFifo->RecvSema < 0)
	{
		DeleteSema(pFifo->FifoSema);
		DeleteSema(pFifo->SendSema);
		return(pFifo->RecvSema);
	}
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoDelete												ref: mtfifo.h
//
//	マルチスレッドFIFOキューを解放
//
//	FIFO *pFifo : FIFO構造体のポインタ
void FifoDelete(FIFO *pFifo)
{
	DeleteSema(pFifo->FifoSema);						// セマフォ解放
	DeleteSema(pFifo->SendSema);
	DeleteSema(pFifo->RecvSema);
	return;
}


//============================================================================
//	FifoFlush												ref: mtfifo.h
//
//	FIFOパイプをフラッシュする
//
//	FIFO *pFifo : FIFO構造体のポインタ
void FifoFlush(FIFO *pFifo)
{
	int ret=-1;

	ret = WaitSema(pFifo->FifoSema);
	if(ret < 0)
	{
		// MFIFOが未初期化状態で呼び出された。
		return;		
	}
	pFifo->nQueHead = 0;						// 先頭インデクス番号を初期化
	pFifo->nInBytes = 0;						// 現在キュー内にあるエレメント数を初期化
	pFifo->nSendReqSize = 0;
	pFifo->nRecvReqSize = 0;

	SignalSema(pFifo->FifoSema);
	return;
}


//============================================================================
//	FifoGetBytes											ref: mtfifo.h
//
//	FIFOキュー内にたまっているデータバイト数
//
//	FIFO *pFifo : FIFO構造体のポインタ
//
//	戻り値 : u_int : キュー内の要素数
u_int FifoGetBytes(FIFO *pFifo)
{
	u_int r;
	WaitSema(pFifo->FifoSema);
	r = pFifo->nInBytes;
	SignalSema(pFifo->FifoSema);
	return r;					// キュー内の要素数を戻り値として返す
}


//============================================================================
//	FifoGetBytes											ref: mtfifo.h
//
//	FIFOキューに値を追加
//	セマフォ待ちなし。FifoRecvData()とペアで使用
//
//	FIFO *pFifo : FIFO構造体のポインタ
//	const void *pData : FIFOキューに渡したいデータのポインタ
//	int nSize : FIFOキューに渡したいデータのサイズ
//
//	戻り値 : int : 0:成功/1:エラー(キューが溢れそう)
int FifoPostData(FIFO *pFifo, const void *pData, int nSize)
{
	int t, m, n;

	if(nSize == 0)
	{
		return(0);
	}

	WaitSema(pFifo->FifoSema);

	m = pFifo->nBufSize;						// 最大要素数を得る
	n = pFifo->nInBytes;						// 現在FIFOパイプ内にあるデータバイト数を得る
	if((m - n) < nSize)
	{
		// キューの空き容量よりも大きいとき、エラー終了
		SignalSema(pFifo->FifoSema);
		return(1);
	}

	t = pFifo->nQueHead + n;					// 末端インデクスを得る
	if(t > m)
	{
		t -= m;
	}

	// ラップアラウンドしないでコピーできるバイト数を計算
	n = m - t;
	if(nSize <= n)
	{
		// ラップアラウンドなし
		memcpy((char*)&pFifo->pData[t], pData, (size_t)nSize);
	}
	else
	{
		// ラップアラウンドあり
		memcpy((char*)&pFifo->pData[t], pData, (size_t)n);
		memcpy((char*)pFifo->pData, &((const char *)pData)[n], (size_t)(nSize - n));
	}
	pFifo->nInBytes += nSize;					// FIFOパイプ内の要素数を増やす
	
	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);

	return(0);
}


//============================================================================
//	FifoRecvData											ref: mtfifo.h
//
//	キュー値から値を取得
//	セマフォ待ちあり。FifoPostData()とペアで使用
//
//	FIFO *pFifo : FIFO構造体のポインタ
//	const void *pData : FIFOキューに渡したいデータのポインタ
//	int nSize : FIFOキューに渡したいデータのサイズ
//	int fPeek : 
//
//	戻り値 : u_int : 0:成功/1:エラー(キューが溢れそう)
u_int FifoRecvData(FIFO *pFifo, void *pData, int nSize, int fPeek)
{
	int h, m, n;
	int bWait = FALSE;

	if(nSize == 0)
	{
		return(0);
	}
	
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;		// 最大要素数を得る
	n = pFifo->nInBytes;

	if(nSize > m)
	{
		// キューの最大容量よりも大きいとき、エラー終了
		SignalSema(pFifo->FifoSema);
		return(1);
	}

	// FIFOキュー内にデータが溜められるまでループ
	if( n < nSize)
	{
		pFifo->nRecvReqSize = nSize;
		bWait = TRUE;
	}
	SignalSema(pFifo->FifoSema);

	if(bWait==TRUE)
	{
	  WaitSema(pFifo->RecvSema);
	}
	WaitSema(pFifo->FifoSema);

	// ラップアラウンドしないでコピーできるバイト数を計算
	h = pFifo->nQueHead;						// 先頭インデクスを得る
	n = m - h;
	if(nSize <= n)
	{
		// ラップアラウンドなし
		memcpy(pData, (char*)&pFifo->pData[h], (size_t)nSize);
		if(!fPeek)
		{
			pFifo->nQueHead = (h + nSize) % m;
			pFifo->nInBytes -= nSize;			// FIFOパイプ内の要素数を減らす
		}
	}
	else
	{
		// ラップアラウンドあり
		memcpy(pData,  (char*)&pFifo->pData[h], (size_t)n);
		memcpy(&((char *)pData)[n], (char*)pFifo->pData, (size_t)(nSize - n));
		if(!fPeek)
		{
			pFifo->nQueHead = nSize - n;
			pFifo->nInBytes -= nSize;			// FIFOパイプ内の要素数を減らす
		}
	}

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoSendData											ref: mtfifo.h
//
//	FIFOキューに値を追加
//	セマフォ待ちあり。FifoPollData()とペアで使用
//
//	FIFO *pFifo : FIFO構造体のポインタ
//	const void *pData : FIFOキューに渡したいデータのポインタ
//	int nSize : FIFOキューに渡したいデータのサイズ
//
//	戻り値 : int : 0:成功/1:エラー(キューが溢れそう)
int FifoSendData(FIFO *pFifo, const void *pData, int nSize)
{
	int t, m ,n;
	int bWait = FALSE;

	if(nSize == 0)
	{
		return(0);
	}
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;

	if( m < nSize)
	{
		// キューの最大容量よりも大きいとき、エラー終了
		SignalSema(pFifo->FifoSema);
		return(1);
	}
	n = pFifo->nInBytes;

	// FIFOキュー内にデータがたまっていない場合は、WAITする。
	if(( m  - n) < nSize)
	{
		pFifo->nSendReqSize = nSize;
		bWait = TRUE;
	}

	SignalSema(pFifo->FifoSema);

	if(bWait==TRUE)
	{
	  WaitSema(pFifo->SendSema);
	}
	WaitSema(pFifo->FifoSema);

	t = pFifo->nQueHead + n;			// 末端インデクスを得る

	if(t > m)
	{
		t -= m;
	}

	// ラップアラウンドしないでコピーできるバイト数を計算
	n = m - t;
	if(nSize <= n)
	{
		// ラップアラウンドなし
		memcpy((char*)&pFifo->pData[t], pData, (size_t)nSize);
	}
	else
	{
		// ラップアラウンドあり
		memcpy((char*)&pFifo->pData[t], pData, (size_t)n);
		memcpy((char*)pFifo->pData, &((const char *)pData)[n], (size_t)(nSize - n));
	}
	pFifo->nInBytes += nSize;					// FIFOパイプ内の要素数を増やす

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(0);
}


//============================================================================
//	FifoPollData											ref: mtfifo.h
//
//	キュー値から値を取得
//	セマフォ待ちなし。FifoSendData()とペアで使用
//
//	FIFO *pFifo : FIFO構造体のポインタ
//	const void *pData : FIFOキューに渡したいデータのポインタ
//	int nSize : FIFOキューに渡したいデータのサイズ
//	int fPeek : 
//
//	戻り値 : u_int : 0:成功/1:エラー(キューが溢れそう)
u_int FifoPollData(FIFO *pFifo, void *pData, int nSize, int fPeek)
{
	int h, m, n;
	
	if(nSize == 0)
	{
		return(0);
	}
	WaitSema(pFifo->FifoSema);
	m = pFifo->nBufSize;
	n = pFifo->nInBytes;
	if(n < nSize)
	{
		// キュー内のデータ量よりも大きいとき、エラー終了
		SignalSema(pFifo->FifoSema);
		return(0);
	}

	// ラップアラウンドしないでコピーできるバイト数を計算
	h = pFifo->nQueHead;		// 先頭インデクスを得る

	n = m - h;
	if(nSize <= n)
	{
		// ラップアラウンドなし
		memcpy(pData,(char*)  &pFifo->pData[h], (size_t)nSize);
		if(!fPeek)
		{
			pFifo->nQueHead = (h + nSize) % m;
			pFifo->nInBytes -= nSize;			// FIFOパイプ内の要素数を減らす
		}
	}
	else
	{
		// ラップアラウンドあり
		memcpy(pData, (char*)  &pFifo->pData[h], (size_t)n);
		memcpy(&((char *)pData)[n], (char*)pFifo->pData, (size_t)(nSize - n));
		if(!fPeek)
		{
			pFifo->nQueHead = nSize - n;
			pFifo->nInBytes -= nSize;			// FIFOパイプ内の要素数を減らす
		}
	}

	CheckWaitRequest(pFifo);
	SignalSema(pFifo->FifoSema);
	return(1);
}

//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	CheckWaitRequest										ref: mtfifo.h
//
//	送信、受信要求サイズが満たされているかどうかを判断し、
//	満たされた場合は、WAIT状態を開放する。
static void CheckWaitRequest(FIFO *pFifo)
{
	if(pFifo->nSendReqSize && ((pFifo->nBufSize - pFifo->nInBytes) >= pFifo->nSendReqSize))
	{
		SignalSema(pFifo->SendSema);
		pFifo->nSendReqSize = 0;
	}
	if(pFifo->nRecvReqSize && ((pFifo->nInBytes >= pFifo->nRecvReqSize)))
	{
		SignalSema(pFifo->RecvSema);
		pFifo->nRecvReqSize = 0;
	}
}

