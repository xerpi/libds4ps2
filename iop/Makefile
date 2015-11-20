IOP_BIN  = ds4ps2.irx
IOP_OBJS = ds4ps2.o

IOP_CFLAGS = -Wall

all: $(IOP_BIN)

clean:
	rm -f -r $(IOP_OBJS) $(IOP_BIN)

install: $(IOP_BIN)
	cp $(IOP_BIN) $(PS2SDK)/iop/irx
	cp ds4ps2.h	  $(PS2SDK)/iop/include
	cp ds4ps2.h	  $(PS2SDK)/ee/include

#FOR ME!!
copy: $(IOP_BIN)
	cp $(IOP_BIN) /run/media/xerpi/USB1GB
	sync

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal
