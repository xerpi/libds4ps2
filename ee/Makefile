EE_LIB    = libds4ps2.a
EE_OBJS   = libds4ps2.o
EE_LIBS   = -lc
EE_CFLAGS = -Wall

all: $(EE_LIB)

clean:
	rm -f -r $(EE_OBJS) $(EE_LIB)

install: $(EE_LIB)
	cp $(EE_LIB)   $(PS2SDK)/ee/lib
	cp libds4ps2.h $(PS2SDK)/ee/include

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
