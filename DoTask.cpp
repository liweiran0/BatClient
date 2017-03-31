#include "DoTask.h"

DoTask::DoTask(string id): processorID(id)
{
  
}

DoTask::~DoTask()
{
  
}

void DoTask::startTask(string tid, string pid, string procid, string bat, string addr, function<void(string)> cb)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  //taskid="taskID":processid="processID":coreid="processorID"
  //cout << " start task cmd = " << cmd << endl;
  taskThread = thread(&DoTask::doingTask, this, cb, cmd);
  
}

void DoTask::doingTask(function<void(string)> cb, string cmd)
{
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> distribution(55, 60);
  int time = distribution(gen);
  this_thread::sleep_for(chrono::seconds(time));
  taskThread.detach();
  cb(cmd);
}

void DoTask::killTask(string tid, string pid, string procid, string bat, function<void(string)> cb)
{
  ::TerminateThread(taskThread.native_handle(), 1);
  taskThread.detach();
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
  cb(cmd);
}
