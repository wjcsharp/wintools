/*
    Ca0s Crypt v1
    Crypts files playing with bytes.
    Two types:
         /- Byte
        ^   Byte
    Includes 19856 bytes stub for working with exe's. If you compile the stub and size is different, you have to
        change 'stubSize' variable or set it when calling the program, with argv.
       
    [st4ck-3rr0r.blogspot.com]
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
 
void uso();
 
int isExe=0, jobMode=0, cryptMode=1, x, originalSize, stubSize=19856, error=0;
FILE *originalFile, *stubFile, *newFile;
struct stat myStat;
char *tmpCByte;
 
char hexConv[32]={'0', 0x00, '1', 0x01, '2', 0x02, '3', 0x03, '4', 0x04, '5', 0x05, '6', 0x06, '7', 0x07, '8', 0x08, '9', 0x09, 'A', 0x0A, 'B', 0x0B, 'C', 0x0C, 'D', 0x0D, 'E', 0x0E, 'F', 0x0F};
 
int main(int argc, char *argv[])
{
  printf("Ca0s Crypt v1\n\n");
 
  char *fileName, *newFileName;
  char *cByte=(char *)malloc(1);
  for(x=0; x<argc; x  )
  {
        if(strcmp(argv[x], "-exe")==0) isExe=1;
        if((strcmp(argv[x], "-file")==0)    && (argc>(x 1))) fileName=argv[x 1];
        if((strcmp(argv[x], "-w")==0)       && (argc>(x 1))) newFileName=argv[x 1];  
        if((strcmp(argv[x], "-job")==0)     && (argc>(x 1)))
        {
            if(strcmp(argv[x 1], "crypt")==0) jobMode=1;
            else if(strcmp(argv[x 1], "decrypt")==0) jobMode=2;
        }
        if((strcmp(argv[x], "-crypt")==0)   && (argc>(x 1)))
        {
            if(strcmp(argv[x 1], "1")==0) cryptMode=1; //  /- 0x20
            if(strcmp(argv[x 1], "2")==0) cryptMode=2; // ^0x20
        }
        if((strcmp(argv[x], "-byte")==0)    && (argc>(x 1)))
        {
            if(strlen(argv[x 1])==2)
            {
                tmpCByte=(char *)malloc(1);
                *tmpCByte=0x00;
                char *argByte=(char *)malloc(2);
                memcpy(argByte, argv[x 1], 2);
                char *conversion=(char *)malloc(1);
                int y=0, z=0, good=0;
                for(y=0; y<=1; y  )
                {
                    good=0;
                    memcpy(conversion, argByte y, 1);
                    for(z=0; z<31; z  )
                    {
                        if(toupper(*conversion) == hexConv[z])
                        {
                            good=1;
                            if((y==0) && (hexConv[z 1]!=0x00)) *tmpCByte =hexConv[z 1]*16;
                            else *tmpCByte =hexConv[z 1];
                            break;
                        }
                    }
                    if(good==0)
                    {
                        error=1;
                        break;
                    }
                }
            }
            else error=1;
        }
        if((strcmp(argv[x], "-stubsize")==0)    && (argc>(x 1))) stubSize=atoi(argv[x 1]);
  }
 
  if((fileName==NULL) || (newFileName==NULL) ||(jobMode==0) || (error==1)) uso();
 
  printf("Original file: %s\n", fileName);
  originalFile=fopen(fileName, "rb");
  if(originalFile==NULL)
  {
        printf("Error: can't open file to crypt.\n");
        return 0;
  }
  fstat(fileno(originalFile), &myStat);
  originalSize=myStat.st_size;
  printf("Size: %d bytes.\n", originalSize);
 
  newFile=fopen(newFileName, "wb");
  if(newFile==NULL)
  {
        printf("Error: can't create output file.\n");
        return 0;
  }
 
  if(jobMode==1) printf("Job: crypt.\n");
  else if(jobMode==2) printf("Job: decrypt.\n");
 
  if(cryptMode==1) printf("Mode:  /- BYTE\n");
  else if(cryptMode==2) printf("Mode: ^ BYTE\n");
 
  if(tmpCByte==NULL)
  {
        *cByte=0x20;
        printf("Using default byte (0x20).\n");
  }
  else
  {
        cByte=tmpCByte;
        printf("Using byte 0x%x.\n", (unsigned char)*cByte);
  }
 
  if(isExe==1) printf("Working with a EXE. Using stub.\n");
  if((isExe==1) && (jobMode==2))
  {
        printf("Stub's size: %d bytes.\n", stubSize);
        stubSize =2;
  }
 
  char *originalBuffer=(char *)malloc(originalSize);
  char *tmpByte1=(char *)malloc(1);
  char *tmpByte2=(char *)malloc(1);
  fread(originalBuffer, originalSize, 1, originalFile);
 
  if((isExe==1) && (jobMode==1))
  {
        char *modeByte=(char *)malloc(1);
        switch(cryptMode)
        {
            case 1:
                *modeByte=0x01;
                break;
            case 2:
                *modeByte=0x02;
                break;
        }
        FILE *myStub=fopen("stub.exe", "rb");
        if(myStub==NULL)
        {
            printf("Error: can't open stub.\n");
            return 0;
        }
        while(fread(tmpByte2, 1, 1, myStub)) fwrite(tmpByte2, 1, 1, newFile);
        fclose(myStub);
        fwrite(modeByte, 1, 1, newFile);
        fwrite(cByte, 1, 1, newFile);
  }
  if((isExe==1) && (jobMode==2))
  {
        originalBuffer =stubSize;
        originalSize-=stubSize;
  }
 
  for(x=0; x<originalSize; x  )
  {
        memcpy(tmpByte1, originalBuffer x, 1);
        if(jobMode==1)
        {
            switch(cryptMode)
            {
                case 1:
                    *tmpByte1 =*cByte;
                    break;
                case 2:
                    *tmpByte1^=*cByte;
                    break;
            }
            fwrite(tmpByte1, 1, 1, newFile);
        }
        else if(jobMode==2)
        {
            switch(cryptMode)
            {
                case 1:
                    *tmpByte1-=*cByte;
                    break;
                case 2:
                    *tmpByte1^=*cByte;
                    break;
            }
            fwrite(tmpByte1, 1, 1, newFile);
        }
  }
 
  fclose(originalFile);
  fclose(newFile);
  printf("\nTerminado. Archivo creado en %s\n", newFileName);
  return 0;
}
 
void uso()
{
    printf("Mandatory args:\n\t-file PATH\t\tSets file to crypt\n\t-w PATH\t\t\tSets output file\n\t-job [crypt|decrypt]\tWhat to do\n");
    printf("Optional args:\n\t-exe\t\t\tInclude stub, for exe's\n\t-byte BYTE (00-FF)\tChanges crypt-byte (default 0x20)\n\t-crypt 1/2\t\tCrypt mode. 1( -BYTE) 2(^BYTE)\n\t-stubsize SIZE\tSets stub size when working with a exe. Default: 19856\n");
    exit(0);
}