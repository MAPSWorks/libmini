// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#else
#include <sys/stat.h>
#include <time.h>
#define stat _stat
#endif

#include "minibase.h"
#include "ministring.h"

#include "miniio.h"

namespace miniio {

// check a file
int checkfile(const char *filename)
   {
   FILE *file;

   if ((file=fopen(filename,"rb"))==NULL) return(0);
   fclose(file);

   return(1);
   }

// check a file path to be absolute or relative
int checkfilepath(const char *filename)
   {
   unsigned int len;

   len=strlen(filename);

   if (len>0)
      if (*filename=='/') return(1);

#ifdef _WIN32
   if (len>0)
      if (*filename=='\\') return(1);

   if (len>2)
      if (isalpha(filename[0]) && filename[1]==':' &&
          (filename[2]=='\\' || filename[2]=='/')) return(3);
#endif

   return(0);
   }

// write a RAW file
void writefile(const char *filename,unsigned char *data,long long bytes)
   {
   FILE *file;

   if (bytes<1) ERRORMSG();

   if ((file=fopen(filename,"wb"))==NULL) IOERROR();
   if ((long long)fwrite(data,1,bytes,file)!=bytes) IOERROR();

   fclose(file);
   }

// read a RAW file
unsigned char *readfile(const char *filename,long long *bytes)
   {
   FILE *file;

   unsigned char *data;

   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   data=readfiled(file,bytes);

   fclose(file);

   return(data);
   }

// read from a RAW file
unsigned char *readfiled(FILE *file,long long *bytes)
   {
   const long long BLOCKSIZE=1<<20;

   unsigned char *data;
   long long cnt,blkcnt;

   data=NULL;
   cnt=0;

   do
      {
      if (data==NULL)
         {if ((data=(unsigned char *)malloc(BLOCKSIZE))==NULL) MEMERROR();}
      else
         if ((data=(unsigned char *)realloc(data,cnt+BLOCKSIZE))==NULL) MEMERROR();

      blkcnt=fread(&data[cnt],1,BLOCKSIZE,file);
      cnt+=blkcnt;
      }
   while (blkcnt==BLOCKSIZE);

   if (cnt==0)
      {
      free(data);
      return(NULL);
      }

   if ((data=(unsigned char *)realloc(data,cnt))==NULL) MEMERROR();

   *bytes=cnt;

   return(data);
   }

// write a string as RAW file
void writestring(const char *filename,const char *cstr)
   {writefile(filename,(unsigned char *)cstr,strlen(cstr));}

// write a ministring as RAW file
void writeministring(const char *filename,const ministring &str)
   {
   unsigned char *text;
   unsigned int size;

   if (str.empty()) ERRORMSG();

   size=str.getsize();

   if ((text=(unsigned char *)malloc(size))==NULL) MEMERROR();

   str.extract_array(text,size);

   writefile(filename,text,size);

   free(text);
   }

void writeministring(ministring filename,const ministring &str)
   {writeministring(filename.c_str(),str);}

// write ministrings as RAW file
void writeministrings(const char *filename,const ministrings &strs)
   {writeministring(filename,strs.to_string("\n"));}

void writeministrings(ministring filename,const ministrings &strs)
   {writeministrings(filename.c_str(),strs);}

// read a RAW file as string
char *readstring(const char *filename)
   {
   char *data;
   long long bytes;

   data=(char *)readfile(filename,&bytes);

   if (data==NULL) return(NULL);

   if ((data=(char *)realloc(data,bytes+1))==NULL) MEMERROR();
   data[bytes]='\0';

   return(data);
   }

// read a RAW file as ministring
ministring readministring(const char *filename)
   {
   unsigned char *data;
   long long bytes;

   ministring str;

   data=readfile(filename,&bytes);

   if (data==NULL) return("");

   str.append_array(data,bytes);

   free(data);

   return(str);
   }

ministring readministring(ministring filename)
   {return(readministring(filename.c_str()));}

// read a RAW file as ministrings
ministrings readministrings(const char *filename)
   {
   ministrings strs;

   strs.from_string(readministring(filename),"\n");

   return(strs);
   }

ministrings readministrings(ministring filename)
   {return(readministrings(filename.c_str()));}

// read a RAW file and compute signature
unsigned int signature(const char *filename)
   {
   unsigned int sig;

   unsigned char *data;
   long long bytes;

   sig=0;

   if ((data=readfile(filename,&bytes))!=NULL)
      {
      sig=signature(data,bytes);
      free(data);
      }

   return(sig);
   }

// compute a signature
unsigned int signature(const unsigned char *data,long long bytes)
   {
   long long i;

   unsigned int sig;

   const unsigned char *ptr;

   sig=0;
   ptr=data;

   for (i=0; i<bytes; i++) sig=((271*sig)^34+(*ptr++));

   return(sig);
   }

// get file modification year
int getmodyear(const char *filename)
   {
   struct tm* clock;
   struct stat attrib;

   if (stat(filename,&attrib)!=0) return(-1);

   clock=gmtime(&(attrib.st_mtime));

   return(clock->tm_year);
   }

// get file modification time relative to year
long long int getmodtime(const char *filename,int year)
   {
   struct tm* clock;
   struct stat attrib;

   if (year<0) return(0);
   if (stat(filename,&attrib)!=0) return(0);

   clock=gmtime(&(attrib.st_mtime));

   return(clock->tm_sec+
          60*(clock->tm_min+
              60*(clock->tm_hour+
                  24*(clock->tm_mday+
                      31*(clock->tm_mon+
                          365*(clock->tm_year-(long long int)year))))));
   }

// get relative modification time
long long int getreltime(const char *file1,const char *file2)
   {
   int year=getmodyear(file1);
   long long int delta1=getmodtime(file1,year);
   long long int delta2=getmodtime(file2,year);
   return(delta1-delta2);
   }

}
