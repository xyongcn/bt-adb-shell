bt-adb-shell for android and linux in the edison.
============

Adb shell with bluetooth capability. The adb shell includes two parts: adb clint (adb) and adb daemon (adbd). The adbd has two version: android compatible and non-android compatible, while the adb has only one version since the adb is usually used in the x86 platform.

#COMPILING
1. Compile adb clint and adbd for android:
	* following the readme in the android directory.
2. Compile adbd for edison:
	* following the readme in the edison directory.

#USAGE
1. sudo ./adb btconnect xx:xx:xx:xx:xx:xx
2. sudo ./adb shell

