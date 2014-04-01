.. _07-qemu:

====
QEMU
====

Do użycia urządzenia ``hashdev`` wymagana jest zmodyfikowana wersja qemu,
dostępna w dwóch wersjach binarnych (na arch linuxa oraz na studentsa) oraz
źródłowej.

Aby własnoręcznie skompilować zmodyfikowaną wersję qemu, należy:

1. Ściągnąć pliki :download:`qemu-1.4.0.tar.bz2` oraz
   :download:`qemu-hashdev.patch`
2. ``tar xf qemu-1.4.0.tar.bz2``
3. ``cd qemu-1.4.0``
4. ``patch -p1 < ../qemu-hashdev.patch``
5. Upewnić się, że są zainstalowane zależności: ``ncurses``, ``libsdl``,
   ``curl``, a w niektórych dystrybucjach także ``ncurses-dev``, ``libsdl-dev``,
   ``curl-dev`` (nazwy pakietów mogą się nieco różnić w zależności
   od dystrybucji)
6. Uruchomić ``./configure`` z opcjami wedle uznania (patrz
   ``./configure --help``). Oficjalna binarka była kompilowana z::

       --target-list=i386-softmmu,x86_64-softmmu --python=$(which python2)
       --disable-vnc --enable-kvm --disable-bluez --disable-brlapi
       --extra-cflags=-I/usr/include/ncurses --enable-curses --disable-linux-aio

7. Wykonać ``make``
8. Opcjonalnie zainstalować wykonując ``make install``

Aby zmodyfikowane qemu emulowało urządzenie ``hashdev``, należy przekazać mu
opcję ``-device hashdev``. Przekazanie tej opcji kilka razy spowoduje emulację
kilku instancji urządzenia.

Aby dodać na żywo (do działającego qemu) urządzenie ``hashdev``, należy:

- przejść do trybu monitora w qemu (Ctrl+Alt+2 w oknie qemu)
- wpisać ``device_add hashdev``
- przejść z powrotem do zwykłego ekranu przez Ctrl-Alt-1
- wpisać ``echo 1 > /sys/bus/pci/rescan``, aby linux zauważył

Aby udać usunięcie urządzenia::

    echo 1 > /sys/bus/pci/devices/0000:<idurządzenia>/remove

Niestety nie da się usunąć urządzenia do końca, ze względu na braki
w obsłudze ACPI w naszym obrazie dysku.
