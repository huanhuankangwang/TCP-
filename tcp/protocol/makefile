OBJS:= easy_parsemsg.o easy_common.o messageQueue.o messageRecord.o

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) -c $^

common.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	rm *.o *.a -rf