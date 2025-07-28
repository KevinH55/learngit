obj-m :=tiny210_led_drv.obj
CURRENT_PATH := $(shell pwd)
KERNELDIR := /opt/FriendlyARM/tiny210/linux/linux-3.0.8/
All:tiny210_led_drv.o tiny210_led_test

tiny210_led_drv.o:tiny210_led_drv.c
	make_Wall-C -C $(KERNELDIR) M=$(CURRENT_PATH) modules

tiny210_led_test:tiny210_led_test.c
	arm-linux-gcc -Wall-o tiny210_led_test tiny210_led_test.c
clean:
	make -Wall -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	rm -rf tiny210_led_test