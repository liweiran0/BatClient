#pragma once
#include "CommonDef.h"

class DoTask
{
private:
  int processorID;
  thread taskThread;
  bool idle = true;
  void doingTask(function<void(string)> cb, string cmd);
public:
  DoTask(int id);
  ~DoTask();
  void startTask(int tid, int pid, int porid, string addr, function<void(string)> cb);
};