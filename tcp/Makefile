HOME_PATH := $(shell pwd)
HOME_PATH_INCLUDE:= $(HOME_PATH)/include
CFLAGS  := -lpthread 
CFLAGS2 := -I$(HOME_PATH_INCLUDE) -I$(HOME_PATH_INCLUDE)/common/
CC:= gcc
LD:=ld


export CFLAGS CFLAGS2 CC LD HOME_PATH_INCLUDE

all:clean
	make -C common
	make -C wtftp
	make -C test

clean:
	make clean -C common
	make clean -C wtftp
	make clean -C test