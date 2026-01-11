#!/usr/bin/env python3

# (i hate myself)
# this is kinda retarded but so is the rest of this project

import os
import subprocess
import shutil
import sys

PROJECT_ROOT = os.path.abspath(os.path.dirname(__file__) + "/..")
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
ISO_DIR = os.path.join(PROJECT_ROOT, "isodir", "boot")
GRUB_CFG = os.path.join(PROJECT_ROOT, "grub", "grub.cfg")
KERNEL_BIN = os.path.join(BUILD_DIR, "kernel", "spoolyos.bin")
ISO_BIN = os.path.join(ISO_DIR, "spoolyos.bin")
COMPILE_COMMANDS = os.path.join(BUILD_DIR, "compile_commands.json")
LINKED_COMPILE_COMMANDS = os.path.join(PROJECT_ROOT, "compile_commands.json")

def run(cmd, cwd=None, fatal=True):
    result = subprocess.run(cmd, cwd=cwd)
    if result.returncode != 0 and fatal:
        print(f"[spoolybuild]: command failed: {' '.join(cmd)}")
        sys.exit(1)


def build_kernel():
    os.makedirs(BUILD_DIR, exist_ok=True)

    cmake_config_cmd = [
        "cmake",
        "-S", PROJECT_ROOT,
        "-B", BUILD_DIR,
        f"-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    ]
    run(cmake_config_cmd)

    cmake_build_cmd = [
        "cmake",
        "--build", BUILD_DIR
    ]
    run(cmake_build_cmd)


def copy_kernel_to_iso():
    os.makedirs(ISO_DIR, exist_ok=True)
    shutil.copy(KERNEL_BIN, ISO_BIN)
    print(f"[spoolybuild]: copied {KERNEL_BIN} → {ISO_BIN}")


def make_iso():
    if not os.path.isfile(GRUB_CFG):
        print(f"[spoolybuild]: grub.cfg not found at {GRUB_CFG}")
        sys.exit(1)

    cmd = ["grub-mkrescue", "-o", os.path.join(PROJECT_ROOT, "spoolyos.iso"), os.path.join(PROJECT_ROOT, "isodir")]
    run(cmd)
    print("[spoolybuild]: we got spoolyos.iso")


def symlink_compile_commands():
    if os.path.exists(LINKED_COMPILE_COMMANDS) or os.path.islink(LINKED_COMPILE_COMMANDS):
        os.remove(LINKED_COMPILE_COMMANDS)
    os.symlink(COMPILE_COMMANDS, LINKED_COMPILE_COMMANDS)
    print(f"[spoolybuild]: symlinked compile_commands.json → {LINKED_COMPILE_COMMANDS}")


def main():
    print("[spoolybuild]: building spoolyos.bin trust")
    build_kernel()
    copy_kernel_to_iso()
    make_iso()
    symlink_compile_commands()
    print("[spoolybuild]: build complete")


if __name__ == "__main__":
    main()
