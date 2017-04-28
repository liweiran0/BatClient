#include "DoTask.h"

DoTask::DoTask(string id): processorID(id), queueMutex(), cv()
{
  startThread();
}

DoTask::~DoTask()
{
  
}

void DoTask::workingThread()
{
  while (true)
  {
    Task task;
    {
      unique_lock<mutex> lck(queueMutex);
      while (taskQueue.empty())
      {
        cv.wait(lck);
      }
      task = taskQueue.front();
      taskQueue.pop();
    }
    if (task)
    {
      task();
    }
  }
}

void DoTask::startThread()
{
  taskThread = thread(&DoTask::workingThread, this);
}


void DoTask::startTask(string tid, string pid, string procid, string bat, string addr, Callback cb, Callback cb2)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  processInfo.set(tid, pid, bat, addr);
  //taskid="taskID":processid="processID":coreid="processorID"
  //cout << " start task cmd = " << cmd << endl;
  string sys_cmd1 = addr + bat;
  string sys_cmd2 = addr + "point_postprocessing.bat";

  {
    unique_lock<mutex> lck(queueMutex);
    taskQueue.push([=]()
    {
      system(sys_cmd1.c_str());
    });
    taskQueue.push([=]()
    {
      system(sys_cmd2.c_str());
    });
    taskQueue.push([=]()
    {
      cb(cmd);
    });
    cv.notify_one();
  }
}



void DoTask::killTask(string tid, string pid, string procid, string bat, Callback cb, Callback cb2)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  if (processInfo.tid == tid && processInfo.pid == pid)
  {
    ::TerminateThread(taskThread.native_handle(), 1);
    //taskThread.detach();
    taskThread.join();
    //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
    cb(cmd);
    startThread();
  }
  else
  {
    //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
    cmd = "order=\"kill\":" + cmd;
    cb2(cmd);
  }
  
}
