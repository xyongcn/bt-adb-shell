bt-adb-shell For Intel Edison
============
# BRIEF DESCRIPTION
This program is a bluetooth connectible adb daemon (bt-adbd) running in the edison (linux). The bt-adbd should be coexisted with the bt-adb (https://github.com/xyongcn/bt-adb-shell). The transplanting from Android to non-Android is from [https://github.com/robclark/core/commit/87f7cfa4632f94255f8ded7c92bd7d576dfd7825].

# COMPILING & INSTALL
1. Run 'make' in the edison directory.
2. Move adbd to the edison system.
3. Configure the system starting up with the adbd.
	* vi startup_bt-adbd.sh:
 ``` bash
#!/bin/sh
rfkill unblock bluetooth
/home/root/adbd &
```  
	* chmod 777 startup_bt-adbd.sh
	* mv startup_bt-adbd.sh /etc/init.d/
	* update-rc.d startup_bt-adbd.sh defaults 97

#Trouble shooting
1. "pushd" and "popd" cmd error
	* You can google it. (simple)
	* Or, you can manually run 'make' in core/libcutils and then core/adb directories. The adbd is in the core/adb.


# OTHER
# 编译环境：
1. 我的编译环境：ubuntu 12.04 32位
2. 交叉编译器的选择：
	* 关于edison交叉编译器网上有两个版本（精简版和完整版），其区别主要在内置的库数量。分别是（精简版：edison-meta-toolchain-linux<....>.zip）和（完整版：edison-sdk-linux<....>.zip）
	* 对于精简版，我们需要交叉编译程序依赖的库bluez，从intel给出的蓝牙文档来看，目前系统版本用到的库是bluez-5.24。
	* 对于完整版，编译bluez库比较繁琐，所以推荐使用该版本。
	* 精简版和完整版的交叉编译过程都经过了测试，可以正常使用。下文以完整版为例。
3. 交叉编译器编译器的下载与部署
	* 地址：https://software.intel.com/en-us/iot/hardware/edison/downloads
		* SDK - Cross Compile Tools一栏，找到自己环境对应的编译器
		* 20150511测试有效
	* 下载后解压，运行压缩包中的脚本，选择解压位置
	* 开启控制台窗口，运行：
		* source <解压位置>/environment-setup-core2-32-poky-linux
			* 名字和下载的编译器版本有关。
		* 测试：echo $CC
			* 会输出相应命令
4. gcc要用到的蓝牙库的编译与配置
	* 编译pc机上运行的连接程序需要gcc编译链中部署了bluez的库，在这我们可以编译老版本的bluez，步骤比较简单。
	* 版本：bluez-libs-3.36，下载位置：http://bluez.sf.net/download/bluez-libs-3.36.tar.gz
	* 编译与部署：
		* cd bluez-libs-3.36
		* ./configure --prefix=/usr --enable-shared=no --enable-static=yes
		* make 
		* make install

5. 编译
	* source <path to cross_toolchain>/environment-setup-core2-32-poky-linux
	* $CC bt-shell-edison.c -o bt-shell-edison -lpthread -lbluetooth
	* gcc linux-client.c -o linux-client -lpthread -lbluetooth
	


	


3. 无法编辑文本
