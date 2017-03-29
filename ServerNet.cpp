#include "ServerNet.h"
#include <iostream>

int ServerNet::init(const char* address, int port)
{
  localIP = address;
  localPort = port;
  if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
  {
    printf("WSAStartup()   failed   with   error   %d\n", Ret);
    WSACleanup();
    return -1;
  }
  // �������ڼ������׽��� 
  if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
  {
    printf("WSASocket()   failed   with   error   %d\n", WSAGetLastError());
    return -1;
  }
  // ���ü�����ַ�Ͷ˿ں�
  InternetAddr.sin_family = AF_INET;
  InternetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  InternetAddr.sin_port = htons(port);
  // �󶨼����׽��ֵ����ص�ַ�Ͷ˿�
  if (::bind(ListenSocket, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
  {
    printf("bind()   failed   with   error   %d\n", WSAGetLastError());
    return -1;
  }
}

void ServerNet::registerThread(string ip, short port, int cores)
{
  this_thread::sleep_for(chrono::seconds(2));
  remoteIP = ip;
  remotePort = port;
  client.Connect(remoteIP.c_str(), remotePort);
  string cmd("register:");
  cmd += localIP + ":" + to_string(localPort);
  cmd += ":" + to_string(cores);
  //cmd: register:IPAddr:port:CoreNumber
  client.SendMsg(cmd);

  client.Close();
}


void ServerNet::registerToServer(string ip, short port, int cores)
{
  registerThread_t = thread(&ServerNet::registerThread, this, ip, port, cores);
}


void ServerNet::run()
{
  if (listen(ListenSocket, 5))
  {
    printf("listen()   failed   with   error   %d\n", WSAGetLastError());
    return;
  }
  // ����Ϊ������ģʽ
  ULONG NonBlock = 1;
  if (ioctlsocket(ListenSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
  {
    printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
    return;
  }

  CreateSocketInformation(ListenSocket);// ΪListenSocket�׽��ִ�����Ӧ��SOCKET_INFORMATION,��ListenSocket��ӵ�SocketArray������

  while (true)
  {
    FD_ZERO(&ReadSet);// ׼����������I/O֪ͨ�Ķ�/д�׽��ּ���
    FD_ZERO(&WriteSet);

    FD_SET(ListenSocket, &ReadSet);// ��ReadSet��������Ӽ����׽���ListenSocket
                                   // ��SocketArray�����е������׽�����ӵ�WriteSet��ReadSet������,SocketArray�����б����ż����׽��ֺ�������ͻ��˽���ͨ�ŵ��׽���
                                   // �����Ϳ���ʹ��select()�ж��ĸ��׽����н������ݻ��߶�ȡ/д������
    for (DWORD i = 0; i < TotalSockets; i++)
    {
      LPSOCKET_INFORMATION SocketInfo = SocketArray[i];
      FD_SET(SocketInfo->Socket, &ReadSet);//��˵����socket�ж����������������ǿͻ��˷����
      FD_SET(SocketInfo->Socket, &WriteSet);//��˵����socket��д������

    }
    // �ж϶�/д�׽��ּ����о������׽���    
    if ((Total = select(0, &ReadSet, &WriteSet, NULL, NULL)) == SOCKET_ERROR)//��NULL���βδ���Timeout����������ʱ��ṹ�����ǽ�select��������״̬��һ���ȵ������ļ�������������ĳ���ļ������������仯Ϊֹ.��������ͣ������ȴ��ͻ�����Ӧ
    {
      printf("select()   returned   with   error   %d\n", WSAGetLastError());
      return;
    }
    // ���δ��������׽��֡�����������һ����Ӧ�������������ӿͻ����յ����ַ����ٷ��ص��ͻ��ˡ�
    for (DWORD i = 0; i < TotalSockets; i++)
    {
      LPSOCKET_INFORMATION SocketInfo = SocketArray[i];            // SocketInfoΪ��ǰҪ������׽�����Ϣ
                                                                   // �жϵ�ǰ�׽��ֵĿɶ��ԣ����Ƿ��н��������������߿��Խ�������
      if (FD_ISSET(SocketInfo->Socket, &ReadSet))
      {
        if (SocketInfo->Socket == ListenSocket)        // ���ڼ����׽�����˵���ɶ���ʾ���µ���������
        {
          Total--;    // �������׽��ּ�1
                      // �����������󣬵õ���ͻ��˽���ͨ�ŵ��׽���AcceptSocket
          if ((AcceptSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET)
          {
            // �����׽���AcceptSocketΪ������ģʽ
            // �����������ڵ���WSASend()������������ʱ�Ͳ��ᱻ����
            NonBlock = 1;
            if (ioctlsocket(AcceptSocket, FIONBIO, &NonBlock) == SOCKET_ERROR)
            {
              printf("ioctlsocket()   failed   with   error   %d\n", WSAGetLastError());
              return;
            }
            // �����׽�����Ϣ����ʼ��LPSOCKET_INFORMATION�ṹ�����ݣ���AcceptSocket��ӵ�SocketArray������
            if (CreateSocketInformation(AcceptSocket) == FALSE)
              return;
          }
          else
          {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
              printf("accept()   failed   with   error   %d\n", WSAGetLastError());
              return;
            }
          }
        }
        else   // ��������
        {
          Total--;                // ����һ�����ھ���״̬���׽���
          memset(SocketInfo->Buffer, ' ', DATA_BUFSIZE);            // ��ʼ��������
          SocketInfo->DataBuf.buf = SocketInfo->Buffer;            // ��ʼ��������λ��
          SocketInfo->DataBuf.len = DATA_BUFSIZE;                // ��ʼ������������
                                                                 // ��������
          DWORD  Flags = 0;
          if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR)
          {
            // ����������WSAEWOULDBLOCK��ʾ��û�����ݣ������ʾ�����쳣
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
              printf("WSARecv()   failed   with   error   %d\n", WSAGetLastError());
              FreeSocketInformation(i);        // �ͷ��׽�����Ϣ
            }
            continue;
          }
          else   // ��������
          {
            SocketInfo->BytesRECV = RecvBytes;        // ��¼�������ݵ��ֽ���
            SocketInfo->DataBuf.buf[RecvBytes] = '\0';
            if (RecvBytes == 0)                                    // ������յ�0���ֽڣ����ʾ�Է��ر�����
            {
              FreeSocketInformation(i);
              continue;
            }
            else
            {
              cout << SocketInfo->DataBuf.buf << endl;// ����ɹ��������ݣ����ӡ�յ�������
              if (callback)
                callback(SocketInfo->DataBuf.buf, SocketInfo->Socket);
            }
          }
        }
      }
      else
      {
        // �����ǰ�׽�����WriteSet�����У���������׽��ֵ��ڲ����ݻ������������ݿ��Է���
        if (FD_ISSET(SocketInfo->Socket, &WriteSet))
        {
          Total--;            // ����һ�����ھ���״̬���׽���
          SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;            // ��ʼ��������λ��
          SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;    // ��ʼ������������
          if (SocketInfo->DataBuf.len > 0)        // �������Ҫ���͵����ݣ���������
          {
            if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR)
            {
              // ����������WSAEWOULDBLOCK��ʾ��û�����ݣ������ʾ�����쳣
              if (WSAGetLastError() != WSAEWOULDBLOCK)
              {
                printf("WSASend()   failed   with   error   %d\n", WSAGetLastError());
                FreeSocketInformation(i);        // �ͷ��׽�����Ϣ
              }
              continue;
            }
            else
            {
              SocketInfo->BytesSEND += SendBytes;            // ��¼�������ݵ��ֽ���
                                                             // ����ӿͻ��˽��յ������ݶ��Ѿ����ص��ͻ��ˣ��򽫷��ͺͽ��յ��ֽ���������Ϊ0
              if (SocketInfo->BytesSEND == SocketInfo->BytesRECV)
              {
                SocketInfo->BytesSEND = 0;
                SocketInfo->BytesRECV = 0;
              }
            }
          }
        }

      }    // ���ListenSocketδ���������ҷ��صĴ�����WSAEWOULDBLOCK���ô����ʾû�н��յ��������󣩣�������쳣

    }
  }
}

void ServerNet::setCallback(decltype(callback) cb)
{
  callback = cb;
}

string getLocalIpAddress()
{
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;
  if (WSAStartup(wVersionRequested, &wsaData) != 0)
    return "";
  char local[255] = {0};
  gethostname(local, sizeof(local));
  hostent* ph = gethostbyname(local);
  if (ph == nullptr)
    return "";
  in_addr addr;
  memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // �������ȡ��һ��ip  
  string localIP;
  localIP.assign(inet_ntoa(addr));
  WSACleanup();
  return localIP;
}

short getUnusedPort(short start_port)
{
  int sock;
  int iErrorMsg;
  WSAData wsaData;
  iErrorMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
  if (iErrorMsg != NO_ERROR)
  {
    printf("wsastartup failed with error : %d\n", iErrorMsg);
    return -1;
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  short port;
  int flag = 0;
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  for (port = start_port; port < 65536; port++)
  {
    addr.sin_port = htons(port);
    if (::bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) >= 0)
    {
      flag = 0;
      if ((port % 2) == 0)
        port++;
    }
    else
    {
      if ((++flag) == 2)
      {
        closesocket(sock);
        return port - 1;
      }
      else
      {
        closesocket(sock);
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      }
    }
  }
  return 0;
}


BOOL   ServerNet::CreateSocketInformation(SOCKET   s)
{
  LPSOCKET_INFORMATION   SI;                                        // ���ڱ����׽��ֵ���Ϣ       
 //  printf("Accepted   socket   number   %d\n",   s);            // ���ѽ��ܵ��׽��ֱ��
// ΪSI�����ڴ�ռ�
  if ((SI = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL)
  {
    printf("GlobalAlloc()   failed   with   error   %d\n", GetLastError());
    return   FALSE;
  }
  // ��ʼ��SI��ֵ    
  SI->Socket = s;
  SI->BytesSEND = 0;
  SI->BytesRECV = 0;

  // ��SocketArray����������һ����Ԫ�أ����ڱ���SI���� 
  SocketArray[TotalSockets] = SI;
  TotalSockets++;                        // �����׽�������

  return(TRUE);
}

// ������SocketArray��ɾ��ָ����LPSOCKET_INFORMATION����
void   ServerNet::FreeSocketInformation(DWORD   Index)
{
  LPSOCKET_INFORMATION SI = SocketArray[Index];    // ��ȡָ��������Ӧ��LPSOCKET_INFORMATION����
  DWORD   i;

  closesocket(SI->Socket);       // �ر��׽���
  GlobalFree(SI);   // �ͷ�ָ��LPSOCKET_INFORMATION������Դ
// ��������index���������Ԫ��ǰ��
  if (Index != (TotalSockets - 1))
  {
    for (i = Index; i < TotalSockets; i++)
    {
      SocketArray[i] = SocketArray[i + 1];
    }
  }

  TotalSockets--;        // �׽���������1
}