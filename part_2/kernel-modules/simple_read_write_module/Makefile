obj-m += module2.o
LINUX_DIR = /lib/modules/6.9.8/build

all:
	make -C $(LINUX_DIR) M=$(PWD) modules 

clean:
	make -C $(LINUX_DIR) M=$(PWD) modules
