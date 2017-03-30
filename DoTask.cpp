#include "DoTask.h"

DoTask::DoTask(int id): processorID(id)
{
  
}

DoTask::~DoTask()
{
  
}

void DoTask::startTask(int tid, int pid, int porID, string addr, function<void(string)> cb)
{
  string cmd = to_string(tid) + ":" + to_string(pid) + ":" + to_string(porID);
  //cout << " start task cmd = " << cmd << endl;
  taskThread = thread(&DoTask::doingTask, this, cb, cmd);
  taskThread.detach();
}

void DoTask::doingTask(function<void(string)> cb, string cmd)
{
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> distribution(50, 60);
  int time = distribution(gen);
  this_thread::sleep_for(chrono::seconds(time));
  cb(cmd);
}
