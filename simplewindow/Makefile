CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link /OUT:simplewindow.exe /SUBSYSTEM:WINDOWS

all: simplewindow 

simplewindow: simplewindow.c
	$(CC) $(CFLAGS) simplewindow.c $(LINKS)

clean:
	del *.exe; del *.obj
