#pragma once
#include "CommonDef.h"
#include "DoTask.h"
class Manager
{
public:
  void cmdCallback(string cmd, SOCKET sock);
  Manager(int cores, string ip);
  void setRemote(string ip, int port);
private:
  void finishCallback(string cmd);
  void killCallback(string cmd);
  void failedCallback(string cmd);
  void parseCommand(string cmd, map<string, string>& param);
  void callTask(map<string, string>param);
  int coreNumber;
  string localIP;
  map<string, shared_ptr<DoTask>> doTasks;
  string remoteIP;
  int remotePort;
  thread workdingThread;
};
