bt-adb-shell
============

Bluetooth-based adb shell application

# 两部分：编译adb与编译adbd
# 编译adb
1. 环境：32位ubuntu
2. cd system/core/adb
3. make
    * 可能需要修改Makefile，将-lbluetooth前指定的位置修正。
    * 如果提示libbluetooth.a不匹配，则需要自行编译bluez-lib-3.36（静态编译./configure --host=arm-none-linux-gnueabi --prefix=<.....> --enable-shared=no --enable-static=yes）

# 编译adbd
1. 环境：64位ubuntu
2. 编译步骤见【配置adb编译环境以及编译adb】一节
    * 将system/core/adb中文件替换原有文件
    * include文件放到external/bluetooth/下
    * 不需要额外的bluetooth动态库。
3. 解包打包uInitrd
    * 解包：
      * dd if=boot/uInitrd of=initrd.cpio.gz bs=1 skip=64
      * sudo gunzip -c initrd.cpio.gz | cpio -i
    * 替换、修改文件
      * 替换adbd
      * 修改default.prop，加入以下行
        * persist.adb.tcp.port=5000
        * 如果要打开日志模式，再加入persist.adb.trace_mask=0x3ff，日志记录在/data/adb/adb....txt，只对adbd有效，并且打开后会有点卡。
    * 打包：
      * 进入对应目录
      * sudo find . | cpio -o -H newc | gzip > newinitrd.cpio.gz
      * mkimage -A arm -O linux -T ramdisk -C gzip -a 0 -e 0 -n initramfs -d newinitrd.cpio.gz uInitrd.new

# 使用方法
1. 插入电源后需要等2~3分钟再用
2. 命令：
    * sudo ./adb btconnect xx:xx:xx:xx:xx:xx
    * sudo ./adb shell



配置adb编译环境以及编译adb：
---------------
>#大致流程#
>首先需要搭建好编译pandaboard android platform的平台，完整编译一遍   
>然后修改了adb的代码，只需要单独敲命令make adb即可编译。   

>#编译platform基本环境#
>操作系统为64位ubuntu13.10    
>虚拟机设置内存4G   
>需要安装jdk-6及其以上版本(必须要是从oracle上下载的，不能采用open-jdk)   
>在linux下配置Java的环境变量   
>>~/.bashrc：   
>>export JAVA_HOME=[java目录]   
>>export JRE_HOME=${JAVA_HOME}/jre   
>>export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib   
>>export PATH=${JAVA_HOME}/bin:$PATH   
    
>#编译需要安装的库#
>sudo apt-get install libc6:i386 libgcc1:i386 gcc-4.6-base:i386 libstdc++5:i386 libstdc++6:i386    
>sudo apt-get install bison g++-multilib git gperf libxml2-utils    
>sudo apt-get install flex bison   
>sudo apt-get install lib32z1   
>sudo apt-get install gperf   
>sudo apt-get  install libxml2-utils   

>#加入pandaboard platform必要文件和驱动：#
>从source.android.com上下载device/ti/panda的目录的文件，让后放到源码的/device/ti/panda目录下。   
>将panda目录下的aosp_panda.mk和full_panda.mk文件复制到/build/target/product/目录下，修改该目录下的AndroidProducts.mk文件，将aosp_panda.mk和full_panda.mk文件添加进来。   
>修改/build/目录下的envsetup.sh文件，找到add_lunch_combo aosp_arm-eng的位置，在其后添加add_lunch_combo aosp_panda-eng和add_lunch_combo full_panda-userdebug。   

>#编译pandaboard android paltform#
>在源码根目录执行以下命令：   
>source ./build/envsetup.sh   
>lunch aosp_panda-eng 可选项[aosp_arm    aosp_maguro    aosp_panda]    
>make -j4   
>即编译出pandaboard的android platform系统平台部分。   

>#修改代码后单独编译adb#   
>当修改了adb或者adbd部分代码后，需要清理    
>source ./build/envsetup.sh    
>lunch aosp_panda-eng    
>make adb   
>make adbd   
>以上命令即可完成该模块重新编译，完成后有提示是在哪个目录下。    

