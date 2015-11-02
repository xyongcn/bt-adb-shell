all:
	pushd core/libcutils ; make ; popd
	pushd core/adb ; make ; popd
	mv core/adb/adbd .

clean:
	pushd core/libcutils ; make clean ; popd
	pushd core/adb ; make clean ; popd
	rm -f adbd core/libcutils/libcutils.a

install: all
	install -d -m 0755 $(DESTDIR)/$(PREFIX)/sbin
	install -D -m 0755 adb/adbd $(DESTDIR)/$(PREFIX)/sbin/
	install -d -m 0755 $(DESTDIR)/$(PREFIX)/lib/systemd/system/
	install -D -m 0755 adbd.service $(DESTDIR)/$(PREFIX)/lib/systemd/system/
