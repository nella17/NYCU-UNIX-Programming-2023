
obj-m     += kshram.o
ccflags-y += -DEXPORT_SYMTAB

KERNELDIR = ../dist/modulebuild

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
