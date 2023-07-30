//
// Created by Hughzcl on 2021/5/22.
//

#include "SocketTool.h"

void SocketTool::setSockAddr(struct sockaddr_in &sockaddr, const std::string &ipaddr, int port)
{
    memset(&sockaddr, 0, sizeof(sockaddr)); //数据初始化--清零
    sockaddr.sin_family = AF_INET; //设置为IP通信

    if (ipaddr.empty())
    {
        sockaddr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
    }
    else
    {
        sockaddr.sin_addr.s_addr = inet_addr((char *) ipaddr.c_str());//服务器IP地址
    }

    sockaddr.sin_port = htons(port); //服务器端口号
}

bool SocketTool::setSockFd(int &fd, const std::string &protocol)
{
    if (protocol == "tcp")
    {
        fd = socket(AF_INET, SOCK_STREAM, 0);
    }
    else if (protocol == "udp")
    {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
    }
    return (fd >= 0);
}

//Unicast Tool
bool UdpServer::init_UniCast(int port, float period)
{
    _serverPort = port;
    _period = period;
    if (_serverFD != 0)
    {
        printf("The server has been initialized.\n");
        return true;
    }

    setSockAddr(_serverAddr, "", _serverPort);

    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_serverFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }

    // 设置超时
    if (_period != 0)
    {
        struct timeval tv;
        tv.tv_sec = (long int) _period;
        tv.tv_usec = (long int) (1e6 * std::fmod(_period, 1.f));
        if (setsockopt(_serverFD, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval)) < 0)
        {
            perror("Timeout setting error!\n");
            return false;
        }
    }

    int on = 1;
    if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
    {
        perror("Reuse addr error!\n");
        return false;
    }

    /*将套接字绑定到服务器的网络地址上*/
    if (bind(_serverFD, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        perror("Bind error\n");
        return false;
    }
}

long UdpServer::Send_UniCast(unsigned char *pSndBuf, int bufLen)
{
    long len;
    struct sockaddr_in targetAddr;
    //lock_clientAddr.pop_wait(&targetAddr);

    if (_serverFD == 0)
    {
        perror("Server fd has been released before, send failed!\n");
        return -1;
    }

    if ((len = sendto(_serverFD, pSndBuf, bufLen, 0, (struct sockaddr *) &targetAddr, sizeof(targetAddr))) < 0)
    {
//        perror("Sendto error!\n");
        return -1;
    }

    return len;
}

long UdpServer::Send_UniCast(const std::string &targetIP, int targetPort, unsigned char *pSndBuf, int bufLen)
{
    long len;
    struct sockaddr_in targetAddr;

    setSockAddr(targetAddr, targetIP, targetPort);

    if (_serverFD == 0)
    {
        perror("Server fd has been released before, send failed!\n");
        return -1;
    }

    if ((len = sendto(_serverFD, pSndBuf, bufLen, 0, (struct sockaddr *) &targetAddr, sizeof(targetAddr))) < 0)
    {
//        perror("Sendto error!\n");
        return -1;
    }

    return len;
}

long UdpServer::Receive_UniCast(unsigned char *pRcvBuf)
{
    long len;

    if (_serverFD == 0)
    {
        perror("Server fd has been released before, receive failed!\n");
        return -1;
    }

    if ((len = recvfrom(_serverFD, pRcvBuf, BUFFSIZE, 0, (struct sockaddr *) &_clientAddr, (socklen_t *) &sin_size)) <
        0)
    {
//        perror("Recvfrom error!\n");
        return -1;
    }

    //lock_clientAddr.push(_clientAddr);

    return len;
}

//BoardCast Tool
bool UdpServer::init_BoardCast()
{
    if (_serverFD != 0)
    {
        printf("The server has been initialized.\n");
        return true;
    }

    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_serverFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }

    const int opt = 1;
    //设置该套接字为广播类型，
    int nb = 0;
    nb = setsockopt(_serverFD, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
    if (nb == -1)
    {
        printf("set socket error...\n");
        return false;
    }
}

long UdpServer::boardCast(unsigned char *pSndBuf, int bufLen, const std::string& boardCastAddress, int targetPort)
{
    long len;
    struct sockaddr_in targetAddr;
    bzero(&targetAddr, sizeof(struct sockaddr_in));

    /* targetAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.1.255", &targetAddr.sin_addr.s_addr);
    //targetAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    targetAddr.sin_port = htons(targetPort); */

    setSockAddr(targetAddr, boardCastAddress, targetPort);

    if ((len = sendto(_serverFD, pSndBuf, bufLen, 0, (struct sockaddr *) &targetAddr, sizeof(targetAddr))) < 0)
    {
//        perror("Sendto error!\n");
        return -1;
    }

    return len;
}

//MiltiCast Tool
bool UdpServer::init_MultiCast()
{
    if (_serverFD != 0)
    {
        printf("The server has been initialized.\n");
        return true;
    }

    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_serverFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }
}

long UdpServer::MultiCast(unsigned char *pSndBuf, int bufLen, const std::string& MultiCastAddress, int MultiCastPort)
{
    long len;
    struct sockaddr_in targetAddr;
    bzero(&targetAddr, sizeof(struct sockaddr_in));

    setSockAddr(targetAddr, MultiCastAddress, MultiCastPort);

    if ((len = sendto(_serverFD, pSndBuf, bufLen, 0, (struct sockaddr *) &targetAddr, sizeof(targetAddr))) < 0)
    {
//        perror("Sendto error!\n");
        return -1;
    }
    return len;
}

void UdpServer::Release()
{
    if (_serverFD != 0)
    {
        close(_serverFD);
        _serverFD = 0;
    }
}


//*******************************************UDP CLient************************************************************//
//Unicast Tool
bool UdpClient::init_UniCast(const std::string &serverIP, int serverPort, float period)
{
    _serverIP = serverIP;
    _serverPort = serverPort;
    _period = period;
    if (_clientFD != 0)
    {
        printf("The client has been initialized.\n");
        return true;
    }

    setSockAddr(_serverAddr, _serverIP, _serverPort);
    int reuse = 1;
    if (setsockopt(_clientFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        // perror("setsockopet error\n");
        // return -1;
    }
    /*创建客户端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_clientFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }

    // 设置超时
    if (_period != 0)
    {
        struct timeval tv;
        tv.tv_sec = (long int) _period;
        tv.tv_usec = (long int) (1e6 * std::fmod(_period, 1.f));
        if (setsockopt(_clientFD, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval)) < 0)
        {
            perror("Timeout setting error!\n");
            return false;
        }
    }

    return true;
}

long UdpClient::Receive_UniCast(unsigned char *pRcvBuf)
{
    long len;

    if (_clientFD == 0)
    {
        perror("Client fd has been released before, receive failed!\n");
        return -1;
    }

    if ((len = recvfrom(_clientFD, pRcvBuf, BUFFSIZE, 0, (struct sockaddr *) &_serverAddr, (socklen_t *) &sin_size)) <
        0)
    {
       perror("Recvfrom error!\n");
        return -1;
    }

    return len;
}

long UdpClient::Send_UniCast(unsigned char *pSndBuf, int bufLen)
{
    long len;

    if (_clientFD == 0)
    {
        perror("Client fd has been released before, send failed!\n");
        return -1;
    }

    if ((len = sendto(_clientFD, pSndBuf, bufLen, 0, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr))) < 0)
    {
//        perror("Sendto error!\n");
        return -1;
    }

    return len;
}

//BoardCast Tool
bool UdpClient::init_BoardCast(int boardCastPort)
{
    _clientPort = boardCastPort;
    if (_clientFD != 0)
    {
        printf("The client has been initialized.\n");
        return true;
    }

    if (!setSockFd(_clientFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }
    int reuse = 0;
    if (setsockopt(_clientFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return -1;
    }
    setSockAddr(_clientAddr, "", _clientPort);
    int ret = bind(_clientFD, (struct sockaddr*)&_clientAddr, sizeof(_clientAddr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }
    return true;
}

long UdpClient::Recieve_BoardCast(unsigned char *RcvBuf)
{
    long len;
    if (_clientFD == 0)
    {
        perror("Client fd has been released before, receive failed!\n");
        return -1;
    }

    if ((len = recvfrom(_clientFD, RcvBuf, BUFFSIZE, 0, NULL, NULL)) <
        0)
    {
        //perror("Recvfrom error!\n");
        return -1;
    }
    return len;
}

//MiltiCast Tool
bool UdpClient::init_MultiCast(const string& multiCastAddress, int multiCastPort)
{
    _clientPort = multiCastPort;
    if (_clientFD != 0)
    {
        printf("The client has been initialized.\n");
        return true;
    }

    if (!setSockFd(_clientFD, "udp"))
    {
        perror("Socket error!\n");
        return false;
    }

    setSockAddr(_clientAddr, "", _clientPort);

    if (bind(_clientFD, (struct sockaddr *) &_clientAddr, sizeof(_clientAddr)) < 0)
    {
        perror("Bind error\n");
        return false;
    }

    int loop = 1;
    if(setsockopt(_clientFD,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop)) < 0)
    {
        perror("setsockopt():IP_MULTICAST_LOOP");
        return false;
    }

    struct ip_mreq mreq;                                                       /*加入多播组*/
    mreq.imr_multiaddr.s_addr = inet_addr((char *) multiCastAddress.c_str());  /*多播地址*/
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);                             /*网络接口为默认*//*将本机加入多播组*/

    if (setsockopt(_clientFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt():IP_ADD_MEMBERSHIP");
        return false;
    }

    return true;
}

long UdpClient::Recieve_MultiCast(unsigned char *RcvBuf)
{
    long len;
    if (_clientFD == 0)
    {
        perror("Client fd has been released before, receive failed!\n");
        return -1;
    }

    if ((len = recvfrom(_clientFD, RcvBuf, BUFFSIZE, 0, (struct sockaddr*)&_clientAddr,(socklen_t *) &sin_size)) < 0)
    {
        perror("Recvfrom error!\n");
        return -1;
    }
    return len;
}

void UdpClient::Release()
{
    if (_clientFD != 0)
    {
        close(_clientFD);
        _clientFD = 0;
    }
}

//*******************************************TCP************************************************************//
bool TcpServer::init(int port)
{
    _serverPort = port;

    if (_serverFD != 0)
    {
        printf("The server has been initialized.\n");
        return true;
    }

    setSockAddr(_serverAddr, "", _serverPort);

    /*创建服务器端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_serverFD, "tcp"))
    {
        perror("Socket error!\n");
        return false;
    }

    int on = 1;
    if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
    {
        perror("Reuse addr error!\n");
        return false;
    }

    /*将套接字绑定到服务器的网络地址上*/
    if (bind(_serverFD, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        perror("Bind error\n");
        return false;
    }

    /*监听连接请求--监听队列长度为2*/
    if (listen(_serverFD, 2) < 0)
    {
        perror("Listen error\n");
        return false;
    }

    return true;
}

bool TcpServer::waitConnect()
{
    if ((_clientFD = accept(_serverFD, (struct sockaddr *) &_clientAddr, (socklen_t *) &sin_size)) < 0)
    {
        perror("Accept error\n");
        return false;
    }
    printf("Accept client %s\n", inet_ntoa(_clientAddr.sin_addr));
    send(_clientFD, "Welcome to the Dog Manager server!\n", 35, 0);

    _flag_connect = true;
    return true;
}

int TcpServer::Receive(char *pRcvBuf)
{
    long len;

    if (_clientFD == 0)
    {
        perror("Not connected yet or client fd has been released before, receive failed!\n");
        return -2;
    }

    len = recv(_clientFD, pRcvBuf, BUFFSIZE, 0);

    if (len < 0)
    {
        perror("Receive error!\n");
        return -1;
    }

    if (len == 0)
    {
        perror("Connect interrupt! The client may be shutdown!\n");
        return 0;
    }

    return len;
}

int TcpServer::Send(char *pSndBuf, int bufLen)
{
    long len;

    if (_clientFD == 0)
    {
        perror("Not connected yet or client fd has been released before, send failed!\n");
        return -1;
    }

    if ((len = send(_clientFD, pSndBuf, bufLen, 0)) < 0)
    {
        perror("Send error!\n");
        return -1;
    }

    return len;
}

void TcpServer::Release()
{
    if (_clientFD != 0)
    {
        close(_clientFD);
        _clientFD = 0;
    }

    if (_serverFD != 0)
    {
        close(_serverFD);
        _serverFD = 0;
    }
}

bool TcpClient::init(const std::string &serverIP, int serverPort)
{
    _serverIP = serverIP;
    _serverPort = serverPort;

    if (_clientFD != 0)
    {
        printf("The client has been initialized.\n");
        return true;
    }

    setSockAddr(_serverAddr, _serverIP, _serverPort);

    /*创建客户端套接字--IPv4协议，面向无连接通信，UDP协议*/
    if (!setSockFd(_clientFD, "tcp"))
    {
        perror("Socket error!\n");
        return false;
    }

    return true;
}

bool TcpClient::Connect()
{
    if (connect(_clientFD, (struct sockaddr *) &_serverAddr, sizeof(_serverAddr)) < 0)
    {
        perror("Connect error\n");
        return false;
    }

    printf("Connected to server\n");
    char pRcvBuf[BUFFSIZE];
    long len = recv(_clientFD, pRcvBuf, BUFFSIZE, 0);//接收服务器端信息
    pRcvBuf[len] = '\0';
    printf("%s", pRcvBuf); //打印服务器端信息

    _flag_connect = true;
    return true;
}

int TcpClient::Receive(char *pRcvBuf)
{
    int len;

    if (_clientFD == 0)
    {
        perror("Client fd has been released before, receive failed!\n");
        return -2;
    }

    len = recv(_clientFD, pRcvBuf, BUFFSIZE, 0);

    if (len < 0)
    {
        perror("Receive error!\n");
        return -1;
    }

    if (len == 0)
    {
        perror("Connect interrupt! The server may be shutdown!\n");
        return 0;
    }

    return len;
}

int TcpClient::Send(char *pSndBuf, int bufLen)
{
    long len;

    if (_clientFD == 0)
    {
        perror("Client fd has been released before, send failed!\n");
        return -1;
    }

    if ((len = send(_clientFD, pSndBuf, bufLen, 0)) < 0)
    {
        perror("Send error!\n");
        return -1;
    }

    return len;
}

void TcpClient::Release()
{
    if (_clientFD != 0)
    {
        close(_clientFD);
        _clientFD = 0;
    }
}
