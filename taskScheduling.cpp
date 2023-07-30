
#include "taskScheduling.h"

periodTask::periodTask(periodTaskManager *taskManager, float period, string name)
        : _period(period), _name(name)
{

    /** 创建周期任务类时，调用有参构造会直接将周期任务添加到任务管理器 */
    taskManager->addTask(this);
};

void periodTask::start()
{
    if (_running)
    {
        printf("[PeriodicTask] Tried to start %s but it was already running!\n",
               _name.c_str());
        return;
    }
    _running = true;

    /** 创建线程，运行任务的循环函数*/
    _thread = thread(&periodTask::loopFunction, this);

};

void periodTask::stop()
{
    if (!_running)
    {
        printf("[PeriodicTask] Tried to stop %s but it isn't running!\n",
               _name.c_str());
        return;
    }
    _running = false;
    printf("[PeriodicTask] Waiting for %s to stop...\n", _name.c_str());

    /** 对创建的线程进行回收 */
    _thread.join();
    printf("[PeriodicTask] Done!\n");
    //cleanup();
};

void periodTask::loopFunction()
{
    while (_running)
    {
        run();
        tool_TimeDelay(_period);
    }
    printf("[PeriodicTask] %s has stopped!\n", _name.c_str());


};

string periodTask::getName()
{
    return _name;
};

int periodTask::tool_TimeDelay(int microseconds)
{
    struct timespec start_time, end_time, sleep_time;

    clock_gettime(CLOCK_REALTIME, &start_time);  // 获取当前时间

    // 计算结束时间
    end_time.tv_sec = start_time.tv_sec + microseconds / 1000000L;
    end_time.tv_nsec = start_time.tv_nsec + (microseconds % 1000000L) * 1000L;
    if (end_time.tv_nsec >= 1000000000L) {
        end_time.tv_sec++;
        end_time.tv_nsec -= 1000000000L;
    }

    // 计算需要睡眠的时间
    sleep_time.tv_sec = end_time.tv_sec - start_time.tv_sec;
    sleep_time.tv_nsec = end_time.tv_nsec - start_time.tv_nsec;
    if (sleep_time.tv_nsec < 0) {
        sleep_time.tv_sec--;
        sleep_time.tv_nsec += 1000000000L;
    }

    // 进行睡眠
    nanosleep(&sleep_time, NULL);

//    clock_gettime(CLOCK_REALTIME, &end_time);  // 获取实际结束时间
//
//    // 计算实际耗时
//    long elapsed_seconds = end_time.tv_sec - start_time.tv_sec;
//    long elapsed_nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
//    if (elapsed_nanoseconds < 0) {
//        elapsed_seconds--;
//        elapsed_nanoseconds += 1000000000L;
//    }

//    std::cout << "Elapsed time: " << elapsed_seconds * 1000000L + elapsed_nanoseconds / 1000L << " microseconds" << std::endl;

    return -1;//
};


void periodTaskManager::addTask(periodTask* task)
{
    _tasks.push_back(task);
    _information.insert(pair<string, int>(task->getName(), _count));
    ++_count;
};

periodTask* periodTaskManager::getTask(string taskName)
{
    auto iter = _information.find(taskName);
    if(iter != _information.end())
    {
        return _tasks[_information[taskName]];
    }
    else
    {
        printf("There is no task named %s !", taskName.c_str());
        return nullptr;
    }
};

void periodTaskManager::startTask(string taskName)
{
    auto iter = _information.find(taskName);
    if (iter != _information.end())
    {
        _tasks[_information[taskName]]->start();
    }
    else
    {
        printf("There is no task named %s !", taskName.c_str());
    }
};

void periodTaskManager::stopTask(std::string taskName)
{
    auto iter = _information.find(taskName);
    if (iter != _information.end())
    {
        _tasks[_information[taskName]]->stop();
    }
    else
    {
        printf("There is no task named %s !", taskName.c_str());
    }
};

void periodTaskManager::startAll()
{
    for (auto& task : _tasks)
    {
        task->start();
    }
};

void periodTaskManager::stopAll()
{
    for (auto& task : _tasks)
    {
        task->stop();
    }
};

void periodTaskManager::printTasks()
{
    for (auto& task : _tasks)
    {
        task->getName();
    }
};