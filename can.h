#ifndef CAN_H
#define CAN_H
#define BT_CAN_02_IP "192.168.0.8"
#define BT_CAN_02_Port  1024

#include "SocketTool.h"
class can
{
public:
    can();
    ~can() = default;

    uint32_t send(unsigned char *sendBuf, uint32_t len);
    uint32_t recv(unsigned char *recvBuf);
    uint32_t sendRecv(unsigned char *sendBuf, uint32_t len,unsigned char *recvBuf);

private:
    UdpClient Ethernet2Can;
};



#endif //CAN_H