EE_BIN  = ds4ps2_sample_client.elf
EE_OBJS = main.o
EE_LIBS = -lds4ps2 -lgskit -lgskit_toolkit -ldmakit -lm
EE_CFLAGS = -I$(PS2DEV)/gsKit/include -L$(PS2DEV)/gsKit/lib

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

#FOR ME!!
copy: $(EE_BIN)
	cp $(EE_BIN) /media/$(USER)/disk
	sync

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
