
#include "SocketTool.h"
#include "can.h"
#include <iostream>
#include "motor.h"
#include "motor_msgs.h"
#include <thread>
#include <cstdlib>
#include <memory>
#include "taskScheduling.h"

using namespace std;
void byte2float(unsigned char* buf, int length);
int main(void)
{

    cout << "run" << endl;
    motor motor_1;
    MotorData_t state_;
    ControlData_t cmd_;


    cmd_.mode.input_mode = INPUT_MODE_PASSTHROUGH;
    cmd_.mode.control_mode = CONTROL_MODE_VELOCITY_CONTROL;
    cmd_.mode.status = AXIS_STATE_IDLE


            ;
    cmd_.mode.id = 0;
    cmd_.cmd.pos_des = 0;
    cmd_.cmd.vel_des = 1;
    cmd_.cmd.tor_des = 0.0;

    motor_1.motorCmdSend(&cmd_);
    while(1){
        motor_1.motorStateRecv(&state_,&cmd_);
//        cout << "----------------" << endl;
        cout << state_.fbk.pos << "\t";
        cout << state_.fbk.speed << "\t";
        cout << state_.fbk.torque << endl;
        usleep(1e3);
    }






//   sleep(10);
}

void byte2float(unsigned char* buf, int length){
    float *p = (float *)buf;
    cout << *p << endl;
}


