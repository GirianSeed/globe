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

/* �⑫:

  ���L�̂悤�� main�֐����g���ăv���O�������邱�Ƃ��\�ł��B
  �������Amain�֐����������v���O������ IOP �̃A�v���P�[�V�����̈�ʓI�ȍ�@�ł�
  �Ȃ����ƂɌ䒍�Ӊ������B
  IOP �̃A�v���P�[�V�����̈�ʓI�ȍ�@�ł́Amain�֐����g�킸�� start�֐��܂���
  �C�ӂ̊֐��i�����N���ɋN���G���g���𖾎��I�Ɏw�肵���ꍇ�j������s���J�n���A
  ���̊֐��ɂĕK�v�ŏ����̏��������s�����̂��A�풓�I����\���߂�l��Ԃ�����
  �ɂȂ��Ă��܂��B(../kernel/module/mylib.c ���Q�Ƃ��Ă��������B)

  ���L�̗l�� main() �֐�������s���J�n����v���O�����́Astart�֐����܂� crt0.o
  �Ƃ����I�u�W�F�N�g�𖾎��I�Ƀ����N���邱�Ƃ��R���p�C���ɒm�点�� -startfiles
  �I�v�V�������g�p���邱�Ƃō쐬���邱�Ƃ��ł��܂��B
  (./Makefile �� 'LOADLIBES=' �̍s�� 'LOADLIBES= -startfiles' �Ə��������܂��B)
  �W���ŗp�ӂ���Ă��� crt0.o �̓���͂��̃\�[�X�t�@�C��
  /usr/local/sce/iop/install/lib/crt0.c �������ɂȂ�Ίm�F�ł��܂��B


      int main(int argc, char *argv[])
      {
          int i;

	  printf("Hello ! \n");
	  for( i = 0; i < argc ; i ++ )
	      printf("  argv[%d] = %s\n",i, argv[i]);
	  return 0;
      }

*/

