# reverse_utils
Collection of Tools I used during my journey of Reverse Engineering

# Contents : 
`kernel_hack` - very simple example of a kernel driver and how to operate it. It shows basics for driver load/unload and simple interaction from userland. I used it to modify game memory without getting detected.

`dumper_sc` - A simple memory monitoring tool. This project monitors native libraries by PID and library name. Useful for learning how to track and dump in-memory native modules.

`GAPIS` - This folder contains Android syscall experiments. I tried to spoof tgid and call functions via syscalls to test bypassing system-level logs. Mostly syscall-level proof-of-concept.

`Linux` - This folder contains pointer experiments: injecting function code into other process and mapping it. Also contains simple TCP/UDP flood scripts to test raw link speed and behaviour. There's also a custom `malloc()` and `free()` function that can detect memory leaks caused by user. 

`aob_search.cpp` - simple AOB (array-of-bytes) scanner for pattern scanning in memory.

`net_scan.c` - basic TCP packet scanner for quick network probing.
