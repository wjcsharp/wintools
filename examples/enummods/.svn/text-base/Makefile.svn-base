CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link kernel32.lib /OUT:em.exe /SUBSYSTEM:CONSOLE

all: enummods 

enummods: enummods.c
	$(CC) $(CFLAGS) enummods.c $(LINKS)

clean:
	del *.exe; del *.obj
