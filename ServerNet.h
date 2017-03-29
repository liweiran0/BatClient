#pragma once
#include "ClientNet.h"  
#pragma comment (lib,"ws2_32.lib")
using namespace std;
#define DATA_BUFSIZE 1024

typedef   struct   _SOCKET_INFORMATION
{
  CHAR   Buffer[DATA_BUFSIZE];        // ���ͺͽ������ݵĻ�����
  WSABUF   DataBuf;                        // ���巢�ͺͽ������ݻ������Ľṹ�壬�����������ĳ��Ⱥ�����
  SOCKET   Socket;                            // ��ͻ��˽���ͨ�ŵ��׽���
  DWORD   BytesSEND;                    // �����׽��ַ��͵��ֽ���
  DWORD   BytesRECV;                    // �����׽��ֽ��յ��ֽ���
  
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;



class ServerNet
{
private:
  void registerThread(string ip, short port, int cores);
  BOOL   CreateSocketInformation(SOCKET   s);
  void   FreeSocketInformation(DWORD   Index);
  SOCKET m_sock;
  function<void(string, SOCKET)> callback;
  ClientNet client;
  string remoteIP;
  short  remotePort;
  string localIP;
  short localPort;
  thread registerThread_t;
  DWORD   TotalSockets = 0;                // ��¼����ʹ�õ��׽���������
  LPSOCKET_INFORMATION   SocketArray[FD_SETSIZE];            // ����Socket��Ϣ��������飬FD_SETSIZE��ʾSELECTģ��������������׽�������
  SOCKET   ListenSocket;                    // �����׽���
  SOCKET   AcceptSocket;                    // ��ͻ��˽���ͨ�ŵ��׽���
  SOCKADDR_IN   InternetAddr;            // �������ĵ�ַ
  WSADATA   wsaData;                        // ���ڳ�ʼ���׽��ֻ���
  INT   Ret;                                            // WinSock API�ķ���ֵ
  FD_SET   WriteSet;                            // ��ȡ��д�Ե��׽��ּ���
  FD_SET   ReadSet;                            // ��ȡ�ɶ��Ե��׽��ּ���
  DWORD   Total = 0;                                // ���ھ���״̬���׽�������
  DWORD   SendBytes;                        // ���͵��ֽ���
  DWORD   RecvBytes;                        // ���յ��ֽ���
public:
  int init(const char* address, int port);
  void registerToServer(string ip, short port, int cores);
  void run();
  void setCallback(decltype(callback) cb);

};

string getLocalIpAddress();
short getUnusedPort(short start_port);