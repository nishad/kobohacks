KoboRoot.tgz : __ALWAYS__
	( cd ../rootfs/lib && find . -type l | xargs rm )
	( cd ../rootfs/lib && ln -s libz.so.1.2.7 libz.so && ln -s libz.so.1.2.7 libz.so.1 ) 
	( cd ../rootfs && tar --owner=root --group=root -zcvf ../script/KoboRoot.tgz ./lib )

.PHONY: __ALWAYS__
