KoboRoot.tgz : __ALWAYS__
	( cd ../rootfs_261 && find . -type l | xargs rm -f )
	( cd ../rootfs_261/lib && ln -s libz.so.1.2.8 libz.so && ln -s libz.so.1.2.8 libz.so.1 ) 
	( cd ../rootfs_261/lib && ln -s libjpeg.so.62.1.0 libjpeg.so && ln -s libjpeg.so.62.1.0 libjpeg.so.62 ) 
	( cd ../rootfs_261/usr/lib && ln -s libfreetype.so.6.10.1 libfreetype.so && ln -s libfreetype.so.6.10.1 libfreetype.so.6 ) 
	( cd ../rootfs_261 && tar --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./lib ./usr ./bin )

README.wiki : __ALWAYS__
	git clone http://wiki.kobohack-j.googlecode.com/git
	( cd git && git tag `date +%y%m%d` )
	cp -p git/README_261.wiki ./README.wiki
	rm -rf git

u-boot_hack.bin : __ALWAYS__
	cp -p ../rootfs/u-boot_hack.bin .

kobohack-j.zip : KoboRoot.tgz README.wiki u-boot_hack.bin
	rm -f KoboRoot.tgz
	( cd ../rootfs_261 && find . -type l | xargs rm -f )
	( cd ../rootfs_261 && git pull )
	( cd ../rootfs_261/lib && ln -s libz.so.1.2.8 libz.so && ln -s libz.so.1.2.8 libz.so.1 ) 
	( cd ../rootfs_261/lib && ln -s libjpeg.so.62.1.0 libjpeg.so && ln -s libjpeg.so.62.1.0 libjpeg.so.62 ) 
	( cd ../rootfs_261/usr/lib && ln -s libfreetype.so.6.10.1 libfreetype.so && ln -s libfreetype.so.6.10.1 libfreetype.so.6 ) 
	( cd ../rootfs_261 && tar --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./lib ./usr/lib ./bin )
	zip -9 $@ $^
	mv $@ kobohack-j-`date +%y%m%d`.zip
	git tag `date +%y%m%d`

.PHONY: __ALWAYS__
