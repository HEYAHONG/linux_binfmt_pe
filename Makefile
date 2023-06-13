obj-m := binfmt_pe.o
all:kernel_modules

kernel_modules:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

