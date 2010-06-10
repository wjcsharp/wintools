CC=i386-mingw32-gcc
#CFLAGS= /nologo /MT /O2 /TC
CFLAGS=-x c
#LINKS=/link kernel32.lib /OUT:dl.exe /SUBSYSTEM:CONSOLE
LINKS=-o dl.exe 

all: jincheng 

drivelist: jincheng.c
	$(CC) $(CFLAGS) jincheng.c $(LINKS)

clean:
	del *.exe; del *.obj
