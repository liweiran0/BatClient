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

class Task
{
public:
  Task(string cmd);
  Task();
  Task(function<void()> f);
  ~Task();
  BOOL terminateExe();
  PROCESS_INFORMATION* start();
private:
  STARTUPINFO         m_si;
  PROCESS_INFORMATION m_pi;
  string cmd;
  function<void()> cb;
  BOOL exeCmd();
};

class DoTask
{
private:
  string processorID;
  thread taskThread;
  ProcessInfo processInfo;
  bool idle = true;
  queue<shared_ptr<Task>> taskQueue;
  mutex queueMutex;
  condition_variable cv;
  void startThread();
  shared_ptr<Task> currentTask;
public:
  DoTask(string id);
  ~DoTask();
  void workingThread();
  void startTask(string tid, string pid, string procid, string bat, string addr, Callback cb, Callback cb2);
  void killTask(string tid, string pid, string procid, string bat, Callback cb, Callback cb2);
};