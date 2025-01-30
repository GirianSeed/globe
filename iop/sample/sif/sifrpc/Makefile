#
#  IOP sifrpc program Makefile
#
#  $Id: Makefile,v 1.2 1999/08/30 10:57:13 hakama Exp $
#

TOP = $(dir $(word 1, $(wildcard include ../include ../../include \
		../../../include ../../../../include ) ))

include $(TOP)lib/Makedefs

CFLAGS  = -I$(TOP)include -Wall
ASFLAGS  = -I$(TOP)include

#----------- customize section --------------
PROGNAME = iopmain
all:	$(PROGNAME)

OBJS     = $(PROGNAME).o # add here other objects

$(PROGNAME).o : $(PROGNAME).c # add your local include file

#----------- rules --------------
clean:
	rm -f *.o $(PROGNAME) *.map

#
#  How to crate IOP relocatable executebule object.
#
# full step
#   % iop-gcc -c part1.c
#   % iop-gcc -c part2.c
#   % iop-ld -dc -r -o allpart.obj  part1.o part2.o \
#		-L<libdir> -l<lib>
#   % ioplibld -s allpartsub.s allpart.obj : <libdir>/*.ilb
#   % iop-as -o allpartsub.o allpartsub.s 
#   % iop-ld -dc -r -o program.obj  allpart.obj allpartsub.o
#   % iopfixup -o program  program.obj
#   % rm allpart.obj allpartstub.s allpartstub.o program.obj
# simple way
#   % iop-gcc -c part1.c
#   % iop-gcc -c part2.c
#   % iop-gcc -specs=iopspecs -o program part1.o part2.o \
#		-L<libdir> -l<lib> -ilb=<libdir>/x.ilb
#
$(PROGNAME): $(OBJS)
	$(LINK.o)  $^ $(LOADLIBES) $(LDLIBS) -o $@ \
		-ilb=$(TOP)lib/iop.ilb 
run:
	dsreset 0 2 ; dsistart $(PROGNAME)
