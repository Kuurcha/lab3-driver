obj-m += main_md_test.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	sudo dmesg
	sudo insmod main_md_test.ko
	sudo rmmod main_md_test.ko
	sudo dmesg

check:
	lsmod | grep "main_md_test"

del:
	sudo rmmod "main_md_test"

#
# Для вновь зарегистрированного устройства, в файле /proc/devices появится запись c DEVICE_NAME
#
#
# /sys/class/
#
# /dev/
#
#
# mknod /dev/ttyIIVS_HVI3 c 236 0
#
# check file:
# ls -l /dev/ttyIIVS_HVI3
#
# view file:
# cat /dev/ttyIIVS_HVI3
#
# echo file:
# root
# echo "text_of_message_test" > /dev/ttyIIVS_HVI3
#

