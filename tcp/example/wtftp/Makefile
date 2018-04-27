
%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(LOCAL_CFLASS) -c $^

LOCAL_CFLASS:= -I$(LIBS_PAHT_INCLUDE)/wtftp/

OBJ:= 

all:wtftp_test test_frame
	echo  "test"

wtftp_test:wtftp_test.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS) $(LOCAL_CFLASS)

test_frame:test_frame.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS) $(LOCAL_CFLASS)

clean:
	rm *.o wtftp_test test_frame -rf
