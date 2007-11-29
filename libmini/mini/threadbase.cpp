// (c) by Stefan Roettger

#include "threadbase.h"

int threadbase::numthreads;

threadbase::PTHREADPTR threadbase::pthread;
pthread_mutex_t threadbase::mutex,threadbase::iomutex;
pthread_attr_t threadbase::attr;

void threadbase::threadinit(int threads,int id,void *data)
   {
#ifdef PTW32_STATIC_LIB
   pthread_win32_process_attach_np();
#endif

   numthreads=threads;

   pthread=new pthread_t[numthreads];

   pthread_mutex_init(&mutex,NULL);
   pthread_mutex_init(&iomutex,NULL);

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
   }

void threadbase::threadexit(int id,void *data)
   {
   pthread_mutex_destroy(&mutex);
   pthread_mutex_destroy(&iomutex);

   pthread_attr_destroy(&attr);

   delete[] pthread;

#ifdef PTW32_STATIC_LIB
   pthread_win32_process_detach_np();
#endif
   }

void threadbase::startthread(void *(*thread)(void *background),backarrayelem *background,int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_create(&pthread[background->background-1],&attr,thread,background);
   }

void threadbase::jointhread(backarrayelem *background,int id,void *data)
   {
   void *status;

   if (data!=NULL) ERRORMSG();
   pthread_join(pthread[background->background-1],&status);
   }

void threadbase::lock_cs(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_lock(&mutex);
   }

void threadbase::unlock_cs(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_unlock(&mutex);
   }

void threadbase::lock_io(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_lock(&iomutex);
   }

void threadbase::unlock_io(int id,void *data)
   {
   if (data!=NULL) ERRORMSG();
   pthread_mutex_unlock(&iomutex);
   }
