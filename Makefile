obj-m := emerg.o
KDIR := /usr/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) M=$(PWD) modules
remove:
	sudo rmmod -f emerg.ko
clean:
	rm -r -f .emerg.* emerg.ko emerg.mod.c emerg.mod.o emerg.o Module.symvers modules.order .tmp_versions
