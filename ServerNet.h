#pragma once
#include "ClientNet.h"  
#pragma comment (lib,"ws2_32.lib")
using namespace std;
#define DATA_BUFSIZE 1024

typedef   struct   _SOCKET_INFORMATION
{
  CHAR   Buffer[DATA_BUFSIZE];        // 发送和接收数据的缓冲区
  WSABUF   DataBuf;                        // 定义发送和接收数据缓冲区的结构体，包括缓冲区的长度和内容
  SOCKET   Socket;                            // 与客户端进行通信的套接字
  DWORD   BytesSEND;                    // 保存套接字发送的字节数
  DWORD   BytesRECV;                    // 保存套接字接收的字节数
  string  cmd_str;                      // 拼接字符串
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;



class ServerNet
{
private:
  void registerThread(string ip, unsigned short port, int cores, string netDir);
  BOOL   CreateSocketInformation(SOCKET   s);
  void   FreeSocketInformation(DWORD   Index);
  SOCKET m_sock;
  function<void(string, SOCKET)> callback;
  ClientNet client;
  string remoteIP;
  unsigned short  remotePort;
  string localIP;
  unsigned short localPort;
  thread registerThread_t;
  DWORD   TotalSockets = 0;                // 记录正在使用的套接字总数量
  LPSOCKET_INFORMATION   SocketArray[FD_SETSIZE];            // 保存Socket信息对象的数组，FD_SETSIZE表示SELECT模型中允许的最大套接字数量
  SOCKET   ListenSocket;                    // 监听套接字
  SOCKET   AcceptSocket;                    // 与客户端进行通信的套接字
  SOCKADDR_IN   InternetAddr;            // 服务器的地址
  WSADATA   wsaData;                        // 用于初始化套接字环境
  INT   Ret;                                            // WinSock API的返回值
  FD_SET   WriteSet;                            // 获取可写性的套接字集合
  FD_SET   ReadSet;                            // 获取可读性的套接字集合
  DWORD   Total = 0;                                // 处于就绪状态的套接字数量
  DWORD   SendBytes;                        // 发送的字节数
  DWORD   RecvBytes;                        // 接收的字节数
public:
  int init(const char* address, int port);
  void registerToServer(string ip, unsigned short port, int cores, string netDir);
  void run();
  void setCallback(decltype(callback) cb);

};

string getLocalIpAddress();
unsigned short getUnusedPort(unsigned short start_port);
string getCommandFromString(string &str);