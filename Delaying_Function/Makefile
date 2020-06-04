obj-m := busy_wait.o
obj-m += schedule.o
obj-m += schedule_timeout.o
obj-m += waitqueue_event.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

