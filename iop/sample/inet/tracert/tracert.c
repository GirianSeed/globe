/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 * $Id: tracert.c,v 1.2.30.2 2003/10/31 07:27:26 ksh Exp $
 *
 *	tracert
 *
 *                          Version 1.2
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *	                 All Rights Reserved.
 *
 *                          tracert.c
 *
 *        Version       Date            Design     Log
 *  --------------------------------------------------------------------
 *        1.1           2002.07.10      tetsu      ping.c -> tracert.c
 *        1.2           2002.07.11      tetsu      add $gp's save/restore sequence
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <kernel.h>
#include <sys/file.h>

#if defined(REMOVABLE_RESIDENT_END) && !defined(DISABLE_MODULE_UNLOAD)
#define	 ENABLE_MODULE_UNLOAD
#endif

#include <inet/inet.h>
#include <inet/in.h>
#include <inet/ip.h>
#include <inet/icmp.h>
#include <inet/dns.h>

ModuleInfo Module = { "tracert", 0x0101 };

#define DEFAULT_TTY_NUMBER (0)

extern int _gp;

static int thread_priority = USER_LOWEST_PRIORITY;
static int thread_stack = 0x1000; /* 4KB */

static struct common {
    int size;                  // IP ヘッダ、ICMP ヘッダを除いたデータサイズ

    int cid;                   // Connection ID
    int main_evfid;            // プログラムの終了を待つイベントフラグ ID
    int time_evfid;            // common.sys_clock のタイミングでイベント発生するイベントフラグ ID
    int rtid;                  // 受信用スレッド ID
    int stid;                  // 送信用スレッド ID
    int atid;                  // TTY 入力用スレッド ID
#if defined(ENABLE_MODULE_UNLOAD)
    int pwid;                  // イベント待ちスレッド ID
#endif
    u_char *rbuf;              // 受信バッファポインタ
    u_char *sbuf;              // 送信バッファポインタ
    int alarmed;               // 1:アラームハンドラが設定されている, 0:いない
    int tty_num;               // TTY プロトコル番号
    int tty_fd;                // TTY ファイルディスクリプタ

    sceInetAddress_t dstaddr;  // ターゲットホストの内部形式アドレス
    int f_sethost;             // 1:ゲートウェイアドレス設定済み, 0:未設定
    sceInetAddress_t hostaddr; // ゲートウェイの内部形式宛先アドレス
    char tmp[128];             // ゲートウェイのドット形式宛先アドレス
    char hostname[1024];       // ゲートウェイのホスト名
    int bufsiz;                // 送信する IP パケットのデータサイズ
    struct SysClock sys_clock; // 送信間隔(1秒)
    u_short ping_id;           // ICMP メッセージ(タイプ 8(エコー))の識別子
    int n_send;                // 1 つのゲートウェイに対する送信回数
    int n_recv;                // 1 つのゲートウェイに対する受信回数

    int ttl;                   // IP パケットヘッダに指定する TTL の値
    int n_try;                 // 1 つのゲートウェイに対する試行回数(default == 3)
    int max_hops;              // 最大ホップ数
    int no_dns;                // 1:名前解決をしない, 0:する
    int timeout;               // タイムアウト時間(default == 5)
} common;

#define RECV_DONE	(0x01)
#define SEND_DONE	(0x02)
#define ABORT_DONE	(0x04)

void dump_byte(void *ptr, int len){
	u_char *p = ptr;
	int i;

	while(0 < len){
		for(i = 0; i < 16; i++)
			printf(((i < len)? "%02x ": "   "), p[i]);
		for(printf("  "), i = 0; i < 16 && i < len; i++)
			printf("%c", (0x20 <= p[i] && p[i] < 0x7f)? p[i]: '.');
		printf("\n"), p += 16, len -= 16;
	}
}

static int usage(void){
	printf("Usage: tracert [<option>...] <hostname>\n");
	printf("  <option>:\n");
	printf("    thpri=<prio>                     : Set thread priority.\n");
	printf("    thstack=<stack>                  : Set thread stack size.\n");
	printf("    -tty <N>                         : Set tty console number.\n");
	printf("    -h <maximum_hops>(default == 30) : Maximum number of hops to search for target.\n");
	printf("    -d                               : Do not resolve address to hostnames.\n");
	printf("    -w <timeout>(default == 5 sec)   : Wait timeout milliseconds for each replay.\n");
	return(-1);
}

static int scan_number(char *str, int *pv){
	char *s = str;
	int value, v, base = 10;

	if(*s == '0' && *(s + 1) != '\0')
		if(base = 8, *++s == 'x')
			++s, base = 16;
	if(*s == '\0')
		goto err;
	for(value = 0; *s; s++, value = value * base + v){
		if('0' <= *s && *s <= '9')
			v = *s - '0';
		else if('a' <= *s && *s <= 'f')
			v = *s - 'a' + 10;
		else
			goto err;
		if(base <= v)
			goto err;
	}
	*pv = value;
	return(0);
err:	printf("%s: %s - invalid digit\n", __FUNCTION__, str);
	return(-1);
}

static void *alloc_memory(int size){
	void *ptr;
	int old;

	CpuSuspendIntr(&old);
	ptr = AllocSysMemory(0, size, NULL);
	CpuResumeIntr(old);
	if(NULL == ptr)
		printf("tracert: AllocSysMemory(%d) no space\n", size);
	else
		bzero(ptr, size);
	return(ptr);
}

static int create_event_flag(void){
	struct EventFlagParam eparam;
	int r;

	eparam.attr = EA_SINGLE;
	eparam.initPattern = 0;
	eparam.option = 0;
	if(0 >= (r = CreateEventFlag(&eparam)))
		printf("tracert: CreateEventFlag (%d)\n", r);
	return(r);
}

static int start_thread(void (*func)(u_long arg), void *arg){
	struct ThreadParam tparam;
	int tid, r;

	tparam.attr = TH_C;
	tparam.entry = func;
	tparam.initPriority = thread_priority;
	tparam.stackSize = thread_stack;
	tparam.option = 0;
	if(0 >= (tid = CreateThread(&tparam))){
		printf("tracert: CreateThread (%d)\n", tid);
		return(tid);
	}
	if(KE_OK != (r = StartThread(tid, (u_long)arg))){
		printf("tracert: StartThread (%d)\n", r); 
		DeleteThread(tid);
		return(r);
	}
	return(tid);
}

static int open_raw(void){
	sceInetParam_t iparam;
	int r, cid;

	// Connection 生成用パラメータ構造体を初期化(0 クリア)
	bzero(&iparam, sizeof(iparam));

	iparam.type = sceINETT_RAW; // IP パケットを直接扱う
	iparam.local_port = IP_PROTO_ICMP;
	sceInetName2Address(0, &iparam.remote_addr, NULL, 0, 0);
	iparam.remote_port = sceINETP_ANY;

	// Connection 生成
	if(0 > (cid = sceInetCreate(&iparam))){
		printf("tracert: sceInetCreate -> %d\n", cid);
		return(cid);
	}

	// Connection 確立
	if(0 > (r = sceInetOpen(cid, 0))){
		printf("tracert: sceInetOpen -> %d\n", r);
		return(r);
	}

	return(cid);
}

static u_int alarm_handler(void *arg){
	struct common *com = arg;
	int r, gp;

	asm volatile ("move %0,$gp; la $gp,_gp" : "=r" (gp) : : "memory");
	if(KE_OK != (r = iSetEventFlag(com->time_evfid, 0x01)))
		printf("tracert: iSetEventFlag (%d)\n", r);
	asm volatile ("move $gp,%0" : : "r" (gp) : "memory");
	return(com->sys_clock.low);
}

static void release_resources(struct common *com){
	int r, old;

	DelayThread(100 * 1000);
	if(0 < com->cid){
		if(0 > (r = sceInetAbort(com->cid, 0)))
			printf("tracert: sceInetAbort (%d)\n", r);
		DelayThread(100 * 1000);
		if(0 > (r = sceInetClose(com->cid, 0)))
			printf("tracert: sceInetClose (%d)\n", r);
	}
	if(0 < com->rtid){
		(void)TerminateThread(com->rtid);
		if(KE_OK != (r = DeleteThread(com->rtid)))
			printf("tracert: DeleteThread (%d)\n", r);
	}
	if(0 < com->stid){
		(void)TerminateThread(com->stid);
		if(KE_OK != (r = DeleteThread(com->stid)))
			printf("tracert: DeleteThread (%d)\n", r);
	}
	if(0 < com->atid){
		(void)TerminateThread(com->atid);
		if(KE_OK != (r = DeleteThread(com->atid)))
			printf("tracert: DeleteThread (%d)\n", r);
	}
	if(com->alarmed)
		if(KE_OK != (r = CancelAlarm(alarm_handler, com)))
			printf("tracert: CancelAlarm (%d)\n", r);
	if(NULL != com->rbuf){
		CpuSuspendIntr(&old);
		r = FreeSysMemory(com->rbuf);
		CpuResumeIntr(old);
		if(KE_OK != r)
			printf("tracert: FreeSysMemory (%d)\n", r);
	}
	if(NULL != com->sbuf){
		CpuSuspendIntr(&old);
		r = FreeSysMemory(com->sbuf);
		CpuResumeIntr(old);
		if(KE_OK != r)
			printf("tracert: FreeSysMemory (%d)\n", r);
	}
	if(0 < com->main_evfid)
		if(KE_OK != (r = DeleteEventFlag(com->main_evfid)))
			printf("tracert: DeleteEventFlag (%d)\n", r);
	if(0 < com->time_evfid)
		if(KE_OK != (r = DeleteEventFlag(com->time_evfid)))
			printf("tracert: DeleteEventFlag (%d)\n", r);
	if(0 <= com->tty_fd)
		close(com->tty_fd);
}

#ifdef __MWERKS__
u_long sum16asm(u_long v, u_char *p, long len)
{
#else
u_long sum16asm(u_long v, u_char *p, long len);
asm("	.globl sum16asm			");
asm("	.ent sum16asm			");
#endif
asm("sum16asm:				");
asm("	move	$2,$4			");
asm("	lhu	$8,0($5)		");
asm("	srl	$6,$6,4			");
asm("	beq	$0,$6,sum16asm_end	");
asm("	.set	noreorder		");
asm("sum16asm_loop:			");
#ifdef __MWERKS__
asm("	addiu	$5,16			");
asm("	addiu	$6,-1			");
#else
asm("	addu	$5,16			");
asm("	addu	$6,-1			");
#endif
asm("	addu	$2,$8			");
asm("	lhu	$9,0-14($5)		");
asm("	lhu	$10,0-12($5)		");
asm("	lhu	$11,0-10($5)		");
asm("	lhu	$12,0-8($5)		");
asm("	lhu	$13,0-6($5)		");
asm("	lhu	$14,0-4($5)		");
asm("	lhu	$15,0-2($5)		");
asm("	lhu	$8,0($5)		");
asm("	addu	$2,$9			");
asm("	addu	$2,$10			");
asm("	addu	$2,$11			");
asm("	addu	$2,$12			");
asm("	addu	$2,$13			");
asm("	addu	$2,$14			");
asm("	bne	$0,$6,sum16asm_loop	");
asm("	addu	$2,$15			");
asm("	.set	reorder			");
asm("sum16asm_end:			");
#ifdef __MWERKS__
}
#else
asm("	j	$31			");
asm("	.end	sum16asm		");
#endif

u_short sum16(register u_long v, register u_char *p, long len){
	if(((long)p & 1) != 0)
		return(0);
	if(32 <= len){
		switch(((long)p >> 1) & 7){
		case 1: v += *((u_short *)p)++, len -= 2;
		case 2: v += *((u_short *)p)++, len -= 2;
		case 3: v += *((u_short *)p)++, len -= 2;
		case 4: v += *((u_short *)p)++, len -= 2;
		case 5: v += *((u_short *)p)++, len -= 2;
		case 6: v += *((u_short *)p)++, len -= 2;
		case 7: v += *((u_short *)p)++, len -= 2;
		}
		v = sum16asm(v, p, len & ~0xf);
		p += len & ~0xf;
		len &= 0xf;
	}
	for( ; 1 < len; len -= 2)
		v += *((u_short *)p)++;
	if((len & 1) != 0)
		v += *((u_char *)p + 0) << 0;	/* only LITTLE_ENDIAN */
	while(v >> 16)
		v = (u_short)v + (v >> 16);
	return(v);
}

typedef struct record {
	int seq;
	struct SysClock sys_clock;
} RECORD;

#define RECORD_SIZE	(256)
#define RECORD_MASK	(RECORD_SIZE - 1)

static RECORD records[RECORD_SIZE];

static void tracert_recv(u_long arg){
	struct common *com = (void *)arg;
	char tmp[128];
	int r, flags, recv_end_flags = 0;
	IP_HDR *ip = (IP_HDR *)com->rbuf;
	ICMP_HDR *icmp;
	RECORD rec, *rp = &rec, *rp0;
	struct SysClock sys_clock;
	int sec, usec, dms;

	// 内部 IP アドレスからドット形式への変換
	if(0 != sceInetAddress2String(com->tmp, sizeof(com->tmp), &com->dstaddr))
	    strcpy(com->tmp, "???");
	// 内部 IP アドレスからホスト名への変換
	if(0 != sceInetAddress2Name(0, com->hostname, sizeof(com->hostname), &com->dstaddr, 0, 0))
	    strcpy(com->hostname, com->tmp);

	printf("\nTracing route to %s [%s]\n", com->hostname, com->tmp);
	printf("over a maximum of %d hops:\n\n", com->max_hops);

loop:
	// データを受信
	if(0 > (r = sceInetRecv(com->cid, com->rbuf, com->bufsiz, &flags, -1)))
		goto done;
	GetSystemTime(&sys_clock);

#if 0
	printf("tracert: sceInetRecv %d -> %d\n", com->bufsiz, r);
	if(0 < r){
		dump_byte(com->rbuf, r);
		printf("\n");
	}
#endif

	// ICMP ヘッダのポインタを指定
	icmp = (ICMP_HDR *)(ip + 1);

	// 受信データのサイズを確認
	if(r < sizeof(*ip) + sizeof(*icmp))
		goto loop;

	// エンディアン変換
	ip_ntoh(ip);

	// ICMP メッセージであることを確認
	if(ip->proto != IP_PROTO_ICMP)
		goto loop;

	// エンディアン変換
	icmp_ntoh(icmp);

	switch(icmp->type){
	case ICMP_TYPE_ECHO_REPLY:
	    {
		// 識別子を比較
		if(icmp->id != com->ping_id) goto loop;
		// 終了フラグを立てる
		recv_end_flags = 1;
	    }
	    break;
	case ICMP_TYPE_TIME_EXCEEDED:
	    {
		switch(icmp->code){
		case ICMP_CODE_TTL_EXCEEDED:
		    {
			// ICMP ヘッダのポインタを差し替える
			IP_HDR *ip2 = (IP_HDR *)(icmp + 1);
			icmp = (ICMP_HDR *)(ip2 + 1);
			icmp_ntoh(icmp);
		    }
		    break;
		case ICMP_CODE_FRT_EXCEEDED:
		default:
		    goto loop;
		}
	    }
	    break;
	default:
	    goto loop;
	}

	// タイムアウト設定をキャンセル
	if(com->alarmed){
	    if(KE_OK != (r = CancelAlarm(alarm_handler, com)))
		printf("tracert: CancelAlarm (%d)\n", r);
	    com->alarmed = 0;
	}

	// 受信回数をカウント
	++com->n_recv;

	// 送信から受信までの時間を計算
	bcopy(rp0 = &records[icmp->seq & RECORD_MASK], rp, sizeof(*rp));
	if(rp->seq == icmp->seq){
	    sys_clock.hi -= rp->sys_clock.hi;
	    if(sys_clock.low < rp->sys_clock.low)
		--sys_clock.hi;
	    sys_clock.low -= rp->sys_clock.low;
	    SysClock2USec(&sys_clock, &sec, &usec);
	    usec += sec * 1000000;
	    dms = (usec + 50) / 100;
	}else{
	    dms = -1;
	}

	// 経路情報を表示
	bcopy(&ip->src, com->hostaddr.data, sizeof(ip->src)); // ゲートウェイのアドレスを取得
	if(com->n_recv == com->n_try){ // 最後のデータ
	    if(sceInetAddress2String(tmp, sizeof(tmp), &com->hostaddr) != 0){
		if(com->f_sethost == 0){
		    printf(" %4d.%d ms\n", dms / 10, dms % 10);
		}else{
		    if(com->no_dns){ // 名前解決を行わない
			printf(" %4d.%d ms %s\n", dms / 10, dms % 10, com->tmp);
		    }else{ // 名前解決を行う
			if(0 != sceInetAddress2Name(0, com->hostname, sizeof(com->hostname), &com->hostaddr, 0, 0))
			    strcpy(com->hostname, com->tmp);
			printf(" %4d.%d ms %s[%s]\n", dms / 10, dms % 10, com->hostname, com->tmp);
		    }
		}
	    }else{
		if(com->f_sethost == 0){
		    strcpy(com->tmp, tmp);
		    com->f_sethost = 1;
		    if(com->no_dns){ // 名前解決を行わない
			printf(" %4d.%d ms %s\n", dms / 10, dms % 10, com->tmp);
		    }else{ // 名前解決を行う
			if(0 != sceInetAddress2Name(0, com->hostname, sizeof(com->hostname), &com->hostaddr, 0, 0))
			    strcpy(com->hostname, com->tmp);
			printf(" %4d.%d ms %s[%s]\n", dms / 10, dms % 10, com->hostname, com->tmp);
		    }
		}else{
		    if(strcmp(com->tmp, tmp) == 0){
			if(com->no_dns){ // 名前解決を行わない
			    printf(" %4d.%d ms %s\n", dms / 10, dms % 10, com->tmp);
			}else{ // 名前解決を行う
			    if(0 != sceInetAddress2Name(0, com->hostname, sizeof(com->hostname), &com->hostaddr, 0, 0))
				strcpy(com->hostname, com->tmp);
			    printf(" %4d.%d ms %s[%s]\n", dms / 10, dms % 10, com->hostname, com->tmp);
			}
		    }else{
			--com->n_recv;
		    }
		}
	    }
	}else{
	    if(com->n_recv == 1){ // 最初のデータ
		if(sceInetAddress2String(tmp, sizeof(tmp), &com->hostaddr) == 0){
		    strcpy(com->tmp, tmp);
		    com->f_sethost = 1;
		}
		printf(" %d %4d.%d ms", com->ttl, dms / 10, dms % 10);
	    }else{ // 2 番目から最後の 1 つ手前までのデータ
		if(sceInetAddress2String(tmp, sizeof(tmp), &com->hostaddr) != 0){
		    printf(" %4d.%d ms", dms / 10, dms % 10);
		}else{
		    if(com->f_sethost == 0){
			strcpy(com->tmp, tmp);
			com->f_sethost = 1;
			printf(" %4d.%d ms", dms / 10, dms % 10);
		    }else{
			if(strcmp(com->tmp, tmp) == 0)
			    printf(" %4d.%d ms", dms / 10, dms % 10);
			else
			    --com->n_recv;
		    }
		}
	    }
	}

	// 送信待ちを解除
	if(KE_OK != (r = SetEventFlag(com->time_evfid, 0x01)))
	    printf("tracert: iSetEventFlag (%d)\n", r);

	// ループ判定
	if(recv_end_flags && com->n_try <= com->n_recv)
	    goto done;
	else
	    goto loop;

done:	SetEventFlag(com->main_evfid, RECV_DONE);
	while(1)
		SleepThread();
}

static void tracert_send(u_long arg){
	struct common *com = (void *)arg;
	int r, seq = 0, flags = 0;
	u_long result;
	IP_HDR *ip = (IP_HDR *)com->sbuf;
	ICMP_HDR *icmp = (ICMP_HDR *)(ip + 1);
	RECORD *rp;

	while(com->ttl <= com->max_hops){
	    bcopy(com->dstaddr.data, &ip->dst, sizeof(ip->dst)); // 宛先アドレス
	    ip->proto = IP_PROTO_ICMP;                           // プロトコル
	    ip->ttl = com->ttl;                                  // 生存時間(TTL)

	    // エンディアン変換
	    ip_hton(ip);

	    icmp->type = ICMP_TYPE_ECHO_REQUEST; // タイプ
	    icmp->code = 0;                      // コード
	    icmp->id = com->ping_id;             // 識別子
	    icmp->seq = seq;                     // 順次番号

	    // エンディアン変換
	    icmp_hton(icmp);

	    // チェックサムの計算
	    icmp->chksum = ~sum16(icmp->chksum = 0, (u_char *)icmp, sizeof(*icmp) + com->size);

	    // エコーメッセージを送信
	    if(0 > (r = sceInetSend(com->cid, com->sbuf, com->bufsiz, &flags, 0)))
		goto done;

	    // 送信回数をカウントアップ
	    ++com->n_send;

	    // 送信した ICMP Echo Request の順序番号と送信時間を記録
	    // ICMP Echo Request の順序番号をカウントアップ
	    rp = &records[seq & RECORD_MASK];
	    rp->seq = seq;
	    GetSystemTime(&rp->sys_clock);
	    seq++;

#if 0
	    printf("tracert: sceInetSend %d -> %d\n", com->bufsiz, r);
	    if(0 < r){
		dump_byte(com->sbuf, r);
		printf("\n");
	    }
#endif

	    // com->timeout 秒毎に alarm_handler() が呼び出されるように設定
	    if(KE_OK != (r = SetAlarm(&com->sys_clock, alarm_handler, com))){
		printf("tracert: SetAlarm (%d)\n", r);
	    }
	    com->alarmed = 1;

	    // タイムアウトか受信完了までイベント待ち
	    if(KE_OK != (r = WaitEventFlag(com->time_evfid, 0x01, EW_OR | EW_CLEAR, &result)))
		printf("tracert: WaitEventFlag (%d)\n", r);

	    // タイムアウト設定をキャンセル
	    if(com->alarmed){
		if(KE_OK != (r = CancelAlarm(alarm_handler, com)))
		    printf("tracert: CancelAlarm (%d)\n", r);
		com->alarmed = 0;
	    }

	    // タイムアウト時の出力処理
	    if(com->n_send != com->n_recv){
		if(com->n_send == 1){
		    printf(" %d      *   ", com->ttl);
		}else{
		    if(com->n_send == com->n_try){
			if(com->f_sethost == 0){
			    printf("      *   \n");
			}else{
			    if(com->no_dns){
				printf("      *    %s\n", com->tmp);
			    }else{
				if(0 != sceInetAddress2Name(0, com->hostname, sizeof(com->hostname), &com->hostaddr, 0, 0))
				    strcpy(com->hostname, com->tmp);
				printf("      *    %s[%s]\n", com->hostname, com->tmp);
			    }
			}
		    }else{
			printf("      *   ");
		    }
		}
		com->n_recv++;
	    }

	    // 1 つのゲートウェイに対する試行回数以上になったら TTL をカウントアップ
	    // 及び、受信、送信のパケットカウンターをクリア
	    if(com->n_try <= com->n_send){
		com->f_sethost = 0;
		com->n_recv = 0;
		com->n_send = 0;
		com->ttl++;
	    }
	}

done:	SetEventFlag(com->main_evfid, SEND_DONE);
	while(1)
		SleepThread();
}

static void tracert_abort(u_long arg){
	struct common *com = (void *)arg;
	int r;
	u_char ch;
	char tmp[10];

	sprintf(tmp, "tty%d:", com->tty_num);
	if(0 > (com->tty_fd = open(tmp, O_RDONLY))){
		printf("tracert: can not open %s\n", tmp);
		return;
	}
	while(0 < (r = read(com->tty_fd, &ch, sizeof(ch)))){
	    if(ch == 0x03) break;
	}
	close(com->tty_fd);
	com->tty_fd = -1;
	SetEventFlag(com->main_evfid, ABORT_DONE);
	while(1)
		SleepThread();
}

static int tracert_start(struct common *com, int ac, char *av[]){
	int r, i;
	char *bp;

	com->size      = 56; // IP ヘッダ、ICMP ヘッダを除いたデータサイズ
	com->ttl       = 1;  // IP パケットヘッダに指定する TTL の初期値
	com->n_try     = 3;  // 1 つのゲートウェイに対する試行回数(default == 3)
	com->max_hops  = 30; // 最大ホップ数(default == 30)
	com->no_dns    = 0;  // 0:名前解決を行う(default),1:行わない
	com->timeout   = 5;  // タイムアウト時間(5 sec(default))

	for(--ac, ++av; 0 < ac && *av[0] == '-'; --ac, ++av)
		if(!strncmp("thpri=", av[0], 6)){
			bp = av[0] + 6;
			if(!isdigit(*bp))
				return(usage());
			thread_priority = atoi(bp);
			if(thread_priority < USER_HIGHEST_PRIORITY
				    || USER_LOWEST_PRIORITY < thread_priority)
				return(usage());
			for( ; *bp != '\0' && isdigit(*bp); bp++)
				;
			if(*bp != '\0')
				return(usage());
		}else if(!strncmp("thstack=", av[0], 8)){
			bp = av[0] + 8;
			if(!isdigit(*bp))
				return(usage());
			thread_stack = atoi(bp);
			for( ; *bp != '\0' && isdigit(*bp); bp++)
				;
			if(!strcmp(bp, "KB"))
				thread_stack <<= 10;
			else if(*bp != '\0')
				return(usage());
		}else if(!strcmp("-help", av[0])){
			return(usage());
		}else if(!strcmp("-?", av[0])){
			return(usage());
		}else if(!strcmp("-d", av[0])){
			com->no_dns = 1;
		}else if(!strcmp("-tty", av[0])){
			if(++av, --ac < 1)
				return(usage());
			if(scan_number(av[0], &com->tty_num))
				return(-1);
			if(com->tty_num < 0 || 9 < com->tty_num){
				printf("tracert: illegal tty number.\n");
				return(-1);
			}
		}else if(!strcmp("-h", av[0])){
			if(++av, --ac < 1)
				return(usage());
			if(scan_number(av[0], &com->max_hops))
				return(-1);
		}else if(!strcmp("-w", av[0])){
			if(++av, --ac < 1)
				return(usage());
			if(scan_number(av[0], &com->timeout))
				return(-1);
		}else
			return(usage());
	if(ac != 1)
		return(usage());

	// ホスト名またはドット形式から内部形式 IP アドレスへの変換を行う
	// RES_TIMEOUT: 6 * 1000 ms(6秒)(dns.h に定義)
	// RES_RETRY: 4 回(dns.h に定義)
	if(0 > (r = sceInetName2Address(0, &com->dstaddr, av[0],
			RES_TIMEOUT, RES_RETRY))){
		printf("sceInetName2Address(%s) -> %d\n", av[0], r);
		return(r);
	}

	if(0 > (com->time_evfid = create_event_flag()))
		return(-1);

	// 1 秒(1000 * 1000 us) をシステムクロック値に変換
	USec2SysClock(com->timeout * 1000 * 1000, &com->sys_clock);

	// Connection の生成と確立を行う
	if(0 >= (com->cid = open_raw()))
		return(-1);

	// 送信する IP パケットのデータサイズを算出
	// IP_HDR: ip.h に定義
	// ICMP_HDR: icmp.h に定義
	com->bufsiz = sizeof(IP_HDR) + sizeof(ICMP_HDR) + com->size;

	// 記録を保持しておく領域(records)を初期化(seq に -1 を代入)
	for(i = 0; i < RECORD_SIZE; i++)
		records[i].seq = -1;

	if(0 > (com->main_evfid = create_event_flag()))
		return(-1);

	// エコー応答メッセージを受信するバッファを確保
	if(NULL == (com->rbuf = alloc_memory(64 * 1024))) /* 64 KB */
		return(-1);
	// 受信用スレッドを起動
	if(0 >= (com->rtid = start_thread(tracert_recv, com)))
		return(-1);

	// 送信するエコーメッセージ分のバッファを確保
	if(NULL == (com->sbuf = alloc_memory(com->bufsiz)))
		return(-1);
	// 送信用スレッドを起動
	if(0 >= (com->stid = start_thread(tracert_send, com)))
		return(-1);

	// ICMP メッセージ(タイプ 8(エコー))の識別子を生成
	com->ping_id = (u_short)((com->stid >> 16) + com->stid);

	// TTY 入力用スレッドを起動
	if(0 >= (com->atid = start_thread(tracert_abort, com)))
		return(-1);

	return(0);
}

#if defined(ENABLE_MODULE_UNLOAD)
static int unloadable(void){
	int dummy, mcount = -1;

	GetModuleIdList(&dummy, 1, &mcount);
	return((mcount < 0)? 0: 1);
}
#endif

static void tracert_wait(u_long arg){
	struct common *com = (void *)arg;
	u_long result, state = 0;
	int r;

	while(1){
		if(KE_OK != (r = WaitEventFlag(com->main_evfid,
				RECV_DONE | SEND_DONE | ABORT_DONE,
				EW_OR | EW_CLEAR, &result))){
			printf("tracert: WaitEventFlag (%d)\n", r);
			break;
		}
		state |= result;
		if(state & ABORT_DONE){
			printf("\ntracert: abort.\n");
			break;
		}
		if(state & RECV_DONE){
			printf("\ntracert: complete.\n");
			break;
		}
		if(state & SEND_DONE){
			printf("\ntracert: max hops == %d\n", com->max_hops);
			break;
		}
		break;
	}
#if defined(ENABLE_MODULE_UNLOAD)
	if(unloadable()){
		SelfStopModule(0, NULL, NULL);
		SelfUnloadModule();
		/* never returns here */
	}
#endif
	release_resources(com);
//	ExitDeleteThread();	<- NOT YET SUPPORTED
	ExitThread();
}

int start(int ac, char *av[]){
	struct common *com = &common;
	int r;

#if defined(ENABLE_MODULE_UNLOAD)
	if(ac < 0){
		if(!strcmp("other", av[0])){
			(void)TerminateThread(com->pwid);
			if(KE_OK != (r = DeleteThread(com->pwid)))
				printf("tracert: DeleteThread (%d)\n", r);
		}else if(!strcmp("self", av[0]))
			;
		release_resources(com);
		return(NO_RESIDENT_END);
	}
#endif
	bzero(com, sizeof(*com));
	com->tty_num = DEFAULT_TTY_NUMBER;
	com->tty_fd = -1;
	if(0 != (r = tracert_start(com, ac, av)))
		return((r << 4) | NO_RESIDENT_END);

	// イベント待ちスレッド起動
#if defined(ENABLE_MODULE_UNLOAD)
	if(0 >= (com->pwid = start_thread(tracert_wait, com))){
#else
	if(0 >= start_thread(tracert_wait, com)){
#endif
		release_resources(com);
		return(NO_RESIDENT_END);
	}
#if defined(ENABLE_MODULE_UNLOAD)
	if(unloadable())
		return(REMOVABLE_RESIDENT_END);
#endif
	return(RESIDENT_END);
}
