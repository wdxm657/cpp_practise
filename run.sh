#!/bin/sh

driver="pango_pci_driver"
target="app"
root_name="root"									#默认管理员名：root	
temp_name=$(whoami)									#执行whoami指令，获取控制台当前的操作用户名
if [ "$root_name" = "$temp_name" ]					#判断操作用户是否为root
then
	if [ ` lsmod | grep -o "$driver"` ]				#判断PCIe驱动是否已经装载
	then
		echo "****************************PCIe驱动已装载***************************"
	else
		echo "*************************开始编译PCIe驱动程序************************"
		cd ./pcie/driver
		make clean
		make
		echo "***************************开始装载PCIe驱动**************************"
		insmod $driver.ko							#装载驱动
		cd ../../
		if [ ` lsmod | grep -o "$driver"` ]			#判断PCIe驱动是否已经装载成功
		then
			echo "***************************PCIe驱动装载成功**************************"
		else
			echo "***************************PCIe驱动装载失败**************************"
			exit
		fi
	fi
	echo "*************************开始编译应用程序************************"
#	cd ./build
#	make clean
#	make -j
#	echo "***************************启动可执行程序************************"
#	./$target
#	cd ../
#	echo "***************************退出可执行程序************************"
else
	echo "该脚本默认管理 员 名：$root_name"
	echo "控制台当前操作用户名：$temp_name"
	echo "请将操作用户切换为$root_name(管理员)"
	exit
fi


