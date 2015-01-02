bt-adb-shell
============

Bluetooth-based adb shell application

配置adb编译环境：
---------------
>#基本环境#
>操作系统为ubuntu13.10上   
>虚拟机设置内存4G   
>需要安装jdk-6及其以上版本(必须要是从oracle上下载的，不能采用open-jdk)   
>在linux下配置Java的环境变量   
>>~/.bashrc：   
>>export JAVA_HOME=[java目录]   
>>export JRE_HOME=${JAVA_HOME}/jre   
>>export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib   
>>export PATH=${JAVA_HOME}/bin:$PATH   
    
>#ubuntu64位安装兼容32位库：#
>sudo apt-get install libc6:i386 libgcc1:i386 gcc-4.6-base:i386 libstdc++5:i386 libstdc++6:i386   
 
>#其他必要库：#
>sudo apt-get install bison g++-multilib git gperf libxml2-utils    
>sudo apt-get install flex bison   
>sudo apt-get install lib32z1   
>sudo apt-get install gperf   
>sudo apt-get  install libxml2-utils   

>#安装编译panda的必要文件和驱动：#
>从source.android.com上下载device/ti/panda的目录的文件，让后放到源码的/device/ti/panda目录下。   
>将panda目录下的aosp_panda.mk和full_panda.mk文件复制到/build/target/product/目录下，修改该目录下的AndroidProducts.mk文件，将aosp_panda.mk和full_panda.mk文件添加进来。   
>修改/build/目录下的envsetup.sh文件，找到add_lunch_combo aosp_arm-eng的位置，在其后添加add_lunch_combo aosp_panda-eng和add_lunch_combo full_panda-userdebug。   
>回退到源码根目录，执行以下命令：   
>source ./build/envsetup.sh   
>lunch aosp_panda-eng 可选项[aosp_arm    aosp_maguro    aosp_panda]   
>make -j4   
>即编译出pandaboard的系统平台部分。   

>#修改代码后单独编译adb#   
>当修改了adb或者adbd部分代码后，只需要执行   
>make adb   
>make abd   
>即可完成该模块编译。   

