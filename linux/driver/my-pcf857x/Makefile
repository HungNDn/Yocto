obj-m := my-pcf857x.o

KDIR := /prj/soc-sw/local/training/hung.nghiem-dinh/yocto/build/tmp/work/rasp3_64_custom-poky-linux/linux-raspberrypi/1_5.4.72+gitAUTOINC+5d52d9eea9_154de7bbd5-r0/linux-rasp3_64_custom-standard-build  # kernel của ARM target
PWD  := $(shell pwd)

CROSS_COMPILE := aarch64-poky-linux-
ARCH := arm64

all:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
