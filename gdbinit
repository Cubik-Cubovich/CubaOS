# gdbinit
target remote localhost:1234
symbol-file build/bin/kernel.elf
break kernel_main
continue