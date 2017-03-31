#include "DoTask.h"

DoTask::DoTask(string id): processorID(id)
{
  
}

DoTask::~DoTask()
{
  
}

void DoTask::startTask(string tid, string pid, string procID, string addr, function<void(string)> cb)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procID + "\"";
  //taskid="taskID":processid="processID":coreid="processorID"
  //cout << " start task cmd = " << cmd << endl;
  taskThread = thread(&DoTask::doingTask, this, cb, cmd);
  taskThread.detach();
}

void DoTask::doingTask(function<void(string)> cb, string cmd)
{
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> distribution(5, 10);
  int time = distribution(gen);
  this_thread::sleep_for(chrono::seconds(time));
  cb(cmd);
}
