#pragma once

#include "Camera/CameraResult.h"
#include <memory>
#include <deque>
#include <mutex>

class ResultListManager
{
    typedef std::shared_ptr<CameraResult> Result_Type;
    typedef std::deque<Result_Type> list_Type;
public:
    ResultListManager();
    ~ResultListManager();

public:
    size_t size();
    bool empty();

    Result_Type front();
    Result_Type GetOneByIndex(int iPosition);
    void pop_front();
    void pop_back();
    void push_back(Result_Type result);

    int GetPositionByPlateNo(const char* plateNo);
    void DeleteToPosition(int position);

    std::string GetAllPlateString();
    
    void ClearALL();
private:
    list_Type m_list;
    std::mutex m_mtx;
};

