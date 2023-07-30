//  
/// COPYRIGHT NOTICE  
/// Copyright (c) 2021, 华中科技大学tickTick Group  （版权声明）
/// All rights reserved.
///
/// @file    taskScheduling.h
/// @brief   周期运行任务的头文件
///
/// 本文件完成任务的周期运行。主要包含两个class：实现周期任务的类，对周期任务进行管理的任务管理器。
/// 在使用本周期管理类时，首先要对周期运行函数创建周期任务类，然后将周期任务类添加到任务管理器进行统一调度。
///
/// @version 1.0
/// @author
/// @E-mail：
/// @date
///
///  修订说明：
//


#ifndef TASKSCHEDULING_H
#define TASKSCHEDULING_H

#include <thread>
#include <string>
#include <time.h>
#include <vector>
#include <map>
#include <sys/time.h>

using namespace std;

class periodTaskManager;

/** 周期任务类
*
*	本类实现了创建周期任务的相关操作
*	例如：创建、配置、启动和停止周期任务
*/
class periodTask
{
public:
    /** 有参构造函数，对周期任务进行配置
    *  @param: periodTaskManager* taskManager 任务管理器指针，将该任务添加到任务管理器中
    *  @param: float period 设置周期任务的执行周期
    *  @param: string name  设置周期任务的名称
    *  @return:
    *  @note:
    *  @see:
    */
    periodTask(periodTaskManager* taskManager, float period, string name);

    /** 启动周期任务
    *
    *  @note:
    *  @see:
    */
    void start();

    /** 停止周期任务
    *
    *  @note:
    *  @see:
    */
    void stop();

    /** 运行周期任务
    *
    *  @note:本函数是纯虚函数，具体实现分为两种，成员函数和普通函数
    *		 主要是实现的功能是将函数的指针传入run()中
    *  @see:
    */
    virtual void run() = 0;

    /** 获取任务名称
    *
    *  @return:周期函数的名称
    *  @note:
    *  @see:
    */
    string getName();

    /** 定时器函数，使用QueryPerformanceFrequency()函数
    *  @param: int us 设置定时时间，单位为us
    *  @return: 定时成功返回实际定时时间，定时失败返回-1
    *  @note:
    *  @see:
    */
    int tool_TimeDelay(int us);
    //void cleanup();


private:
    /** 周期任务循环函数
    *
    *  @note:使用run()函数和tool_TimeDelay()进行延时
    *  @see:
    */
    void loopFunction();

private:
    /** 设置任务的执行周期 */
    float _period;

    /** 设置周期任务的名称 */
    string _name;

    /** 周期任务的执行标志 */
    volatile bool _running = false;

    /** 实例化线程对象 */
    thread _thread;
};

/** 周期函数类
*
*	本类实现了对periodTask类中run()函数的重写，实现普通函数的运行
*/
class periodTask_Function : public periodTask
{
public:
    periodTask_Function(periodTaskManager* taskManager, float period, string name, void (*function)())
            :periodTask(taskManager, period, name), _function(function){};
    void run(){
        _function();
    };

private:

    void (*_function)() = nullptr;
};

/** 周期成员函数类
*
*	本类实现了对periodTask类中run()函数的重写，实现成员函数的运行
*/
template<typename T>
class periodTask_MemberFunction : public periodTask
{
public:
    periodTask_MemberFunction(periodTaskManager* taskManager, float period, string name, void (T::* function)(), T* obj)
            :periodTask(taskManager, period, name), _function(function), _obj(obj)
    {};

    /** 运行函数主体
    *
    *  @note:使用run()函数和tool_TimeDelay()进行延时
    *  @see:
    */
    void run(){
        (_obj->*_function)();
    };
private:
    /** 创建T的成员函数指针 */
    void (T::*_function)();
    /** 实例化类T的指针 */
    T* _obj;
};


/** 周期任务管理类
*
*	本类实现了管理周期任务的相关操作
*	例如：添加任务到任务管理器、启动和停止周期任务等相关操作
*/
class periodTaskManager
{
public:

    /** 将周期任务添加到任务管理器
    *  @param: periodTask *task 周期任务类的指针
    *
    *  @note:将周期任务指针添加到任务管理器_tasks中，将名字和序列号添加到_information中
    *  @see:
    */
    void addTask(periodTask *task);

    /** 将周期任务添加到任务管理器
    *  @param: string taskName 周期任务类的名称
    *  @return: 返回值为周期任务的指针
    *  @note:
    *  @see:
    */
    periodTask* getTask(string taskName);

    /** 从任务管理器启动周期任务
    *  @param: string taskName 周期任务类的名称
    *
    *  @note:
    *  @see:
    */
    void startTask(string taskName);

    /** 从任务管理器启动周期任务
    *  @param: string taskName 周期任务类的名称
    *
    *  @note:
    *  @see:
    */
    void stopTask(string taskName);

    /** 从任务管理器启动所有周期任务
    *  @note:
    *  @see:
    */
    void startAll();

    /** 从任务管理器停止所有周期任务
    *  @note:
    *  @see:
    */
    void stopAll();

    /** 从任务管理器获取所有周期任务的名字
    *  @note:
    *  @see:
    */
    void printTasks();

private:
    /** 任务管理器中存储任务指针的数组 */
    vector<periodTask*> _tasks;

    /** 任务管理器中存储任务序列号和名称的map容器 */
    map<string, int> _information;

    /** 任务管理器中存储任务的数量 */
    int _count = 0;
};
#endif //TASKSCHEDULING_H
