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
	* This has to do with /bin/sh being linked to /bin/dash instead of /bin/bash in ubuntu’s    latest releases. To fix it once and for all do the following:
		* rm -f /bin/sh
		* ln -s /bin/bash /bin/sh
	* Or, you can manually run 'make' in core/libcutils and then core/adb directories. The adbd is in the core/adb.

