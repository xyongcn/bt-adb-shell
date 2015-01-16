使用方式
===========
>将目录中的uInitrd-ble-server文件拷贝到pandaboard的boot分区中，改名为uInitrd（要提前删掉原来的uInitrd）    
>进入jni目录，拷贝linux-client到linux系统上，运行。输入bleconnect [蓝牙地址] ，经提示连接上后，即可输入命令进行相应操作。    
>注意：输入exit会退出连接，然后可以通过bleconnect 连接其他的设备，如果再次输入exit会退出客户端。因此，在连接上蓝牙设备时，需要用到exit的命令都是无法传递到pandaboard端的，这点需要注意。   

从源代码编译
--------
>在该目录下执行ndk-build即可编译生成可执行文件，在./obj目录中往下可以找到badb-pan,这就是pandaboard端运行的服务程序    
>将boot分区中的uInitrd解压，并将badb-pan拷贝到sbin目录中，然后再将init.rc文件的末尾加上     
>>service badb-pan /sbin/badb-pan    
>>    class main    
>>    user root    
>再将数据压缩还原成uInitrd就可得到上述的文件。   
>    
>编译jni文件夹里面的linux-client.c文件就可以得到在linux下执行的文件（gcc linux-client.c -o linux-client -lpthread -lbluetooth）    
>开启pandaboard端程序，运行linux-client程序。输入 bleconnect [panda蓝牙地址] 即可连接上pandaboard，然后输入命令即可执行，如ls。    
