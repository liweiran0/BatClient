#pragma once
#include "CommonDef.h"

struct ProcessInfo
{
  string tid;
  string pid;
  string bat;
  string logdir;
  void reset()
  {
    tid = pid = bat = logdir = "";
  }
  void set(string& tid, string& pid, string& bat, string& logdir)
  {
    this->tid = tid;
    this->pid = pid;
    this->bat = bat;
    this->logdir = logdir;
  }
};

typedef function<void()> Task;
class DoTask
{
private:
  string processorID;
  thread taskThread;
  ProcessInfo processInfo;
  bool idle = true;
  queue<Task> taskQueue;
  mutex queueMutex;
  condition_variable cv;
  void startThread();
public:
  DoTask(string id);
  ~DoTask();
  void workingThread();
  void startTask(string tid, string pid, string procid, string bat, string addr, Callback cb, Callback cb2);
  void killTask(string tid, string pid, string procid, string bat, Callback cb, Callback cb2);
};