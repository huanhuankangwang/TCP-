OBJS:= debug_manager.o debugTagManager.o netprint.o stdout.o

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(LOCAL_CFLASS) -fPIC -c $^

wlog.a:$(OBJS)
	$(LD) -r -o $@ $^

clean:
	rm *.o *.a -rf