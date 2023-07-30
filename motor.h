//
// Created by zcl on 23-2-19.
//

#ifndef MOTOR_NET_CAN_SDK_MOTOR_H
#define MOTOR_NET_CAN_SDK_MOTOR_H
#define CAN_LENGTH 8
#include "motor_msgs.h"
#include "can.h"
#include <string.h>
#include <string>
#include <iostream>
#include "LockData.h"

class motor {
public:
    motor();
    ~motor() = default;

// 实现can报文的数据段，以及cmd_id
    uint8_t ESTOP();                                                                 // 电机模式：电子刹车
    uint8_t setMotorState(uint8_t state, unsigned char *buf);                        // 电机模式, (0: idle, 2:FOC闭环)
    uint8_t setControllerMode(uint8_t control_mode, uint8_t input_mode, unsigned char *buf);  // 电机控制模式(0:电压控制，1: 力矩控制，2:速度控制，3:位置控制)
                                                                                     // 电机输入模式(0:失能，1:直接，5:位置提醒)
    uint8_t setTorque(float torque_cmd, unsigned char *buf);                         // 力矩控制模式(0~4字节)

    uint8_t setPosition(float position_cmd, short velocity_ff, short torque_ff, unsigned char *buf); // 位置控制模式(0~4字节:位置命令，5~6:速度前馈，7~8:力矩前馈)
    uint8_t setVelocity(float velocity_cmd, float torque_ff, unsigned char *buf);                    // 速度控制模式(0~4字节:速度命令，5~8:力矩前馈)
    uint8_t setPosGain(float pos_gain);          // 位置增益: 未实现
    uint8_t setVelGain(float vel_gain, float vel_inter_gain);  // 速度增益: 未实现
    uint8_t getIQ();                             // 获取电机电流(0~4字节:期望电流，5~8:实际电流)
    uint8_t getEncoder();                        // 获取电机的位置和速度(0~4字节:位置反馈，5~8:速度反馈)


// 判断电机的控制模式，调用合适的接口，并组织成完成的UDP报文(can类型_1Bit、can格式_1Bit、canID号_4Bit、帧数据_8Bit)
    void motorCmdSend(ControlData_t *control_cmd);
    void motorStateRecv(MotorData_t *motor_state, ControlData_t *control_cmd);
    void motorSendRecv(ControlData_t *control_cmd, MotorData_t *motor_state);
    void msgAssemble(uint8_t can_type, uint8_t can_form, uint8_t axis_id, uint8_t cmd_index, uint8_t can_length, unsigned char *buf, unsigned char *msg);
    void sendTrans(unsigned char *buf, int length, unsigned char *var);

private:
    can can_;

    // 如果没有变动，则不下发报文；
    uint8_t control_mode_;  // 控制模式: 0:电压控制，1: 力矩控制，2:速度控制，3:位置控制
    uint8_t input_mode_;    // 电机输入模式:  0:失能，1:直接，5:位置梯形
    uint8_t motor_mode_;    // 工作模式: 0.电子刹车 1.失能 2.FOC闭环 3.保留电机模式

    bool iq_callback_switch_ = true;
    bool pos_callback_switch_ = true;

//    MotorData_t motor_state_;
//    ControlData_t motor_cmd_;
    LockData<MotorData_t> motor_state_;
    LockData<ControlData_t> motor_cmd_;



};


#endif //MOTOR_NET_CAN_SDK_MOTOR_H
