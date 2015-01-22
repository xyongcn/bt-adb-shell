解压uInitrd
==========
>dd if=./uInitrd of=initrd.cpio.gz bs=1 skip=64    
>mkdir file    
>cd file  
>#将.gz解压到file里面   

重新打包uInitrd
===============
>#进入到file目录里面
>sudo find . | cpio -o -H newc | gzip > ../newinitrd.cpio.gz    
>cd ..
>mkimage -A arm -O linux -T ramdisk -C gzip -a 0 -e 0 -n initramfs -d ./newinitrd.cpio.gz ./uInitrd.new    