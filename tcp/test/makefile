%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) -c $^
	
OBJ:= ../common/common.a
OBJS:= filetest.o

testapp:$(OBJS)
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

clean:
	rm *.o -rf