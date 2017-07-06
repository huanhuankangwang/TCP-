OBJS:= filereceiver.o filesender.o packet_frame.o receiver.o sender.o easy_common.o


LOCAL_CFLASS:= -I$(HOME_PATH_INCLUDE)/wtftp/

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g -fPIC $(CFLAGS2) $(LOCAL_CFLASS) -c $^

TARGET:=libwtftp.so
	
all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $^ -shared -lpthread -L$(HOME_PATH_LIBS) -lcommon  -o $@
	mv $@ $(HOME_PATH_LIBS)/$@


wtftp.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	rm *.o *.a -rf