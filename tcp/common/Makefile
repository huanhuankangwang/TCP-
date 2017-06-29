OBJS:= messageQueue.o messageRecord.o fileReader.o fileWriter.o ringbuffer.o fileoperation.o

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) -c $^

common.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	rm *.o *.a -rf