//
// Created by zcl on 23-2-19.
//

#include "motor.h"

motor::motor() {
    control_mode_ = CONTROL_MODE_VOLTAGE_CONTROL;  // 控制模式: 0:电压控制，1: 力矩控制，2:速度控制，3:位置控制
    input_mode_ = INPUT_MODE_PASSTHROUGH;    // 电机输入模式:  0:失能，1:直接，5:位置梯形
    motor_mode_ = AXIS_STATE_CLOSED_LOOP_CONTROL;    // 工作模式: 0.电子刹车 1.失能 2.FOC闭环 3.保留电机模式
}

uint8_t motor::ESTOP() {
    return MSG_ODRIVE_ESTOP;
};

// 电机模式, (0: idle, 2:FOC闭环)
uint8_t motor::setMotorState(uint8_t state, unsigned char *buf){
    uint16_t state_index = 0;
    switch (state) {
        case AXIS_STATE_IDLE:
            state_index = AXIS_STATE_IDLE;
            memcpy(buf, &state_index, sizeof(uint16_t));
            break;
        case AXIS_STATE_CLOSED_LOOP_CONTROL:
            state_index = AXIS_STATE_CLOSED_LOOP_CONTROL;
            memcpy(buf, &state_index, sizeof(uint16_t));
            break;
        default:
            std::cout << "motor mode input error!";
    }

    return MSG_SET_AXIS_REQUESTED_STATE;
};

// 电机控制模式(0:电压控制，1: 力矩控制，2:速度控制，3:位置控制)
// 电机输入模式(0:失能，1:直接，5:位置梯形)
uint8_t motor::setControllerMode(uint8_t control_mode, uint8_t input_mode, unsigned char *buf){
    uint32_t control_mode_index = 0;
    uint32_t input_mode_index = 0;
    if(control_mode == CONTROL_MODE_VOLTAGE_CONTROL){
        control_mode_index = 0;
        memcpy(buf, &control_mode_index, sizeof(control_mode_index));}
    else if(control_mode == CONTROL_MODE_TORQUE_CONTROL){
        control_mode_index = 1;
        memcpy(buf, &control_mode_index, sizeof(control_mode_index));}
    else if(control_mode == CONTROL_MODE_VELOCITY_CONTROL){
        control_mode_index = 2;
        memcpy(buf, &control_mode_index, sizeof(control_mode_index));}
    else if(control_mode == CONTROL_MODE_POSITION_CONTROL){
        control_mode_index = 3;
        memcpy(buf, &control_mode_index, sizeof(control_mode_index));
    }

    if(input_mode == INPUT_MODE_INACTIVE){
        input_mode_index = INPUT_MODE_INACTIVE;
        memcpy(buf+4, &input_mode_index, sizeof(input_mode_index));}
    else if(input_mode == INPUT_MODE_PASSTHROUGH){
        input_mode_index = INPUT_MODE_PASSTHROUGH;
        memcpy(buf+4, &input_mode_index, sizeof(input_mode_index));}
    return MSG_SET_CONTROLLER_MODES;
};

// 力矩控制模式(0~4字节)
uint8_t motor::setTorque(float torque_cmd, unsigned char *buf){
    memcpy(buf, &torque_cmd, sizeof(uint32_t));
    return MSG_SET_INPUT_TORQUE;
};

// TODO: 检查报文的大小端存储
// 位置控制模式(0~4字节:位置命令，5~6:速度前馈，7~8:力矩前馈)
uint8_t motor::setPosition(float position_cmd, short velocity_ff, short torque_ff, unsigned char *buf){
    memcpy(buf, &position_cmd, sizeof(uint32_t));
    memcpy(buf+4, &velocity_ff, sizeof(short));   //注意指针加上的是字节数
    memcpy(buf+6, &torque_ff, sizeof(short));  //注意指针加上的是字节数，不是位数
    return MSG_SET_INPUT_POS;
};
// TODO: float->short 的正确性

// 速度控制模式(0~4字节:速度命令，5~8:力矩前馈)
uint8_t motor::setVelocity(float velocity_cmd, float torque_ff, unsigned char *buf){
    memcpy(buf, &velocity_cmd, sizeof(uint32_t));
    memcpy(buf+4, &torque_ff, sizeof(uint32_t));
    return MSG_SET_INPUT_VEL;
};

// 位置增益，odrive未实现
uint8_t motor::setPosGain(float pos_gain){};

// 速度增益，odrive未实现
uint8_t motor::setVelGain(float vel_gain, float vel_inter_gain){};

// 获取电机电流(0~4字节:期望电流，5~8:实际电流)
uint8_t motor::getIQ(){
    return MSG_GET_IQ;
};

// 获取电机的位置和速度(0~4字节:位置反馈，5~8:速度反馈)
uint8_t motor::getEncoder(){
    return MSG_GET_ENCODER_ESTIMATES;
};



void motor::motorCmdSend(ControlData_t *control_cmd){
//    std::cout << control_cmd <<std::endl;
    uint8_t axis_id = control_cmd->mode.id;
    uint8_t control_mode = control_cmd->mode.control_mode;
    uint8_t input_mode = control_cmd->mode.input_mode;
    uint8_t state = control_cmd->mode.status;
    unsigned char msg[15];
    unsigned char buf[CAN_LENGTH];
    // 设置工作模式: 0.电子刹车 1.失能 2.FOC闭环 3.保留电机模式
    if(state != motor_mode_){
        if(state == AXIS_STATE_ESTOP){
            memset(buf, 0, CAN_LENGTH);
            auto cmd_index = ESTOP();
            msgAssemble(STANDARD_FRAME, REMOTE_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);
        }
        else if((state == AXIS_STATE_IDLE) |(state == AXIS_STATE_CLOSED_LOOP_CONTROL)){
            memset(buf, 0, CAN_LENGTH);
            auto cmd_index = setMotorState(state, buf);
            msgAssemble(STANDARD_FRAME, DATA_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);
        }
        motor_mode_ = state;
        can_.send(msg, 15);
    }

    // 设置控制模式: 0:电压控制，1: 力矩控制，2:速度控制，3:位置控制

    if((control_mode != control_mode_) | (input_mode != input_mode_)){
        std::cout << "control_mode_" << std::endl;
        memset(buf, 0, CAN_LENGTH);
        auto cmd_index = setControllerMode(control_mode, input_mode, buf);
        msgAssemble(STANDARD_FRAME, DATA_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);

        control_mode_ = control_mode;
        input_mode_ = input_mode;
        can_.send(msg, 15);
    }

    // 控制指令
    memset(buf, 0, CAN_LENGTH);
    uint8_t cmd_index;
    switch (control_mode_) {
        case CONTROL_MODE_TORQUE_CONTROL:      // 力矩控制
            cmd_index = setTorque(control_cmd->cmd.tor_des, buf);
            break;
        case CONTROL_MODE_VELOCITY_CONTROL:      // 速度控制
            cmd_index = setVelocity(control_cmd->cmd.vel_des, control_cmd->cmd.tor_des, buf);
            break;
        case CONTROL_MODE_POSITION_CONTROL:
            cmd_index = setPosition(control_cmd->cmd.pos_des, control_cmd->cmd.vel_des, control_cmd->cmd.tor_des, buf);
            break;
        default:
            std::cout << "control_mode error!" << std::endl;
    }
    msgAssemble(STANDARD_FRAME, DATA_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);
    can_.send(msg, 15);



};



void motor::motorStateRecv(MotorData_t *motor_state, ControlData_t *control_cmd){

//    unsigned char buf[15] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x14, 0x08, 0x22, 0x7b, 0x69, 0x3e, 0x30, 0x22, 0x2a, 0x3c};
    // 状态请求指令
    unsigned char msg[15];
    unsigned char buf[CAN_LENGTH];
    uint8_t cmd_index;
    uint8_t axis_id = control_cmd->mode.id;

    memset(buf, 0, CAN_LENGTH);
    if(iq_callback_switch_){
        cmd_index = getIQ();
        msgAssemble(STANDARD_FRAME, REMOTE_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);
        can_.send(msg, 15);
    }
    if(pos_callback_switch_){
        cmd_index = getEncoder();
        msgAssemble(STANDARD_FRAME, REMOTE_FRAME, axis_id, cmd_index, CAN_LENGTH, buf, msg);
        can_.send(msg, 15);
    }

    memset(buf, 0, CAN_LENGTH);
    auto get_node_id = [](uint32_t can_id) {
        return (can_id >> 5); };
    auto get_cmd_id = [](uint32_t can_id) {
        return (can_id & 0x01F); };
    float *tor = new float(0); float *pos = new float(0); float *vel = new float(0);
    can_.recv(buf);
    unsigned int *can_id = (unsigned int*)(buf+2);
    unsigned int tmp;
    tmp = ((*can_id >> 24) & 0xff) | ((*can_id >> 8) & 0xff00) | ((*can_id << 8) & 0xff0000) | ((*can_id << 24) & 0xff000000);
    motor_state->mode.id = get_node_id(tmp);
    uint8_t cmd_id = get_cmd_id(tmp);

    switch (cmd_id) {
        case MSG_GET_IQ:
            tor = (float*)(buf+11);
            motor_state->fbk.torque = *tor;
            break;
        case MSG_GET_ENCODER_ESTIMATES:
            pos = (float*)(buf+7);
            vel = (float*)(buf+11);
            motor_state->fbk.pos = *pos;
//            std::cout << motor_state->fbk.pos << std::endl;
            motor_state->fbk.speed = *vel;
//            std::cout << motor_state->fbk.speed << std::endl;
            break;
        default:
            break;
        delete tor, pos, vel;
    };

};
void motor::motorSendRecv(ControlData_t *control_cmd, MotorData_t *motor_state){};

void motor::msgAssemble(uint8_t can_type, uint8_t can_form, uint8_t axis_id, uint8_t cmd_index, uint8_t can_length, unsigned char *buf, unsigned char *msg){
    memset(msg, 0, 15);
    uint32_t can_id = axis_id << 5 | cmd_index;
    unsigned char tmp[4];
    sendTrans(tmp,4,(unsigned char *)&can_id);
    memcpy(msg, &can_type, sizeof(uint8_t));
    memcpy(msg+1, &can_form, sizeof(uint8_t));
    memcpy(msg+2, tmp, sizeof(int32_t));
    memcpy(msg+6, &can_length, sizeof(uint8_t));
    memcpy(msg+7, buf, can_length);

};

void motor::sendTrans(unsigned char *buf, int length, unsigned char *var){
    for(int i = 0; i < length; i++){
        buf[i] = *(var+length-1-i);
    }
}