
%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(LOCAL_CFLASS) -c $^

LOCAL_CFLASS:= -I$(HOME_PATH_INCLUDE)/wtftp/

OBJ:= $(HOME_PATH_INCLUDE)/../common/common.a  ../wtftp.a

all:wtftp_test test_frame
	echo  "test"

wtftp_test:wtftp_test.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS) $(LOCAL_CFLASS)

test_frame:test_frame.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS) $(LOCAL_CFLASS)

clean:
	rm *.o wtftp_test test_frame -rf
