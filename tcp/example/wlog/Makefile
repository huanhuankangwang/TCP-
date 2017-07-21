LOCAL_CFLASS:= -I$(HOME_PATH_INCLUDE)/common/wlog/

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(LOCAL_CFLASS) -c $^



OBJ:= 

test_wlog: test_wlog.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS) $(LOCAL_CFLASS)


clean:
	rm *.o test_wlog -rf

