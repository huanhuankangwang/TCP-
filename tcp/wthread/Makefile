WTHREAD_INCLUDE:= -I$(HOME_PATH_INCLUDE)/wthread/

OBJS:= wthread.o wthread_private.o
%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g  -fPIC $(CFLAGS) $(CFLAGS2) $(WTHREAD_INCLUDE) -c $^

TARGET:= libwthread.so
all:$(TARGET)

wthread.a:$(OBJS)
	$(LD) -r -o $@ $^

libwthread.so:$(OBJS)
	$(CC) $^ -shared -lpthread -o  $@
	mv $@ $(HOME_PATH_LIBS)/$@

clean:
	rm *.o *.a -rf