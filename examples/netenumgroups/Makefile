CC=cl
CFLAGS= /nologo /MT /O2
LINKS=/link kernel32.lib Netapi32.lib /OUT:netenumgroups.exe /SUBSYSTEM:CONSOLE

all: netenumgroups 

netenumgroups: 
	$(CC) $(CFLAGS) netenumgroups.c $(LINKS)

clean:
	del *.exe; del *.obj
