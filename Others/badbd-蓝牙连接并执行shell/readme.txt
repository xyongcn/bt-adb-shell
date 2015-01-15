1.在该目录下执行ndk-build即可编译生成可执行文件，在./obj目录中往下可以找到
2.编译jni文件夹里面的linux-client.c文件就可以得到在linux下执行的文件（gcc linux-client.c -o linux-client -lpthread -lbluetooth）
3.开启pandaboard端程序，运行linux-client程序。输入 bleconnect [panda蓝牙地址]
	即可连接上pandaboard，然后输入命令即可执行，如ls。