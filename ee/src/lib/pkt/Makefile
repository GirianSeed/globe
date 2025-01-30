### Makefile

SHELL=/bin/sh

TOP = ../../..
LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include
SRC_INST_DIR = /usr/local/sce/ee/src/lib/pkt
INCDIR = $(HDR_INST_DIR)

TARGET = libpkt
OBJS = libdmapk.o libvifpk.o libgifpk.o
OBJS =\
	dmapk000.o dmapk001.o dmapk002.o dmapk003.o dmapk004.o dmapk005.o dmapk006.o dmapk007.o\
	dmapk008.o dmapk009.o dmapk010.o dmapk011.o dmapk012.o dmapk013.o dmapk014.o\
	gifpk000.o gifpk001.o gifpk002.o gifpk003.o gifpk004.o gifpk005.o gifpk006.o gifpk007.o\
	gifpk008.o gifpk009.o gifpk010.o gifpk011.o gifpk012.o gifpk013.o gifpk014.o gifpk015.o\
	gifpk016.o gifpk017.o gifpk018.o gifpk019.o gifpk115.o gifpk116.o\
	vifpk000.o vifpk001.o vifpk002.o vifpk003.o vifpk004.o vifpk005.o vifpk006.o vifpk007.o\
	vifpk008.o vifpk009.o vifpk010.o vifpk011.o vifpk012.o vifpk013.o vifpk014.o vifpk015.o\
	vifpk016.o vifpk017.o vifpk018.o vifpk019.o vifpk020.o vifpk021.o vifpk022.o vifpk023.o\
	vifpk024.o vifpk025.o vifpk026.o vifpk027.o vifpk028.o vifpk029.o vifpk030.o vifpk031.o\
	vifpk032.o vifpk033.o vifpk034.o vifpk035.o vifpk036.o vifpk037.o vifpk038.o vifpk039.o\
	vifpk040.o vifpk041.o vifpk042.o vifpk043.o vifpk044.o vifpk045.o vifpk046.o vifpk047.o\
	vifpk048.o vifpk049.o vifpk050.o vifpk051.o vifpk052.o vifpk053.o vifpk054.o vifpk055.o\
	vifpk056.o vifpk057.o vifpk121.o vifpk122.o vifpk140.o vifpk240.o vifpk150.o vifpk151.o\
	vifpk152.o vifpk153.o vifpk253.o vifpk353.o
INST_HDRS = libpkt.h libdmapk.h libgifpk.h libvifpk.h

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
RM=/bin/rm -f
INSTALL = install


CFLAGS = -c -Wall -Wa,-al -fno-common -O2 -G0 
ASFLAGS = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 

INIT_OBJ = dmapk000.o gifpk000.o vifpk000.o vifpk025.o

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

clean:
	/bin/rm -f *.o *.lst core *.elf *.a

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	$(INSTALL) -d $(SRC_INST_DIR)
	sed '/version/d' libdmapk.c > $(SRC_INST_DIR)/libdmapk.c
	sed '/version/d' libgifpk.c > $(SRC_INST_DIR)/libgifpk.c
	sed '/version/d' libvifpk.c > $(SRC_INST_DIR)/libvifpk.c
	sed '/version/d' Makefile > $(SRC_INST_DIR)/Makefile

dmapk000.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC000 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk001.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC001 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk002.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC002 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk003.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC003 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk004.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC004 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk005.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC005 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk006.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC006 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk007.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC007 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk008.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC008 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk009.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC009 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk010.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC010 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk011.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC011 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk012.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC012 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk013.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC013 -I$(INCDIR) -c $< -o $*.o > $*.lst
dmapk014.o : libdmapk.c $(INCDIR)/libdmapk.h
	$(CC) $(CFLAGS) -DFUNC014 -I$(INCDIR) -c $< -o $*.o > $*.lst


gifpk000.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC000 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk001.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC001 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk002.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC002 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk003.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC003 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk004.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC004 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk005.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC005 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk006.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC006 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk007.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC007 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk008.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC008 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk009.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC009 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk010.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC010 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk011.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC011 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk012.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC012 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk013.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC013 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk014.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC014 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk015.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC015 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk115.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC115 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk016.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC016 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk116.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC116 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk017.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC017 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk018.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC018 -I$(INCDIR) -c $< -o $*.o > $*.lst
gifpk019.o : libgifpk.c $(INCDIR)/libgifpk.h
	$(CC) $(CFLAGS) -DFUNC019 -I$(INCDIR) -c $< -o $*.o > $*.lst

vifpk000.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC000 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk001.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC001 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk002.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC002 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk003.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC003 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk004.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC004 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk005.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC005 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk006.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC006 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk007.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC007 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk008.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC008 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk009.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC009 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk010.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC010 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk011.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC011 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk012.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC012 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk013.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC013 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk014.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC014 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk015.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC015 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk016.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC016 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk017.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC017 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk018.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC018 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk019.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC019 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk020.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC020 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk021.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC021 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk022.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC022 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk023.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC023 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk024.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC024 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk025.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC025 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk026.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC026 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk027.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC027 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk028.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC028 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk029.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC029 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk030.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC030 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk031.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC031 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk032.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC032 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk033.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC033 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk034.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC034 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk035.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC035 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk036.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC036 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk037.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC037 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk038.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC038 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk039.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC039 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk040.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC040 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk041.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC041 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk042.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC042 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk043.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC043 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk044.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC044 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk045.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC045 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk046.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC046 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk047.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC047 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk048.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC048 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk049.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC049 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk050.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC050 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk051.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC051 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk052.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC052 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk053.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC053 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk054.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC054 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk055.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC055 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk056.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC056 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk057.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC057 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk121.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC121 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk122.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC122 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk140.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC140 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk240.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC240 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk150.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC150 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk151.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC151 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk152.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC152 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk153.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC153 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk253.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC253 -I$(INCDIR) -c $< -o $*.o > $*.lst
vifpk353.o : libvifpk.c $(INCDIR)/libvifpk.h
	$(CC) $(CFLAGS) -DFUNC353 -I$(INCDIR) -c $< -o $*.o > $*.lst

$(INIT_OBJ): version.h 

#[eof]
