/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *              I/O Processor Library Sample Program
 * 
 *                         - hello -
 * 
 * 
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 * 
 *                            hello.c
 * 
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.4.0            
 *      1.6.0          Apr,4,2000       tei         change main() to start()
 */

#include <stdio.h>
#include <kernel.h>

int start(int argc, char *argv[])
{
    int i;

    printf("Hello ! \n");
    for( i = 0; i < argc ; i ++ )
	printf("  argv[%d] = %s\n",i, argv[i]);
    return NO_RESIDENT_END;
}

/* Note:
  Programming with the main function is also possible as shown below.
  However this method is not generally used for IOP applications.
  Normally, the start function or other function (when the activation entry
  is specified explicitly) is executed instead of the main function then the 
  minimum required initialization is performed.  The value that represents the 
  resident termination should be returned.  (See ../kernel/module/mylib.c )

  A program that is executed from the main() function can be created with the
  -startfiles option.  With this option, it is notified a compiler that the
  crt0.o object including the start function is linked explicitly.
  (Replace the 'LOADLIBES=' line with 'LOADLIBES= -startfiles' in ./Makefile.
   For operations of the standard crt0.o, refer to the source file crt0.o /usr/
   local/sce/iop/install/lib/crt0.c.)


      int main(int argc, char *argv[])
      {
          int i;

	  printf("Hello ! \n");
	  for( i = 0; i < argc ; i ++ )
	      printf("  argv[%d] = %s\n",i, argv[i]);
	  return 0;
      }

*/

/* 補足:

  下記のように main関数を使ってプログラムすることも可能です。
  ただし、main関数を持ったプログラムは IOP のアプリケーションの一般的な作法では
  ないことに御注意下さい。
  IOP のアプリケーションの一般的な作法では、main関数を使わずに start関数または
  任意の関数（リンク時に起動エントリを明示的に指定した場合）から実行を開始し、
  その関数にて必要最小限の初期化を行ったのち、常駐終了を表す戻り値を返すこと
  になっています。(../kernel/module/mylib.c を参照してください。)

  下記の様に main() 関数から実行を開始するプログラムは、start関数を含む crt0.o
  というオブジェクトを明示的にリンクすることをコンパイラに知らせる -startfiles
  オプションを使用することで作成することができます。
  (./Makefile の 'LOADLIBES=' の行を 'LOADLIBES= -startfiles' と書き換えます。)
  標準で用意されている crt0.o の動作はそのソースファイル
  /usr/local/sce/iop/install/lib/crt0.c をご覧になれば確認できます。


      int main(int argc, char *argv[])
      {
          int i;

	  printf("Hello ! \n");
	  for( i = 0; i < argc ; i ++ )
	      printf("  argv[%d] = %s\n",i, argv[i]);
	  return 0;
      }

*/

