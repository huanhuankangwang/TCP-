WTHREAD_INCLUDE:= -I$(HOME_PATH_INCLUDE)/wthread/

%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) $(CFLAGS2) $(WTHREAD_INCLUDE) -c $^
	
OBJ:= 
OBJS:= wthread_test.o

TARGET:= wthreadtest

all:$(TARGET)

wthreadtest:$(OBJS)
	$(CC) -o $@  $^ $(OBJ)  $(CFLAGS)
clean:
	rm *.o $(TARGET) -rf