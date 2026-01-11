!#/usr/bin/env bash
set -e

cd "$(dirname "$0")/.." || exit 1

qemu-system-i386 -cdrom spoolyos.iso
