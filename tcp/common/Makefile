OBJS:= messageQueue.o messageRecord.o fileReader.o fileWriter.o ringbuffer.o fileoperation.o

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g -fPIC $(CFLAGS) $(CFLAGS2) -c $^


TARGET:=libcommon.so
	
all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) $^ -shared -lpthread -o $@
	mv $@ $(HOME_PATH_LIBS)/$@
	
common.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	rm *.o *.a -rf