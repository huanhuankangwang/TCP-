EXAMPLE_TARGET:=pwthread pcommon pwtftp

all:$(EXAMPLE_TARGET)
	@echo "############################################"
	@echo "#############make all sucess!##############"
	@echo "############################################"
pcommon:
	make -C common
pwtftp:
	make -C wtftp
pwthread:
	make -C wthread

clean:
	make clean -C common
	make clean -C wtftp
	make clean -C wthread