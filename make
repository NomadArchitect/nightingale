#!/usr/bin/env bash

set -euo pipefail

build_dir="build"
cmake_toolchain="toolchain/CMake/CMakeToolchain.txt"

case ${1-} in
arm)
    build_dir="build-arm"
    cmake_toolchain="toolchain/CMake/CMakeToolchain-arm.txt"
    ;;
clean)
    rm -rf build build-arm
    exit 0
    ;;
esac


# Prevent macos from creating phantom metadata files in tar archives
# https://unix.stackexchange.com/a/9865
export COPYFILE_DISABLE=1

mkdir -p "$build_dir"
cd "$build_dir"
[[ -e "CMakeCache.txt" ]] || cmake -DCMAKE_TOOLCHAIN_FILE="$cmake_toolchain" -G Ninja ..
ninja install | grep -v Up-to-date

mkdir -p isodir/boot/limine

cd sysroot
tar -c -f ../isodir/boot/initfs.tar ./*
cd ..

[[ -e limine ]] || git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
make -C limine

cp ./kernel/nightingale_kernel isodir/boot/nightingale_kernel.elf
cp ../kernel/limine.cfg isodir/boot/limine
cp ./limine/limine.sys ./limine/limine-cd.bin ./limine/limine-cd-efi.bin isodir/boot/limine

xorriso -as mkisofs -b boot/limine/limine-cd.bin \
  -no-emul-boot -boot-load-size 4 --boot-info-table \
  --efi-boot boot/limine/limine-cd-efi.bin -efi-boot-part \
  --efi-boot-image --protective-msdos-label \
  isodir -o ngos.iso

./limine/limine-deploy ngos.iso

cp ngos.iso ..