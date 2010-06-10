CC=cl
CFLAGS= /nologo /MT /O2 /TC /Zi
LINKS=/link kernel32.lib /OUT:md5index.exe /SUBSYSTEM:CONSOLE

all: md5index 

md5index: md5index.c
	$(CC) $(CFLAGS) md5index.c md5.c $(LINKS)

clean:
	del *.exe; del *.obj *.pdb *.ilk
