#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>

FILE * fp;

void writeheader(char * fmt,...)
{
   if(!fmt)
      return;

   if(!fp)
      return;

   va_list varlist;
   va_start(varlist,fmt);
   vfprintf(fp,fmt,varlist);
   va_end(varlist);

}

char * upper (char * string)
{
   int i;
   if (*string >='a'  && *string <= 'z' || *string == ' ')
   {
      for( i = 0; string[i] != 0; i++ )
         string[i] = toupper( string[i] );
   }

   return string;
}


char * getFileName (char * fullpath)
{
   static char DOT[] = ".";
   char * dpart;
   char * fpart;

   if ((fpart = strrchr(fullpath, '\\')) == NULL)
   {
      if ((dpart = strdup(DOT)) == NULL)
         return fpart;

      if ((fpart = strdup(fullpath)) == NULL)
      {
         free(dpart);
         return fpart;
      }
     }
     else
    {
       if ((dpart = strdup(fullpath)) == NULL)
         return fpart;
      
       dpart[fpart - fullpath] = '0';
       if ((fpart = strdup(++fpart)) == NULL)
       {
          free(dpart);
          return fpart;
       }
    }
    return fpart;
}

int main(int argc, char* argv[])
{
   FILE* finput;
   long lSize = 0;
   char * memblock;
   size_t bytes;

   finput = fopen(argv[1],"rb");

   if (!finput)
      return 0;

   printf("File Opened: %s\n",argv[1]);
   fseek (finput, 0, SEEK_END);
   lSize = ftell(finput);
   rewind(finput);
   printf("Size %d\n",lSize);
   
   memblock = (char*)malloc(sizeof(char)*lSize);
   bytes = fread (memblock, 1, lSize, finput);
   
   char * filenameWithExt = getFileName(argv[1]);
   char filename[MAX_PATH] = "";
   char headername[MAX_PATH] = "";
   strncpy(filename,filenameWithExt, strrchr(filenameWithExt, '.') - filenameWithExt);

   sprintf(headername,"%s.h",filename);
   fp = fopen(headername,"a");
   
   writeheader("#define %s_SIZE %d\n\n",upper(filename),lSize);
   writeheader("static unsigned char %s[]=\n{\n\t",filename);
   
   int linecount = 0;
   for (int i = 0;i < lSize;i++)
   {
      writeheader("0x%02x",(unsigned char)memblock[i]);

      if(i != lSize-1)
         writeheader(",");

      linecount++;
      if (linecount == 17)
      {
         writeheader("\n\t");
         linecount = 0;
      }
   }
   writeheader("\n};");

   fflush(stdin);
   getchar();

   fclose(finput);
   fclose(fp);
}
