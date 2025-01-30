/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *             Emotion Engine  Library Sample Program
 *
 *                         - <cache> -
 *
 *                         Version <1.00>
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <cache.c>
 *                     <cache instruction sample>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      1.00            Mar,20,2001     kumagae     first ci
 *      1.01            Apr,29,2003     hana        change coding style
 */

#include <eekernel.h>
#include <stdio.h>
#include <assert.h>

#define DCACHE_LINE_SIZE   0x40

#define TAGLO_PFN_MASK    0xfffff000
#define TAGLO_VALID_MASK  (0x1<<5)
#define TAGLO_LOCK_MASK   (0x1<<3)
#define TAGLO_DIRTY_MASK  (0x1<<6)


/*
  LockDCache  キャッシュラインをロックする
  引数
    begin:ロックする領域の開始物理アドレス(64byte align)
    end  :ロックする領域の最終物理アドレス(64byte align)
  返値
     0:ロック成功
    -1:該当データキャッシュラインはすでにロックされていたものがある。
*/
int LockDCache(void *begin, void *end)
{
	int t0, t1, i, ret = 0;
	int index;
	u_int Begin = (u_int)begin;
	u_int End = (u_int)end;

	assert((Begin & 0x3f) == 0);
	assert((End & 0x3f) == 0);
	/*
	   両方のキャッシュラインをロックすることはできない。
	 */
	assert((End - Begin) <= 4096);

	DI();
	for (i = (Begin & 0xffffffc0); i <= End; i += DCACHE_LINE_SIZE) {

		/*
		   Data Cacheに載っていないものは、ロックできないため
		   先にキャッシュ上に載せておく。
		 */
		__asm__ volatile ("pref 0, 0(%0)" : : "r"(i));

		index = i & 0xfc0;

		/*エントリがすでにロック済かどうかを確認する。 */
		__asm__ volatile (
			"sync.l\n"
			"cache 0x10, 0(%2)\n"
			"sync.l\n"
			"mfc0  %0, $28\n"

			"sync.l\n"
			"cache 0x10, 1(%2)\n"
			"sync.l\n"
			"mfc0  %1, $28\n"
			
			: "=r"(t0), "=r"(t1) : "r"(index)
		);
		if ((t0 & TAGLO_LOCK_MASK) || (t1 & TAGLO_LOCK_MASK)) {
			ret = -1;
		} else {
			if ((t0 & TAGLO_PFN_MASK) == (i & TAGLO_PFN_MASK)) {
				/* ロックするときにDirty bitを1にしないとメモりに書き戻されない */
				t0 |= (TAGLO_LOCK_MASK | TAGLO_DIRTY_MASK);
				
				__asm__ volatile (
					"mtc0 %0, $28\n"
					"sync.l\n"
					"cache 0x12, 0(%1)\n"
					"sync.l\n"
					: : "r"(t0), "r"(index)
				);
			} else {
				/* ロックするときにDirty bitを1にしないとメモりに書き戻されない */
				t1 |= (TAGLO_LOCK_MASK | TAGLO_DIRTY_MASK);
				
				__asm__ volatile (
					"mtc0 %0, $28\n"
					"sync.l\n"
					"cache 0x12, 1(%0)\n"
					"sync.l\n"
					: : "r"(t1), "r"(index)
				);
			}
		}
	}
	EI();

	if (ret < 0) {
		printf("Lock fail\n");
	} else {
		printf("Lock success\n");
	}
	return ret;

}



/*
  引数のアドレスを含むD Cacheのラインの状態を取得する。
  戻り値
    0:キャッシュ上にない
    !=0:TagLoを返す
 */
int IsOnDCache(unsigned int addr)
{
	int ret = 0;
	int index = addr & 0xfc0;

	assert((addr & 0xf0000000) == 0);

	DI();
	__asm__ volatile (
		"sync.l\n"
		"cache   0x10, 0(%1)\n"
		"sync.l\n"
		"mfc0    %0, $28\n"
		: "=r"(ret) : "r"(index)
	);

	if (((ret & TAGLO_PFN_MASK) == (addr & TAGLO_PFN_MASK)) && (ret & TAGLO_VALID_MASK)) {
		EI();
		return ret;
	}

	__asm__ volatile (
		"sync.l\n"
		"cache   0x10, 1(%1)\n"
		"sync.l\n"
		"mfc0    %0, $28\n"
		: "=r"(ret) : "r"(index)
	);

	EI();

	if (((ret & TAGLO_PFN_MASK) == (addr & TAGLO_PFN_MASK)) && (ret & TAGLO_VALID_MASK))
		return ret;

	return 0;
}

/*
  PrintDCacheStatus 
  指定アドレスを含む、Dキャッシュラインの状態を表示する。
 */
void PrintDCacheLineStatus(void *addr)
{
	int taglo = IsOnDCache((u_int)addr);

	if (taglo) {
		printf("%08x: taglo %08x Valid %d Dirty %d Lock %d LRF %d\n",
				(u_int)addr, taglo & 0xfffff000, (taglo >> 5) & 1, (taglo >> 6) & 1,
				(taglo >> 3) & 1, (taglo >> 4) & 1);
	} else {
		printf("%08x: not on DCache\n", (u_int)addr);
	}
}


volatile int data[64] __attribute__ ((aligned(64)));

int main(void)
{

	PrintDCacheLineStatus((void *)data);
	/*
	   Dキャッシュラインをロックします。
	   キャッシュラインをロックしたとき、
	   ロックしていないもうひとつのWAYへの
	   キャッシュミスは、ブロッキングモードになります。
	 */
	LockDCache((void *)data, (void *)data);

	data[0] = 0x01234567;

	PrintDCacheLineStatus((void *)data);

	/*Lockしていたキャッシュラインをメモりに書き戻し、ロックを解除します。 */
	FlushCache(WRITEBACK_DCACHE);

	/*
	   Lockしていたデータは、メモりに反映されています。
	 */
	printf("data[0] = %x\n", (volatile int)data[0]);


	/*
	   再びDキャッシュラインをロックします。

	   キャッシュラインをロックしたとき、
	   ロックしていないもうひとつのWAYへの
	   キャッシュミスは、ブロッキングモードになります。
	 */
	LockDCache((void *)data, (void *)data);

	data[1] = 0x01234567;

	PrintDCacheLineStatus((void *)data);

	/*Lockしていたキャッシュラインを無効化し、ロックを解除します。 */
	InvalidDCache((void *)data, (void *)data);

	PrintDCacheLineStatus((void *)data);

	/*
	   Lockしていたデータは、無効化されてしまい、メモリに反映されません。
	 */

	printf("data[1] = %x\n", (volatile int)data[1]);

	return 0;
}

