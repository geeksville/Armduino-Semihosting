# stm32duino-semihosting
Note: This is a fork of [STM32duino-Semihosting](https://github.com/koendv/STM32duino-Semihosting) by @koendv.
Summary of minor improvements (virtually identical to the STM32 version):

  * Larger output buffer (256 bytes)
  * SemihostingStreams now need to be explicitly opened.  This allows better control of when we talk to the debugger (or not)
  * It should work on any cortex ARM.

With semihosting an arduino arm system can do keyboard input, screen output, and file I/O on the pc where the debugger is running. To run a semihosting program, you need a debugger probe to connect the program on your arduino to the debugger on your pc. Semihosting only runs on systems with arm processors, sorry.

# Installation
* connect a debugger probe with semihosting support, e.g. a [segger](https://www.segger.com/products/debug-probes/j-link/) or a [black magic probe](https://github.com/blacksphere/blackmagic/wiki).
* run examples from File→Examples→STM32duino-Semihosting

# Arduino Use
Use SemihostingStream where you would use Serial:
```
#include <SemihostingStream.h>
SemihostingStream sh;
void setup() {
  sh.open();
  sh.println("hello world!");
}
void loop() {
}
```
In the Arduino IDE, choose *Tools→Upload Method→BMP (Black Magic Probe)* and *Tools→Optimize→Debug (-g)*.

Every debugger probe is a bit different. When using e.g. Black Magic Probe you typically would use commands like:
```
koen@raspberrypi:~ $ gdb -q
(gdb) target extended-remote /dev/ttyBmpGdb
(gdb) monitor swdp_scan
Available Targets:
No. Att Driver
 1      STM32F1 medium density M3/M4
(gdb) attach 1
(gdb) file /tmp/arduino_build_195867/HelloWorld.ino.elf
(gdb) load
(gdb) run
hello world!
^C
(gdb) where
(gdb) quit
```

Remember a semihosting program needs a debugger probe to run. If no debugger probe is present, the program will *hang*.

# Source

## SemihostingStream
The SemihostingStream Arduino class allows you to write to and read from the semihosting console. This class provides buffered output to stdout and buffered input from stdin. 
## SemihostingPrint
The SemihostingPrint Arduino class allows you to write to the semihosting debug output. This class provides unbuffered output to stderr. 
## semihosting_syscalls
The semihosting_syscalls C++ source gives you access to the raw semihosting calls.
## semihosting_bmp
The semihosting_bmp C++ header file contains the error numbers for Black Magic Probe debuggers.

# Semihosting calls
Not every debugger probe implements every semihosting call, and even if it's implemented you may get some surprises.

function       |JLink|BMP|   |
-----------------|---|---|---|
sys_clock        |   | * | Returns the number of centiseconds (hundredths of a second) since execution started.
sys_close        | * | * | Closes a file on the host system.
sys_elapsed      |   |   | Returns the number of elapsed target ticks since execution started.
sys_errno        |   | * | Returns the value of the C library errno variable.
sys_exit         | * | * | Report to the debugger that execution has completed.
sys_exit         |   | * | Report to the debugger that execution has completed. (64-bit version)
sys_flen         | * | * | Returns the length of a specified file.
sys_getcmdline   | * | * | Returns the command line that is used for the call to the executable (= argc and argv).
sys_heapinfo     |   | * | Returns the system stack and heap parameters. Used in crt0.S.
sys_iserror      |   | * | Determines whether the return code from another semihosting call is an error status or not.
sys_istty        | * | * | Checks whether a file is connected to an interactive device.
sys_open         | * | * | Opens a file on the host system.
sys_read         | * | * | Reads the contents of a file into a buffer.
sys_readc        | * | * | Reads a byte from the console.
sys_remove       | * | * | Deletes a specified file on the host filing system.
sys_rename       | * | * | Renames a specified file.
sys_seek         | * | * | Seeks to a specified position in a file using an offset specified from the start of the file.
sys_system       |   | * | Passes a command to the host command-line interpreter.
sys_tickfreq     |   |   | Returns the number of target ticks per second.
sys_time         |   | * | Returns the number of seconds since 00:00 January 1, 1970.
sys_tmpnam       |   | * | Returns a temporary name for a file identified by a system file identifier.
sys_write        | * | * | Writes the contents of a buffer to a specified file at the current file position.
sys_writec       | * | * | Writes a character byte to the debug channel.
sys_write0       | * | * | Writes a null-terminated string to the debug channel.

J-Link data from document UM08001, software version 6.70, March 27, 2020.
Black Magic Probe data from the source file ``src/target/cortexm.c``.

# Notes 

The semihosting specification is Arm document [Semihosting for AArch32 and AArch64](https://www.google.com/search?q=Semihosting+for+AArch32+and+AArch64+site%3A.arm.com)

SYS_GET_CMDLINE returns the command line arguments, separated by spaces. bmp: If a command line argument contains spaces, the spaces are escaped using backslashes. gdb: Set command line arguments using ``set args`` or as parameters to the ``run`` command.
 
Error numbers reported by SYS_ERRNO are system and debugger dependent. bmp: Error numbers used are from gdb filei/o. See gdb source file ``gdb/include/gdb/fileio.h`` 

SYS_SYSTEM is used to execute a command on the host. bmp: Allow command execution using gdb command ``set remote system-call-allowed 1``

SYS_HEAPINFO is used to set heap and stack during target boot. See newlib source file ``newlib/libc/sys/arm/crt0.S``. bmp: Set heapinfo using gdb command ``monitor heapinfo``

bmp: If the target wants to read the special filename ":semihosting-features" to know what semihosting features are supported, it's easiest to create that file on the host in the directory where gdb runs:
```
$ echo -e 'SHFB\x03' > ":semihosting-features"
$ chmod 0444 ":semihosting-features"
```

The [Qemu](http://www.qemu.org) emulator and the [openocd](http://www.openocd.org) debugger are open source projects that also support semihosting and gdb.

If you find errors, omissions, or have additional data, feel free to open an issue.
