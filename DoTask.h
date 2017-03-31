#pragma once
#include "CommonDef.h"

class DoTask
{
private:
  string processorID;
  thread taskThread;

  bool idle = true;
  void doingTask(function<void(string)> cb, string cmd);
public:
  DoTask(string id);
  ~DoTask();
  void startTask(string tid, string pid, string procid, string bat, string addr, function<void(string)> cb);
  void killTask(string tid, string pid, string procid, string bat, function<void(string)> cb);
};