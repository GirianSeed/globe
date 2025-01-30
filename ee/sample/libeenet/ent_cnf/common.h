/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <ent_cnf - common.h>
 *       <header for user-level iop handling functions for eenet>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Nov,17,2001     komaki      first version
 */

/* ent_cnf sifrpc num */
#define ENT_CNF_SIFRPC_NUM 0x0a31108e

/* code */
#define ENT_CNF_SIFRPC_LOAD_CONFIG      1
#define ENT_CNF_SIFRPC_SET_CONFIG       2
#define ENT_CNF_SIFRPC_SET_CONFIG_ADDR  3

/* utility macro */
#define ee_rpc_size(size) ((size + 15) & ~15)
#define iop_rpc_size(size) ((size + 3) & ~3)

