KoboRoot.tgz : __ALWAYS__
	( cd ../rootfs && find . -type l | xargs rm -f )
	( cd ../rootfs/lib && ln -s libz.so.1.2.7 libz.so && ln -s libz.so.1.2.7 libz.so.1 ) 
	( cd ../rootfs/lib && ln -s libpng.so.3.50.0 libpng.so.3 ) 
	( cd ../rootfs/lib && ln -s libpng12.so.0.50.0 libpng.so && ln -s libpng12.so.0.50.0 libpng12.so && ln -s libpng12.so.0.50.0 libpng12.so.0 ) 
	( cd ../rootfs/usr/lib && ln -s libfreetype.so.6.10.0 libfreetype.so && ln -s libfreetype.so.6.10.0 libfreetype.so.6 ) 
	( cd ../rootfs/usr/lib && ln -s libxml2.so.2.7.8 libxml2.so && ln -s libxml2.so.2.7.8 libxml2.so.2 ) 
	( cd ../rootfs && tar --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./lib ./usr/lib ./bin )

README.wiki : __ALWAYS__
	git clone http://wiki.kobohack-j.googlecode.com/git
	( cd git && git tag `date +%y%m%d` )
	cp -p git/README.wiki .
	rm -rf git

u-boot.bin : __ALWAYS__
	cp -p ../rootfs/u-boot.bin .

kobohack-j.zip : KoboRoot.tgz README.wiki u-boot.bin
	rm -f kobohack-j.zip KoboRoot.tgz
	( cd ../rootfs && find . -type l | xargs rm -f )
	( cd ../rootfs && git pull )
	( cd ../rootfs/lib && ln -s libz.so.1.2.7 libz.so && ln -s libz.so.1.2.7 libz.so.1 ) 
	( cd ../rootfs/lib && ln -s libpng.so.3.50.0 libpng.so.3 ) 
	( cd ../rootfs/lib && ln -s libpng12.so.0.50.0 libpng.so && ln -s libpng12.so.0.50.0 libpng12.so && ln -s libpng12.so.0.50.0 libpng12.so.0 ) 
	( cd ../rootfs/usr/lib && ln -s libxml2.so.2.7.8 libxml2.so && ln -s libxml2.so.2.7.8 libxml2.so.2 ) 
	( cd ../rootfs/usr/lib && ln -s libfreetype.so.6.10.0 libfreetype.so && ln -s libfreetype.so.6.10.0 libfreetype.so.6 ) 
	( cd ../rootfs/usr/lib && rm libfreetype* ) 
	( cd ../rootfs && tar --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./lib ./usr/lib ./bin )
	git tag `date +%y%m%d`
	zip -9 $@ $^
	mv $@ kobohack-j-`date +%y%m%d`.zip

.PHONY: __ALWAYS__
