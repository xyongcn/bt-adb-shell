bt-adb-shell for android and linux in the edison.
============

Adb shell with bluetooth capability. The adb shell includes two parts: adb clint (adb) and adb daemon (adbd). The adbd has two version: android compatible and non-android compatible, while the adb has only one version since the adb is usually used in the x86 platform.

#ENVIRONMENT
1. Compile and configure the bluetooth lib for gcc. (Build adb and non-android compatible adbd)
	* version: bluez-libs-3.36, http://bluez.sf.net/download/bluez-libs-3.36.tar.gz
	* tar -xvzf bluez-libs-3.36.tar.gz
	* cd bluez-libs-3.36
	* ./configure --prefix=/usr --enable-shared=no --enable-static=yes
	* make 
	* make install

#COMPILING
1. Compile adb clint and adbd for android:
	* following the readme in the android directory.
2. Compile adbd for edison:
	* following the readme in the edison directory.

#USAGE
1. sudo ./adb btconnect xx:xx:xx:xx:xx:xx
2. sudo ./adb shell

