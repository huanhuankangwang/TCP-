OBJS:= filereceiver.o filesender.o packet_frame.o receiver.o sender.o easy_common.o


LOCAL_CFLASS:= -I$(HOME_PATH_INCLUDE)/wtftp/

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(LOCAL_CFLASS) -c $^

all:wtftp.a
	make -C test
	
wtftp.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	make clean -C test
	rm *.o *.a -rf