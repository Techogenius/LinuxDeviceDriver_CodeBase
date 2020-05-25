obj-m := ioregion.o

#obj-m := ioport.o

#obj-m := iomem_region.o
#obj-m := test.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

