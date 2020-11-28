all: sysroot makefile.inc $(AUTOGENERATED)
	$(MAKE) -C . INNER=1 ngos.iso

.PHONY: all clean
.SUFFIXES:

clean: makefile.inc
	$(MAKE) -C . INNER=1 mp_clean
	rm -rf sysroot

ifeq ($(INNER),1)
# autogenerated rules from `build.rb`
include makefile.inc
endif

MKISO ?= grub-mkrescue
PARALLEL ?= $(shell nproc)
NIGHTINGALE_VERSION ?= $(shell git describe --tags)
export NIGHTINGALE_VERSION

AUTOGENERATED := \
	kernel/include/ng/autogenerated_syscall_consts \
	kernel/include/ng/autogenerated_syscalls.h \
	kernel/autogenerated_syscalls.inc  \
	libc/include/autogenerated_syscall_names.inc \
	libc/include/autogenerated_syscalls.h \
	libc/autogenerated_syscalls.inc \

sysroot: $(shell find . -name '*.h')
	./install_headers.bash

makefile.inc: magpie_build.rb generate_makefile.rb
	./generate_makefile.rb > makefile.inc

$(AUTOGENERATED): SYSCALLS
	./generate_syscalls.rb

init.tar: $(MP_ALL_INSTALL_TARGETS)
	$(info TAR	$@)
	@cd sysroot/usr/bin; tar cf ../../../$@ *

ngos.iso: init.tar ngk.elf kernel/grub.cfg
	$(info ISO	$@)
	@mkdir -p isodir/boot/grub
	@cp kernel/grub.cfg isodir/boot/grub
	@cp ngk.elf isodir/boot/ngk
	@cp init.tar isodir/boot/initfs
	@$(MKISO) -o $@ isodir/
	@rm -rf isodir

.PHONY: reiso
reiso:
	$(info TAR	init.tar)
	@cd sysroot/usr/bin; tar cf ../../../init.tar *
	$(info ISO	ngos.iso)
	@mkdir -p isodir/boot/grub
	@cp kernel/grub.cfg isodir/boot/grub
	@cp ngk.elf isodir/boot/ngk
	@cp init.tar isodir/boot/initfs
	@$(MKISO) -o ngos.iso isodir/
	@rm -rf isodir

.PHONY: external
external:
	@cd external; ./make.bash
