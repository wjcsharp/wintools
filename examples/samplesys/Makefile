#Linux version:
CROSS=i386-mingw32-

CC=$(CROSS)gcc

all: simple.sys

# tell the compiler/linker to use the native subsystem
# the entrypoint of the program is _DriverEntry@8
# for the linker to find _DriverEntry@8, the function's calling
# convention must be STDCALL.
# gcc can set this with __attribute__((__stdcall__)).

simple.sys: 
	$(CC) -Wall -s $^ -Wl,--subsystem,native -Wl,--entry,_DriverEntry@8 -nostartfiles -lntoskrnl -lhal -nostdlib -shared -o $@
clean:
	rm -f *.sys *~ *.o