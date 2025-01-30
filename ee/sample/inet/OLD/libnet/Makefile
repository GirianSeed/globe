### Makefile

SHELL=/bin/sh

TOP = /usr/local/sce/ee
INCDIR = -I$(TOP)/include -I.
PUB_INCDIR = $(TOP)/include
LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include

TARGET = libnet
OBJS = libnet.o
INST_HDRS =

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
RM=/bin/rm -f

IOP_LIBNET_DIR = ../../../../iop/sample/inet/libnet

CFLAGS = -c -Wa,-al -fno-common -O2 -G0
ASFLAGS = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 


.SUFFIXES: .c .s .cc .dsm

#all: $(TARGET).a $(LIB_INST_DIR)
all: $(TARGET).a

$(TARGET).a: $(OBJS) $(IOP_LIBNET_DIR)/libnet.irx
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -I$(PUB_INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -I$(PUB_INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -I$(PUB_INCDIR) -o $@ $< > $*.lst

$(IOP_LIBNET_DIR)/libnet.irx:
	cd $(IOP_LIBNET_DIR) ; make
clean:
	/bin/rm -f *.o *.lst core *.elf *.a


