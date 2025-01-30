[SCEI CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library  Release 3.0
     Copyright (C) 2002 Sony Computer Entertainment Inc.
                                        All Rights Reserved

Sample Program Showing How to Perform Communications via an i.LINK (IEEE1394) Terminal

<Description>

This program uses the i.LINK socket, which is an SCE-original protocol, to display on the monitor whether the receive results are correct when two PlayStation 2 devices mutually send and receive automatically generated data. 

This program is associated with an IOP-side sample, and both samples work together. Since the EE-side program automatically loads the IOP-side module, be sure to build the IOP-side sample (iop/sample/ilink) in advance.

- Procedure up to connection
First, since the GUID (Global Unique ID) of the target PlayStation 2 is unknown, it is sent by a broadcast. At the same time, the receiving thread monitors the socket and checks the sender GUID of a received packet. If the packet is from a node other than its own node, the receiving thread registers that GUID as a communication target and connects to that node. Subsequently, packets are sent only to the connected node. This program does not assume that an i.LINK connection is disconnected during communication.

- Display
This program displays whether the checksum that was added to the receive data is correct. Since reception is asynchronous, when there is no data to be received, blank lines are displayed, and when there is excess data, it is entered in a FIFO buffer and fetched sequentially starting with the oldest data.

- Additional information
If you enable "#define SEND_NO_WAIT" at the beginning of eemain.c, transmission will be asynchronous. If you disable "#define SEND_NO_WAIT", transmission will be synchronous. The only difference between synchronous and asynchronous transmission is whether or not SIF_RPCM_NOWAIT is used when a RPC is performed by socketee.c.

- Concerning the i.LINK socket driver
When a broadcast is performed using the i.LINK socket, the load on the IOP is great and increases in proportion to the number of nodes that are connected to the i.LINK bus. Therefore, you should use a broadcast only when necessary.

When the i.LINK socket is used, the maximum payload size (size that the user can freely use) that can be sent or received in one packet is 492 bytes. Since the sample sends or receives 7 packets in 1V, the data transfer rate is 492x7x60=206650 bytes/second.

When packets arrive faster than the speed at which receive packets from the i.LINK socket driver are fetched with sceILsockRecv, the FIFO buffer inside the driver will overflow. The default size of this FIFO is 7 packets.

Packets exceeding this number are discarded and not stored in the FIFO.

- Termination of the sample program
The thread created by sceSocketClose() is stopped/deleted, and memory that was allocated is freed. Then, ilsample.irx, ilsock.irx, and ilink.irx are stopped in order, and unloaded.

<Files>
	eemain.c        Main program
	socketee.c      RPC client

<Execution>
Connect two PlayStation 2 devices by an S400-compliant IEEE1394 cable (Sony VMC-IL4415, 4435, etc.).

	% make :  IOP-side compile (iop/sample/ilink)

	Change to EE-side sample directory (ee/sample/ilink)
	% make :  EE-side compile

After compilation, start up dsedb at each PlayStation 2 device and execute the following.

	% dsedb
	> run eemain.elf

	
<Controller Operation>
	Circle button: Stops and unloads modules, then terminates the sample program.

<Explanation of Screen Displays>
	NodeID :  Node ID of own device
	Src :  GUID (64 bits) of own device
	Dest :  GUID (65 bits) of correspondent device
	Status :  Link status of own device
			Single:  Unconnected
			Connect: Connected
	Elapsed :  Elapsed time since execution started 
		  (hours : minutes : seconds : frames)
	nRemHDPkt :  Number of times half done packets (*) 
		     were deleted from application FIFO
	(*:  If a problem is detected in the continuity of 	     the receive packets when multiple packets are 	     received in 1V, those packets are deleted from 	     the FIFO.)
	nRetryCnt :  Number of retransmission attempts due 
		     to some kind of error during 
		     synchronous transmission
                     This is not displayed when asynchronous 
		     transmission is used.
	TxFrm : Transmission frame number (second : 1/60 
		second)
	nRx :  Number of packets remaining in the 
	       reception FIFO
	RxFrm :  Reception frame number (second : 1/60 
		 second)
	SumN... : Checksum result of the Nth packet

<Notes>
When communication starts, the line:
	eemain.c line: xxx, Error: -1021 at sceILsockSend
is displayed.

This error:
	-1021(= SCE1394ERR_RESET_DETECTED)
is caused by a bus reset issued by the PlayStation 2 on the destination side, and is not a problem.

