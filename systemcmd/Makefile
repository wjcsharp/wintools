CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link kernel32.lib  /OUT:SystemCmd.exe /SUBSYSTEM:CONSOLE
SERVICELINKS=/link kernel32.lib Advapi32.lib /OUT:SystemCmdSvc.exe /SUBSYSTEM:CONSOLE

all: systemcmdsvc 

systemcmdsvc: systemcmdsvc.c
	$(CC) $(CFLAGS) systemcmdsvc.c $(SERVICELINKS)

clean:
	del *.exe; del *.obj
