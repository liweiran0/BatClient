#include "Manager.h"
#include "ClientNet.h"

Manager::Manager(int cores, string ip)
{
  localIP = ip;
  coreNumber = cores;
  for (auto i = 0; i < cores; i++)
  {
    shared_ptr<DoTask> task(new DoTask(i));
    doTasks.push_back(task);
  }
}

void Manager::cmdCallback(string cmd, SOCKET sock)
{
  if (cmd.find("start") == 0)
  {
    //start:tid:taskName:pid:processor
    string tmp;
    int taskID;
    string taskName;
    int processID;
    int processor;
    tmp = cmd.substr(6);
    taskID = stoi(tmp.substr(0, tmp.find_first_of(":")));
    tmp = tmp.substr(tmp.find_first_of(":") + 1);
    taskName = tmp.substr(0, tmp.find_first_of(":"));
    tmp = tmp.substr(tmp.find_first_of(":") + 1);
    processID = stoi(tmp.substr(0, tmp.find_first_of(":")));
    tmp = tmp.substr(tmp.find_first_of(":") + 1);
    processor = processID = stoi(tmp.substr(0, tmp.find_first_of(":")));
    tmp = tmp.substr(tmp.find_first_of(":") + 1);
    string addr = tmp;
    doTasks[processID]->startTask(taskID, processID, processor, addr, bind(&Manager::callback, this, placeholders::_1));
  }
}

void Manager::setRemote(string ip, int port)
{
  remoteIP = ip;
  remotePort = port;
}

void Manager::callback(string cmd)
{
  ClientNet client;
  cmd = "finish:" + localIP + ":" + cmd;
  client.Connect(remoteIP.c_str(), remotePort);
  client.SendMsg(cmd);
  client.Close();
}
