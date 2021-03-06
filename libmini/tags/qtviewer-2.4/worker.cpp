// (c) by Stefan Roettger, licensed under GPL 2+

#include "worker.h"

WorkerThread::WorkerThread(QObject *parent)
   : QThread(parent), grid_worker()
{}

WorkerThread::~WorkerThread()
{
   abort_jobs();
}

void WorkerThread::run_job(Job *job)
{
   MINILOG("running job: "+job->get_id());
   MINILOG("job details:");
   MINILOG(job->to_strings().to_string("\n"));

   grid_worker::run_job(job);
}

void WorkerThread::abort_jobs()
{
   grid_worker::abort_jobs();

   MINILOG("aborted jobs");
}

void WorkerThread::block_jobs()
{
   mutex.lock();
}

void WorkerThread::unblock_jobs()
{
   mutex.unlock();
}

void WorkerThread::start_jobs()
{
   if (!isRunning())
      start(LowPriority);
}

void WorkerThread::wait4jobs()
{
   wait();
}

void WorkerThread::run()
{
   grid_worker::run();
}

void WorkerThread::set_progress(double percentage)
{
   grid_worker::set_progress(percentage);

   emit reportProgress(percentage, get_job_id(), get_jobs());
}

void WorkerThread::job_success(Job *job)
{
   MINILOG("finished job: "+job->get_id());
   MINILOG("job details:");
   MINILOG(job->to_strings().to_string("\n"));

   emit finishedJob(job->get_id(), job->get_all());
}

void WorkerThread::job_failure(Job *job, int errorcode)
{
   MINILOG("failed job: "+job->get_id());
   MINILOG("job details:");
   MINILOG(job->to_strings().to_string("\n"));

   emit failedJob(job->get_id(), job->get_all(), errorcode);
}
