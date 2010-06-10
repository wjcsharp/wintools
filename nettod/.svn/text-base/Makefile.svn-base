CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link kernel32.lib Netapi32.lib /OUT:nettod.exe /SUBSYSTEM:CONSOLE

all: nettod 

nettod: 
	$(CC) $(CFLAGS) nettod.c $(LINKS)

clean:
	del *.exe; del *.obj
