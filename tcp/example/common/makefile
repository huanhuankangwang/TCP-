%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) -c $^
	
OBJ:= 
OBJS:= filetest.o


TARGET:= testapp writertest ringbuffer_test readerTest test_queue

all:$(TARGET)

testapp:$(OBJS)
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

writertest:writertest.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

ringbuffer_test:ringbuffer_test.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

readerTest:readerTest.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

test_queue:test_queue.o
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

clean:
	rm *.o $(TARGET) -rf