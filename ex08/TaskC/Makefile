obj-m += myproc.o
BUILDROOTDIR := /home/student/sola/ttk4147/ex08/buildroot
KERNELDIR := $(BUILDROOTDIR)/output/build/linux-2.6.35.4
CROSS := $(BUILDROOTDIR)/output/staging/usr/bin/avr32-linux-

all:
	make ARCH=avr32 CROSS_COMPILE=$(CROSS) -C $(KERNELDIR) M=$(shell pwd) modules
	
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers
