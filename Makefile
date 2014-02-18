HTML = 01_kernel/readme.html 01_kernel/qemu.html

all: $(HTML)

%.html: %.rst
	rst2html $< $@
