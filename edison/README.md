bt-adb-shell For Intel Edison
============

# 程序的完整编译：
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
4. 程序文件说明：
	* bt-shell-edison.c：运行在edison上的daemon程序
	* linux-client.c：运行在pc机上的连接程序
5. 编译
	* source <path to cross_toolchain>/environment-setup-core2-32-poky-linux
	* $CC bt-shell-edison.c -o bt-shell-edison -lpthread -lbluetooth
	* gcc linux-client.c -o linux-client -lpthread -lbluetooth
	
6. 软件使用
	* edison：
		* rfkill unblock bluetooth
		* ./bt-shell-edison
	* PC:
		* ./linux-client
		* bleconnect <edison的蓝牙地址>
		* 连接成功后可以操作edison的shell
