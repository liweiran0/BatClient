#include "ServerNet.h"
#include "Manager.h"

using namespace std;

ServerNet server;
void main(int argv, char* argc[])
{
  string fileName = "client.cfg";
  string serverIP = "10.8.0.131";
  string netDir = "\\\\10.8.0.132\\xzheng\\";
  string localDir = "D:\\xzheng\\";

  int serverPort = 30000;
  int localCores = 15;
  if (argv < 3)
  {
    cout << "Usage: localIP localCores [cfg_file=client.cfg]" << endl;
    return;
  }

  string localIP = string(argc[1]);
  localCores = stoi(string(argc[2]));
  if (argv == 4)
  {
    fileName = argc[3];
  }

  FILE* fp = fopen(fileName.c_str(), "r");
  if (fp)
  {
    char buffer[1024];
    fgets(buffer, 1023, fp);
    string tmp(buffer);
    stringstream ss(tmp);
    ss >> serverIP >> serverPort >> netDir >> localDir;
  }
  short localPort = getUnusedPort(20000);
  Manager manager(localCores, localIP, netDir, localDir);
  manager.setRemote(serverIP, serverPort);
  server.init(localIP.c_str(), localPort);
  server.setCallback(bind(&Manager::cmdCallback, &manager, placeholders::_1, placeholders::_2));
  server.registerToServer(serverIP, serverPort, localCores, netDir);
  server.run();
}
