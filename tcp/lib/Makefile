LIBS_TARGET:= libwthread.so libcommon.so libwtftp.so

all:$(LIBS_TARGET)

libwthread.so:
	make -C wthread
libcommon.so:
	make -C common

libwtftp.so:
	make -C wtftp
	
clean:
	make clean -C wthread
	make clean -C common
	make clean -C wtftp

rm_libs:
	rm lib/* -rf