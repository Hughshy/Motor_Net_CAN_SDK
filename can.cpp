#include "can.h"

can::can(){
    Ethernet2Can.init_UniCast(BT_CAN_02_IP, BT_CAN_02_Port, 0);

}

uint32_t can::send(unsigned char *sendBuf, uint32_t Len){
    uint32_t len = Ethernet2Can.Send_UniCast(sendBuf, Len);
    return len;
}

uint32_t can::recv(unsigned char *recvBuf){
    uint32_t len = Ethernet2Can.Receive_UniCast(recvBuf);
    return len;
}

uint32_t can::sendRecv(unsigned char *sendBuf, uint32_t len, unsigned char *recvBuf){
    Ethernet2Can.Send_UniCast(sendBuf, len);
    Ethernet2Can.Receive_UniCast(recvBuf);

}