CC=cl
CFLAGS= /nologo /MT /O2
LINKS=/link user32.lib kernel32.lib version.lib /OUT:exeinfo.exe /SUBSYSTEM:CONSOLE

all: exeinfo 

exeinfo: exeinfo.c
	$(CC) $(CFLAGS) exeinfo.c $(LINKS)

clean:
	del *.exe; del *.obj
