CC=cl
CFLAGS= /nologo /MT /O2 /TC 
LINKS=/link kernel32.lib /OUT:systeminfo.exe /SUBSYSTEM:CONSOLE

all: systeminfo 

systeminfo: systeminfo.c
	$(CC) $(CFLAGS) systeminfo.c $(LINKS)

clean:
	del *.exe; del *.obj
