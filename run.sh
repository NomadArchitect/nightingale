#!/bin/sh

KERNEL=ngk
ISO=ngos.iso
VM=qemu-system-x86_64

DEFAULTS="-cdrom $ISO -vga std -no-reboot -m 64M"
VIDEO="-display none -serial stdio"
EXTRA=""

while getopts "dvsim" opt; do
    case $opt in
        d)
            # Debug
            EXTRA="$EXTRA -S -s"
            ;;
        v)
            # Video on
            VIDEO=""
            EXTRA="$EXTRA -monitor stdio"
            ;;
        s)
            # Serial on (video/X off) - default
            ;;
        i)
            # Show interrupts
            EXTRA="$EXTRA -d int"
            ;;
        m)
            # Only monitor (no output)
            VIDEO="-display none"
            EXTRA="$EXTRA -monitor stdio"
            ;;
        /?)
            echo "Unknown option: -$OPTARG" >&2
            ;;
    esac
done

echo $VM $DEFAULTS $VIDEO $EXTRA
$VM $DEFAULTS $VIDEO $EXTRA
