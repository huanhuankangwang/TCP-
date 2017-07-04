HOME_PATH := $(shell pwd)
HOME_PATH_INCLUDE:= $(HOME_PATH)/include
HOME_PATH_LIBS:=$(HOME_PATH)/lib

CFLAGS  := -lpthread -L$(HOME_PATH_LIBS) -lwthread -lcommon -lwtftp
CFLAGS2 := -I$(HOME_PATH_INCLUDE) -I$(HOME_PATH_INCLUDE)/common/
CC:= gcc
LD:=ld

export CFLAGS CFLAGS2 CC LD HOME_PATH_INCLUDE HOME_PATH_LIBS

LIBS_TARGET:= libwthread.so libcommon.so libwtftp.so
	
all:
	make -C example

libs:$(LIBS_TARGET)

libwthread.so:
	make -C wthread
libcommon.so:
	make -C common

libwtftp.so:
	make -C wtftp
	
clean:
	make clean -C common
	make clean -C wtftp
	make clean -C wthread
	make clean -C example
	
testapp:
	make  -C example