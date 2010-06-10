CC=cl
CFLAGS= /nologo /MT /O2 /TC /Zi
LINKS=/link kernel32.lib /OUT:md5search.exe /SUBSYSTEM:CONSOLE

all: md5search 

md5search: md5search.c
	$(CC) $(CFLAGS) md5search.c md5.c $(LINKS)

clean:
	del *.exe; del *.obj *.pdb *.ilk
