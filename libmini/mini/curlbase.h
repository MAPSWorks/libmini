// (c) by Stefan Roettger

#ifndef CURLBASE_H
#define CURLBASE_H

#include "minibase.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

class curlbase
   {
   public:

   curlbase();
   ~curlbase();

   static void curlinit(int threads,int id,char *proxyname,char *proxyport,void *data);
   static void curlexit(int id,void *data);

   static void getURL(char *src_url,char *src_id,char *src_file,char *dst_file,int background,int id,void *data);
   static int checkURL(char *src_url,char *src_id,char *src_file,int id,void *data);

   private:

   struct MemoryStruct
      {
      char *memory;
      size_t size;
      };

   typedef CURL *CURLPTR;

   struct MULTICURL_STRUCT
      {
      int numthreads;
      CURLPTR *curl_handle;
      };

   typedef MULTICURL_STRUCT MULTICURL_TYPE;

   MULTICURL_TYPE **MULTICURL;
   int MAXMULTICURL,NUMMULTICURL;

   static int INSTANCES;

   void curlinit_safe(int threads,int id,char *proxyname,char *proxyport);
   void curlexit_safe(int id);

   void getURL_safe(char *src_url,char *src_id,char *src_file,char *dst_file,int background,int id);
   int checkURL_safe(char *src_url,char *src_id,char *src_file,int id);

   void initmulticurl(int id);
   void exitmulticurl(int id);

   static void *myrealloc(void *ptr,size_t size);
   static size_t WriteMemoryCallback(void *ptr,size_t size,size_t nmemb,void *data);
   };

#endif
