CC=cl
CFLAGS= /nologo /MT /O2 /TC /Zi
LINKS=/link kernel32.lib /OUT:adssearch.exe /SUBSYSTEM:CONSOLE

all: adssearch 

adssearch: adssearch.c
	$(CC) $(CFLAGS) adssearch.c $(LINKS)

clean:
	del *.exe; del *.obj *.pdb *.ilk
