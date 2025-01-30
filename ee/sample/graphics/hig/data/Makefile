#!gmake
#
# Makefile for HiG Library Core Archive
#
#		start 2000.09.04
#
TARGETS		:= 	ball_anim ball_basem ball_hrchy \
			ball_key ball_micro ball_val \
			ball1_shape ball1_tex2d \
			ball2_shape ball2_tex2d \
			ball3_shape ball3_tex2d \
			ball4_shape ball4_tex2d \
			dino logo shuttle tri \
			cowstrip mbox sbox \
			dino_raw mbox_raw earth_raw \
			mult ballS \
			APEWALK39 \
			line line_t linestrip linestrip_t \
			point point_t triangle triangle_t \
			tristrip tristrip_t trifan trifan_t \
			restex curtain \
			emboss0 emboss1 clutbump \
			shadowmap0 shadowmap1 \
			wall litmap \
			tim2obj \
			reftorus refroom reflect reflect_fish refract refract_fish refobj radtea tree clip\
			skin0 skin1 water xyz_anim
 
HIGDIR		:= ../../../..

#--------------------------------------------------

TOP		:= /usr/local/sce/ee
BINDIR		:= $(TOP)/bin
INCDIR		:= $(TOP)/include
HIGINCDIR	:= $(HIGDIR)/include
GCCDIR		:= $(TOP)/gcc
GCCBIN		:= $(GCCDIR)/bin

BFILES		:= $(addsuffix .bin,$(TARGETS))
AFILES		:= $(BFILES:.bin=.s)
OFILES		:= $(BFILES:.bin=.o)

MAKEFILE	:= Makefile
SHELL		:= /bin/sh
RM		:= /bin/rm -f
ECHO		:= /bin/echo
CP		:= /bin/cp
AS		:= $(GCCBIN)/ee-gcc
OBJCOPY		:= $(GCCBIN)/ee-objcopy

INCDIRS		:= $(INCDIR) $(HIGINCDIR)
INCLUDES	:= $(addprefix -I,$(INCDIRS))
ASFLAGS		:= $(INCLUDES) -c -xassembler-with-cpp -Wa,-al
OBJCOPYFLAGS	:= -O binary -S -g -j .data

default all: $(BFILES)

clean:
	$(RM) $(OFILES) $(BFILES) *.lst *~

%.bin : %.o
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst
