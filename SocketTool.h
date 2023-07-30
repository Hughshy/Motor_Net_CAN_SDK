//
// Created by Hughzcl on 2021/5/22.
//

#ifndef TRIGGERFORPI_SOCKETTOOL_H
#define TRIGGERFORPI_SOCKETTOOL_H
#define BUFFSIZE 1024 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cmath>
#include <iostream>


using namespace std;

class SocketTool
{
public:
    SocketTool() = default;

    ~SocketTool() = default;

    static void setSockAddr(struct sockaddr_in& sockaddr, const std::string& ipaddr, int port);

    static bool setSockFd(int& fd, const std::string& protocol);

protected:
    int sin_size = sizeof(struct sockaddr_in);

};

class UdpServer : public SocketTool
{
public:
    UdpServer() : SocketTool() {}

    ~UdpServer() = default;

    //UniCast Tool
    bool init_UniCast(int port, float period);  //period == 0: 阻塞 无超时

    long Send_UniCast(unsigned char* pSndBuf, int bufLen);

    long Send_UniCast(const std::string& targetIP, int targetPort, unsigned char* pSndBuf, int bufLen);

    long Receive_UniCast(unsigned char* pRcvBuf);

    //boardCast Tool
    bool init_BoardCast();

    long boardCast(unsigned char *pSndBuf, int bufLen, const std::string& boardCastAddress, int targetPort);//"192.168.xx.255", xxxx,

    //MultiCast Tool
    bool init_MultiCast();

    long MultiCast(unsigned char *pSndBuf, int bufLen, const std::string& MultiCastAddress, int MultiCastPort);//"224.0.0.xxx", xxxx

    void Release();

private:
    int _serverFD = 0;
    int _serverPort;
    struct sockaddr_in _serverAddr;
    struct sockaddr_in _clientAddr;
    float _period;
    //LockShareData<struct sockaddr_in> lock_clientAddr;
};

class UdpClient : public SocketTool
{
public:
    UdpClient() : SocketTool() {}

    ~UdpClient() = default;

    //UniCast Tool
    bool init_UniCast(const std::string& serverIP, int serverPort, float period);//period == 0: 阻塞 无超时

    long Receive_UniCast(unsigned char* pRcvBuf);

    long Send_UniCast(unsigned char* pSndBuf, int bufLen);

    //BoardCast Tool
    bool init_BoardCast(int boardCastPort);

    long Recieve_BoardCast(unsigned char *pRcvBuf);

    //MultiCast Tool
    bool init_MultiCast(const string& multiCastAddress, int multiCastPort);

    long Recieve_MultiCast(unsigned char *RcvBuf);

    void Release();

private:
    int _clientFD = 0;
    std::string _serverIP;
    int _serverPort;
    int _clientPort;
    struct sockaddr_in _serverAddr;
    struct sockaddr_in _clientAddr;
    float _period;

};

class TcpServer : public SocketTool
{
public:
    TcpServer() : SocketTool() {}

    ~TcpServer() = default;

    bool init(int port);

    bool waitConnect();

    int Receive(char* pRcvBuf);

    int Send(char* pSndBuf, int bufLen);

    void Release();

private:
    int _serverPort;
    int _serverFD = 0;
    int _clientFD = 0;
    struct sockaddr_in _serverAddr;
    struct sockaddr_in _clientAddr;
    bool _flag_connect = false;
};

class TcpClient : public SocketTool
{
public:
    TcpClient() : SocketTool() {}

    ~TcpClient() = default;

    bool init(const std::string& serverIP, int serverPort);

    bool Connect();

    int Receive(char* pRcvBuf);

    int Send(char* pSndBuf, int bufLen);

    void Release();

private:
    int _clientFD = 0;
    std::string _serverIP;
    int _serverPort;
    struct sockaddr_in _serverAddr;
    bool _flag_connect = false;
};



#endif //TRIGGERFORPI_SOCKETTOOL_H
