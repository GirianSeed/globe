/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/*
 *                     I/O Processor Library 
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        kerror.h
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.4.0
 */

enum KernelErrorCode {
  KE_OK                  =    0, /* no error                               */
  KE_ERROR               =   -1, /* error                                  */
  KE_ILLEGAL_EXPCODE     =  -50, /* illegal exception code                 */
  KE_EXPHANDLER_NOUSE    =  -51, /* exception handler not use              */
  KE_EXPHANDLER_USED     =  -52, /* exception handler already used         */
  KE_ILLEGAL_CONTEXT     = -100, /* call from interrupt_handler/thread     */
  KE_ILLEGAL_INTRCODE    = -101, /* illegal INTRCODE                       */
  KE_CPUDI               = -102, /* CPU already interrupt disable          */
  KE_INTRDISABLE         = -103, /* INTRCODE already interrupt disable     */
  KE_FOUND_HANDLER       = -104, /* Hnadler already exist                  */
  KE_NOTFOUND_HANDLER    = -105, /* Handler not found                      */
  KE_NO_TIMER            = -150, /* ハードウエアタイマは取得できなかった   */
  KE_ILLEGAL_TIMERID     = -151, /* ハードウエアタイマのIDが不正           */
  KE_NO_MEMORY           = -400, /* no memory                              */
  KE_ILLEGAL_ATTR        = -401, /* illegal attr parameter                 */
  KE_ILLEGAL_ENTRY       = -402, /* illegal tread entry address            */
  KE_ILLEGAL_PRIORITY    = -403, /* illegal priority value                 */
  KE_ILLEGAL_STACK_SIZE  = -404, /* illegal stack size                     */
  KE_ILLEGAL_MODE        = -405, /* illegal mode                           */
  KE_ILLEGAL_THID        = -406, /* illegal thread ID                      */
  KE_UNKNOWN_THID        = -407, /* not found thread                       */
  KE_UNKNOWN_SEMID       = -408, /* not found semaphore                    */
  KE_UNKNOWN_EVFID       = -409, /* not found eventflag                    */
  KE_UNKNOWN_MBXID       = -410, /* not found messagebox                   */
  KE_UNKNOWN_VPLID       = -411, /* not found Vpool                        */
  KE_UNKNOWN_FPLID       = -412, /* not found Fpool                        */
  KE_DORMANT             = -413, /* thread already DORMANT                 */
  KE_NOT_DORMANT         = -414, /* thread is not DORMANT                  */
  KE_NOT_SUSPEND         = -415, /* thread is not SUSPEND                  */
  KE_NOT_WAIT            = -416, /* thread is not WAIT                     */
  KE_CAN_NOT_WAIT        = -417, /* now dispatch disabled                  */
  KE_RELEASE_WAIT        = -418, /* WAIT status release                    */
  KE_SEMA_ZERO           = -419, /* セマフォの資源が獲得できない           */
  KE_EVF_COND            = -420, /* イベントフラグの待ち条件が不成立だった */
};
