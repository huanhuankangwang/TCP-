HOME_PATH := $(shell pwd)
LIBS_PAHT_INCLUDE:= $(HOME_PATH)/lib/include
HOME_PATH_INCLUDE:= $(HOME_PATH)/include

HOME_PATH_LIBS:=$(HOME_PATH)/lib

CFLAGS  := -lpthread -L$(HOME_PATH_LIBS) -lwthread -lcommon -lwtftp
CFLAGS2 := -I$(HOME_PATH_INCLUDE) -I$(LIBS_PAHT_INCLUDE)/common/
CC:= gcc
LD:=ld

export CFLAGS CFLAGS2 CC LD HOME_PATH_INCLUDE HOME_PATH_LIBS LIBS_PAHT_INCLUDE

LIBS_TARGET:= alllibs
	
all:rm_libs libs
	make -C example

libs:$(LIBS_TARGET)

alllibs:
	make -C lib
	
clean:
	make clean -C lib
	make clean -C example

rm_libs:
	rm lib/*.so -rf
	
testapp:
	make  -C example