# Files in Linux

**Everything is a File**

- **All interfaces(attributes) between user-space and kernel is located in `sys`**.


## File Descriptors.
- Any process will be running in linux it will automatically create three files and they re located `/proc//fd`

## Cpp with file descriptor
> To interact with files in cpp we can use `fstream` library.


## System Calls in Cpp.
> It's the part of the interface that the kernel provides for the user-space to access hardware.
- **SPC**: Simple Procedures Call
    - Normal Function call.
- **RPC**: Remote Procedures Call
    - A system call which is called through a remote device.

## Posix
> Portable Operating System Interface.
- System calls are considered as posix interface(function).
- This means the posix interface depending on a standard.


## Cross Compilation:
> crosstool-ng: Creates a cross/native compiler based on some configuration.

## How to compile without system calls.

