CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link kernel32.lib /OUT:pedump.exe /SUBSYSTEM:CONSOLE

all: pedump 

pedump: pedump.c
	$(CC) $(CFLAGS) pedump.c $(LINKS)

clean:
	del *.exe; del *.obj
