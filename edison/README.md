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
	
6. 在edison中开机启动后台daemon程序
	* 思路：写一个shell脚本，在脚本中执行后台程序。把你的启动脚本放在/etc/init.d里
	* 注：先前的edison的yocto版本1.6中，在/etc里没有init.d文件夹，可以自己创建一个mkdir init.d,而最新的yocto版本1.6.1里面是含有init.d文件夹的
	* 脚本内容：
```bash
#!/bin/sh
rfkill unblock bluetooth
/mnt/bt-shell-edison
```
	* 把bt-shell-edison程序放到/mnt下，并设置可执行权限
	* 执行：update-rc.d /etc/init.d/my_start.sh defaults 97
	

7. 软件使用
	* edison：插电，等待启动
	* PC:
		* ./linux-client
		* bleconnect <edison的蓝牙地址>
		* 连接成功后可以操作edison的shell

# 存在的问题
1. 只重定向了标准输入输出，没有重定向错误流
2. tab键补全操作无效
3. 无法编辑文本
