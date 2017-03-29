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
  void callback(string cmd);
  int coreNumber;
  string localIP;
  vector<shared_ptr<DoTask>> doTasks;
  string remoteIP;
  int remotePort;
};
