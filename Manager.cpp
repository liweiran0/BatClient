#include "Manager.h"
#include "ClientNet.h"

Manager::Manager(int cores, string ip)
{
  localIP = ip;
  coreNumber = cores;
  for (auto i = 0; i < cores; i++)
  {
    shared_ptr<DoTask> task(new DoTask(to_string(i)));
    doTasks[to_string(i)] = task;
  }
}

void Manager::cmdCallback(string cmd, SOCKET sock)
{
  cout << "receive :" << cmd << endl;
  map<string, string> param;
  parseCommand(cmd, param);
  if (param["cmd"] == "start")
  {
    //cmd="start":taskid="TaskID":taskname="TaskName":processid="ProcessID":coreid="ProcessorID":bat="LocalScriptName":logdir="RemoteLogDir"
    if (doTasks.count(param["coreid"]) > 0)
    {
      doTasks[param["coreid"]]->startTask(param["taskid"], param["processid"], param["coreid"], param["bat"], bind(&Manager::callback, this, placeholders::_1));
    }
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
  cmd = "cmd=\"finish\":ip=\"" + localIP + "\":" + cmd;
  //cmd="finish":ip="IPAddr":taskid="taskID":processid="processID":coreid="processorID"
  cout << "send    :" + cmd << endl;
  client.Connect(remoteIP.c_str(), remotePort);
  client.SendMsg(cmd);
  client.Close();
}

void Manager::parseCommand(string cmd, map<string, string>& param) 
{
  while (cmd != "")
  {
    auto firstLoc = cmd.find_first_not_of("=:\"");
    auto lastLoc = cmd.find_first_of("=");
    if (firstLoc != string::npos && lastLoc != string::npos)
    {
      auto len = lastLoc - firstLoc;
      string Key = cmd.substr(firstLoc, len);
      cmd = cmd.substr(lastLoc + 2);
      lastLoc = cmd.find_first_of("\"");
      string Value = "";
      if (lastLoc != string::npos)
        Value = cmd.substr(0, lastLoc);
      param[Key] = Value;
      cmd = cmd.substr(lastLoc + 1);
    }
    else
    {
      break;
    }
  }
}
