SHELL       = /bin/sh

TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCOPT      = -I$(TOP)/../common/include -I$(TOP)/include 
LIBOPT      = -L$(LIBDIR)

TARGET      = main
OBJS        = crt0.o \
              $(TARGET).o 

LCFILE      = $(LIBDIR)/app.cmd
LIBS        =  $(LIBDIR)/libgraph.a

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc 
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

CFLAGS      = -O2 -Wa,-al -fno-common -G0
CXXFLAGS    = -O2 -Wa,-al -fno-exceptions -fno-common
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS =
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 $(LIBOPT) -lsdr -lpad -lm 
TMPFLAGS    = -I../../../../common/include

.SUFFIXES: .c .s .cc .dvpasm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCOPT) -o $@ $< > $*.lst

.dvpasm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCOPT) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) $(INCOPT) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) $(INCOPT) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	dsreset 0 0; $(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst *.dis *.elf core

clear:
	$(RM) *~
