//
// Created by Hughzcl on 2021/5/23.
//

#ifndef TRIGGERFORPI_LOCKDATA_H
#define TRIGGERFORPI_LOCKDATA_H
#include <vector>
#include <mutex>
#include <condition_variable>
using namespace  std;
template<typename T>
class LockData
{
public:

    LockData()
    {
        _update = false;
    }

    explicit LockData(unsigned char capacity)
    {
        _update = false;
        _capacity = capacity;
    }

    void setCapacity(unsigned char capacity)
    {
        _capacity = capacity;
    }

    void push(T &newElem)
    {
        std::lock_guard<std::mutex> lk(_mutex);
        while (_lockData.size() >= _capacity){
            _lockData.pop_back();
        }
        _lockData.push_back(newElem);
        _update = true;
        _cv.notify_one();
    }

    void push(T* newElem, u_int8_t len)
    {
        std::lock_guard<std::mutex> lk(_mutex);

        for (int i = 0; i < len; ++i)
        {
            while (_lockData.size() >= _capacity)
            {
                _lockData.pop_back();
            }
            _lockData.push(*(newElem + i));
        }
        _update = true;
        _cv.notify_one();
    }

    T getData(u_int8_t SerNum)
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _cv.wait(lk, [this]
        { return !_lockData.empty(); });
        return _lockData[SerNum];
    }

    void pop_wait(T *receiver)
    {
        std::unique_lock<std::mutex> lk(_mutex); //?????????????????????????
        _cv.wait(lk, [this]
        { return !_lockData.empty(); });
        *receiver = _lockData.back();
        _update = false;
    }

    void pop_wait(T *receiver, u_int8_t len)
    {
        std::unique_lock<std::mutex> lk(_mutex);
        _cv.wait(lk, [this]
        { return !_lockData.empty(); });

        for (int i = 0; i < len; ++i)
        {
            *(receiver + i) = *(&(_lockData.back()) - (len - 1) + i);
        }
        _update = false;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lk(_mutex);
        std::vector<T> empty;
        std::swap(empty, _lockData);
    }

private:
    std::vector<T> _lockData;
    std::mutex _mutex;
    std::condition_variable _cv;
    u_int8_t _capacity;
    bool _update;
};


#endif //TRIGGERFORPI_LOCKDATA_H
