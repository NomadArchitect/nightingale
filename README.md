# Nightingale

## About

Nightingale is an operating system for x86\_64 that I have been
developing for 7 years to learn about low-level programming and operating
system design.

Nightingale implements a mostly POSIX-like userland, though compliance is not a
goal. I see POSIX as useful as a well-understood and documented interface, and
one that permits compatability with large amounts of existing software.

![Screenshot](/doc/prompt2024.png?raw=true)

For more specific feature and capability information, see
[ABOUT.md](/doc/ABOUT.md).

## Building nightingale

Nightingale uses the CMake build system, and you can find package
requirements in the GitHub Actions workflow files.

- To build, execute `make` in the root of the project.
- To run, use `./run.rb` - its help text will show the available options

## Project map

### Directories

- `doc`: Documentation
- `include`: Header files for the whole system
- `interface`: Interface definitions for syscalls and errno values
- `kernel`: The core of the operating system
- `libc`: Common userland routines, including things like `printf`
- `linker`: Kernel module loader, userland dynamic linker, and `libelf`
- `script`: Utility scripts for building and developing nightingale
- `toolchain`: CMake toolchain files
- `user`: In-tree usermode programs distributed with the system

### Scripts

- `bt.bash`: convenience wrapper around `addr2line` for backtracing
- `dump.bash`: convenience wrapper around `objdump`
- `format.bash`: convenience wrapper around `clang-format`
- `make.bash`: core build engine for the project
- `run.rb`: convenience wrapper around `qemu-system-x86_64`

### Interface Manifests

These manifest files define the public syscall interface of the nightingale
kernel, they are rendered into C enums and metadata that is used by both the
kernel and the C library.

- `ERRNOS`: defines the values of `errno`, their names, and their `perror`
  strings
- `SYSCALLS`: defines syscall numbers, types, and arguments
