#include "DoTask.h"

DoTask::DoTask(string id): processorID(id)
{
  
}

DoTask::~DoTask()
{
  
}

void DoTask::startTask(string tid, string pid, string procid, string bat, string addr, Callback cb, Callback cb2)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  processInfo.set(tid, pid, bat, addr);
  //taskid="taskID":processid="processID":coreid="processorID"
  //cout << " start task cmd = " << cmd << endl;
  taskThread = thread(&DoTask::doingTask, this, cb, cmd);
}

void DoTask::doingTask(Callback cb, string cmd)
{
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> distribution(10, 15);
  int time = distribution(gen);
  this_thread::sleep_for(chrono::seconds(time));
  taskThread.detach();
  processInfo.reset();
  cb(cmd);
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
  }
  else
  {
    //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
    cmd = "order=\"kill\":" + cmd;
    cb2(cmd);
  }
  
}
