/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *	  Netcnf Interface Library Sample Program
 *
 *						 Version 1.9
 *						 Shift-JIS
 *
 *	  Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *						All Rights Reserved.
 *
 *						 load_entry.c
 *
 *	   Version   Date		 Design   Log
 *  --------------------------------------------------------------------
 *	   1.1	   2002.01.28   tetsu	First version
 *	   1.2	   2002.01.31   tetsu	Correspond to libnet
 *	   1.3	   2002.01.31   tetsu	Remove get_list()
 *	   1.4	   2002.01.31   tetsu	Change "Combination1"'s definition
 *	   1.5	   2002.02.13   tetsu	Correspond to pppoe connection(up)
 *	   1.6	   2002.02.13   tetsu	Correspond to pppoe connection(down)
 *	   1.7	   2002.02.15   tetsu	Add sceLibnetWaitGetInterfaceID()
 *	   1.8	   2002.04.10   tetsu	Modified
 *                                  sceLibnetWaitGetInterfaceID()'s Arg
 *                                  sceLibnetWaitGetAddress()'s Argument
 *	   1.9	   2003.04.30   ksh	    decode on
 */

#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <libnet.h>
#include <libmrpc.h>

#include "load_module.h"

#define WORKAREA_SIZE (0x1000)

#define FNAME "mc0:/BWNETCNF/BWNETCNF"
#define NET_USR_NAME "Combination1"

static sceNetcnfifArg_t if_arg;

static int load_entry(char *fname, int type, char *usr_name, sceNetcnfifData_t *data)
{
	FlushCache(0);
	sceNetcnfifLoadEntry(fname, type, usr_name, data);
	while (sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0) {
		scePrintf("type(%d):sceNetcnfifLoadEntry (%d)\n", type, if_arg.data);
		return (if_arg.data);
	}
	sceNetcnfifDataDump(data);
	return (0);
}

static int set_env(int type, sceNetcnfifData_t *data)
{
	int addr, id;

	sceNetcnfifGetAddr();
	while (sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	addr = if_arg.addr;

	id = sceNetcnfifSendIOP((unsigned int)data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
	while (sceNetcnfifDmaCheck(id));

	sceNetcnfifSetEnv(type);
	while (sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0){
		scePrintf("type(%d):sceNetcnfifSetEnv (%d)\n", type, if_arg.data);
		return (0);
	}
	return (if_arg.addr);
}

int main(void)
{
	static sceNetcnfifData_t data __attribute__((aligned(64)));
	static sceSifMClientData cd;
	static u_char net_buf[2048] __attribute__((aligned(64)));
	static int if_id[sceLIBNET_MAX_INTERFACE];
	static sceInetAddress_t myaddr;
	static char myptrname[128];
	int r, addr;

	sceSifInitRpc(0);
	while (!sceSifRebootIop(IOPRP));
	while (!sceSifSyncIop());
	sceSifInitRpc(0);
	sceSifLoadFileReset();
	sceFsReset();

	load_module();

	sceNetcnfifInit();
	sceMcInit();
	sceSifMInitRpc(0);

	if ((r = sceLibnetInitialize(&cd, sizeof(net_buf), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY)) < 0) {
		scePrintf( "sceLibnetInitialize (%d)\n", r);
		goto done;
	}
	if ((r = sceLibnetRegisterHandler(&cd, net_buf)) < 0) {
		scePrintf( "sceLibnetRegisterHandler (%d)\n", r);
		goto done;
	}

	sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
	while (sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0){
		scePrintf("sceNetcnfifAllocWorkarea (%d)\n", if_arg.data);
		goto done;
	}

	sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_off);

	sceNetcnfifDataInit(&data);
	if (load_entry(FNAME, sceNetcnfifArg_type_net, NET_USR_NAME, &data) < 0) {
		goto done;
	}

	if ((addr = set_env(sceNetcnfifArg_type_net, &data)) == 0) {
		goto done;
	}

	if ((r = sceLibnetSetConfiguration(&cd, net_buf, addr)) < 0){
		scePrintf( "sceLibnetSetConfiguration (%d)\n", r);
		goto done;
	}

	if ((r = sceLibnetWaitGetInterfaceID(&cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE)) < 0) {
		scePrintf( "sceLibnetGetInterfaceID (%d)\n", r);
		goto done;
	}

	if ((r = sceInetCtlUpInterface(&cd, net_buf, 0)) < 0) {
		scePrintf( "sceInetCtlUpInterface (%d)\n", r);
		goto done;
	}

	if ((r = sceLibnetWaitGetAddress(&cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0)) < 0) {
		scePrintf( "sceLibnetWaitGetAddress (%d)\n", r);
		goto done;
	}
	
	if ((r = sceInetAddress2String(&cd, net_buf, myptrname, sizeof(myptrname), &myaddr)) < 0) {
		scePrintf( "[%s] sceInetAddress2String (%d)\n",r);
		goto done;
	}
	scePrintf( "address: \"%s\"\n", myptrname);

	if ((r = sceInetCtlDownInterface(&cd, net_buf, 0)) < 0) {
		scePrintf( "sceInetCtlDownInterface (%d)\n", r);
		goto done;
	}
	
done:
	sceSifMExitRpc();
 	sceMcEnd();
	sceNetcnfifTerm();
	scePrintf("complete\n");

	return (0);
}

/*** End of file ***/

