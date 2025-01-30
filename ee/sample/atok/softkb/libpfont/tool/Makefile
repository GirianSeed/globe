SHELL		= /bin/sh

TARGET1		= pb2pf
TARGET2		= gf2pb
TARGET3		= bmp2gf
SRC1		= $(TARGET1).c
SRC2		= $(TARGET2).c
SRC3		= $(TARGET3).c

OBJS1		= $(SRC1:.c=.o)
OBJS2		= $(SRC2:.c=.o)
OBJS3		= $(SRC3:.c=.o)

AS		= gcc
CC		= gcc
LD		= gcc

CFLAGS		= -g -Wall -Wa,-al
ASFLAGS		= -c -xassembler-with-cpp -Wa,-al
LDFLAGS		= -g -lm  -Wl,-Map,$@.map
TMPFLAGS	=

#########################################################################

all:	$(TARGET1) $(TARGET2) $(TARGET3)

font:
	./bmp2gf bmp2gf.txt sample_k.gf
	./gf2pb gf2pb.txt sample_k.pb
	./pb2pf pb2pf.txt output.pf
	cp output.pf ../sample/advanced/sample.pf
	cp output.pf ../sample/basic/sample.pf

$(TARGET1): $(OBJS1) Makefile
	@echo link $(TARGET1)
	@$(LD) -o $@ $(OBJS1) $(LDFLAGS)

$(TARGET2): $(OBJS2) Makefile
	@echo link $(TARGET2)
	@$(LD) -o $@ $(OBJS2) $(LDFLAGS)

$(TARGET3): $(OBJS3) Makefile
	@echo link $(TARGET3)
	@$(LD) -o $@ $(OBJS3) $(LDFLAGS)

## #######################################################################

.SUFFIXES: .c .s .cc .dsm

.s.o:
	@echo asm $<
	@$(AS) $(MD) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.c.o:
	@echo compile $<
	@$(CC) $(MD) $(CFLAGS) $(TMPFLAGS) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *~

depend: $(SRC1) $(SRC2) $(SRC3)
	@makedepend $(SRC1) $(SRC2) $(SRC3)
	@echo done.

