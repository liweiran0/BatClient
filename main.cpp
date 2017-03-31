#include "ServerNet.h"
#include "Manager.h"

using namespace std;

ServerNet server;
void main(int argv, char* argc[])
{
  string fileName = "client.cfg";
  string serverIP = "10.8.0.130";
  string localIP = getLocalIpAddress();
  
  int serverPort = 30000;
  int localCores = 1;

  if (argv >= 2)
  {
    localIP = string(argc[1]);
  }
  if (argv == 3)
  {
    localCores = stoi(string(argc[2]));
  }
  FILE* fp = fopen(fileName.c_str(), "r");
  if (fp)
  {
    char buffer[1024];
    fgets(buffer, 1023, fp);
    string tmp(buffer);
    stringstream ss(tmp);
    ss >> serverIP >> serverPort;
  }
  short localPort = getUnusedPort(20000);
  Manager manager(localCores, localIP);
  manager.setRemote(serverIP, serverPort);
  server.init(localIP.c_str(), localPort);
  server.setCallback(bind(&Manager::cmdCallback, &manager, placeholders::_1, placeholders::_2));
  server.registerToServer(serverIP, serverPort, localCores);
  server.run();
}
