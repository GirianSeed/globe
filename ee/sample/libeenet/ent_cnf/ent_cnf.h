/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/* 
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <ent_cnf - ent_cnf.h>
 *       <header for user-level iop handling functions for eenet>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Nov,17,2001     komaki      first version
 */

#ifndef _ENT_CNF_H_
#define _ENT_CNF_H_

#ifdef __cplusplus
extern "C" {
#endif

int ent_cnf_init(void);
int ent_cnf_load_config(const char *fname, const char *usr_name);
int ent_cnf_set_config(void);
int ent_cnf_set_sifcmdbuffer(int sifcmd_num);

#ifdef __cplusplus
}
#endif

#endif /* _ENT_CNF_H_ */
