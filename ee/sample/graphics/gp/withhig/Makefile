
SHELL       = /bin/sh

TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include

TARGET      = main
OBJS        = crt0.o $(TARGET).o util.o micro.o camera.o

LCFILE      = $(LIBDIR)/app.cmd
LIBS        =   $(LIBDIR)/libgp.a \
                $(LIBDIR)/libdev.a \
                $(LIBDIR)/libpkt.a \
                $(LIBDIR)/libdma.a \
                $(LIBDIR)/libvu0.a \
                $(LIBDIR)/libpad.a \
                $(LIBDIR)/libhig.a\
                $(LIBDIR)/libhip.a\
                $(LIBDIR)/libgraph.a \

INCLUDES    = -I$(INCDIR)

PREFIX      =  ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

DEBUGOPT    = -O2
CFLAGS      = $(DEBUGOPT) -Wall -Werror -Wa,-al -G0 -fno-common -fno-strict-aliasing
CXXFLAGS    = $(DEBUGOPT) -Wall -Werror -Wa,-al -G0 -fno-exceptions -fno-common -fno-strict-aliasing
ASFLAGS     = -c $(DEBUGOPT) -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS    =

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCLUDES) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCLUDES) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) $(INCLUDES) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) $(INCLUDES) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf
