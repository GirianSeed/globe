[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2003 Sony Computer Entertainment Inc.
				      All Rights Reserved

Unloading Modules

<Overview>
	This program unloads the module specified on the command line.

<Files>
	Makefile
	unload.c

<Build Method>

	% make

<Execution Method>

	% dsestart unload.erx [-s] <module-name | module-ID>

<Description>
	This sample program uses the sceErxStopModule() and sceErxUnloadModule() functions of liberx to stop and unload a module currently loaded in memory.
	Since the DECI2 ELOADP protocol does not define a protocol for unloading a module, a module cannot be directly unloaded by issuing a request from the host. Instead, the target module can be indirectly unloaded by using the following procedure:
	1. Temporarily start up unload.erx from the host.
	2. Have unload.erx perform unload processing.
	3. Have unload.erx itself terminate with a non-resident end.

	If the -s option is specified, the module will only be stopped.

