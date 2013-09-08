KoboRoot.tgz : __ALWAYS__
	( cd ../rootfs_261 && find . -type l | xargs rm -f )
	( cd ../rootfs_261/lib && ln -s libz.so.1.2.8 libz.so && ln -s libz.so.1.2.8 libz.so.1 ) 
	( cd ../rootfs_261/lib && ln -s libjpeg.so.62.1.0 libjpeg.so && ln -s libjpeg.so.62.1.0 libjpeg.so.62 ) 
	( cd ../rootfs_261/lib && ln -s libpng16.so.16.3.0 libpng12.so && ln -s libpng16.so.16.3.0 libpng12.so.0 ) 
	( cd ../rootfs_261/usr/lib && ln -s libfreetype.so.6.10.2 libfreetype.so && ln -s libfreetype.so.6.10.2 libfreetype.so.6 ) 
	( cd ../rootfs_261/bin && ln -s ../sbin/mkfs.fat mkdosfs ) 
	( cd ../rootfs_261/bin && ln -s ../sbin/fsck.fat dosfsck ) 
	( cd ../rootfs_261/lib && ln -s libexpat.so.1.6.0 libexpat.so && ln -s libexpat.so.1.6.0 libexpat.so.1 ) 
	( cd ../rootfs_261/lib && ln -s libsqlite3.so.0.8.6 libsqlite3.so && ln -s libsqlite3.so.0.8.6 libsqlite3.so.0 ) 
	( cd ../rootfs_261 && tar --exclude-from=../script/hackfiles.txt --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./bin ./lib ./sbin ./usr )

KoboRoot_hack.tgz : __ALWAYS__
	( cd ../rootfs_261 && find . -type l | xargs rm -f )
	( cd ../rootfs_261/bin && ln -s dropbearmulti dropbear && ln -s dropbearmulti dropbearkey && ln -s dropbearmulti dropbearconvert && ln -s dropbearmulti dbclient ) 
	( cd ../rootfs_261 && tar --files-from=../script/hackfiles.txt --owner=root --group=root -zcvf ../script/KoboRoot_hack.tgz ./libexec )

README.wiki : __ALWAYS__
	git clone http://wiki.kobohack-j.googlecode.com/git
	( cd git && git tag `date +%y%m%d` )
	cp -p git/README_261.wiki ./README.wiki
	rm -rf git

u-boot_hack.bin : __ALWAYS__
	cp -p ../rootfs/u-boot_hack.bin .

u-boot_hack_mark4.bin : __ALWAYS__
	cp -p ../rootfs_261/u-boot_hack_mark4.bin .

kobohack-j.zip : KoboRoot.tgz KoboRoot_hack.tgz README.wiki u-boot_hack.bin u-boot_hack_mark4.bin
	zip -9 $@ $^
	mv $@ kobohack-j-`date +%y%m%d`.zip
	git tag `date +%y%m%d`

.PHONY: __ALWAYS__
