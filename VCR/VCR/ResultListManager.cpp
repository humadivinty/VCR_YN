#include "stdafx.h"
#include "ResultListManager.h"
#include "utilityTool/ToolFunction.h"


ResultListManager::ResultListManager()
{
}

ResultListManager::~ResultListManager()
{
    ClearALL();
}

size_t ResultListManager::size()
{
    std::unique_lock<std::mutex> locker(m_mtx);

    return m_list.size();
}

bool ResultListManager::empty()
{
    std::unique_lock<std::mutex> locker(m_mtx);

    return m_list.empty();
}

ResultListManager::Result_Type ResultListManager::front()
{
    std::unique_lock<std::mutex> locker(m_mtx);
    Result_Type value;
    if (!m_list.empty())
    {
        value = m_list.front();
    }
    return value;
}

ResultListManager::Result_Type ResultListManager::GetOneByIndex(int iPosition)
{
    std::unique_lock<std::mutex> locker(m_mtx);
    Result_Type value;
    if (m_list.empty())
    {
        return value;
    }

    int iPos = 0;
    for (list_Type::const_iterator it = m_list.cbegin(); it != m_list.cend(); it++)
    {
        if (iPos == iPosition)
        {
            value = *it;
            break;
        }
        iPos++;
    }
    return value;
}

void ResultListManager::pop_front()
{
    std::unique_lock<std::mutex> locker(m_mtx);
    Result_Type value;
    if (!m_list.empty())
    {
        m_list.pop_front();
    }
}

void ResultListManager::pop_back()
{
    std::unique_lock<std::mutex> locker(m_mtx);
    Result_Type value;
    if (!m_list.empty())
    {
        m_list.pop_back();
    }
}

void ResultListManager::push_back(Result_Type result)
{
    std::unique_lock<std::mutex> locker(m_mtx);
    if (result != NULL)
    {
        m_list.push_back(result);
    }    
}

int ResultListManager::GetPositionByPlateNo(const char* plateNo)
{
    std::unique_lock<std::mutex> locker(m_mtx);
    int iRet = -1;
    if (m_list.empty() || NULL == plateNo)
    {
        return iRet;
    }
    int iPos = 0;
    std::string strSrcPlate(plateNo);
    for (list_Type::const_iterator it = m_list.cbegin(); it != m_list.cend(); it++)
    {
        Result_Type value = *it;
        std::string strDestPlate(value->chPlateNO);
        if (std::string::npos != strSrcPlate.find(strDestPlate))
        {
            iRet = iPos;
            break;
        }
        else if (Tool_DimCompare(strSrcPlate.c_str(), strDestPlate.c_str()))
        {
            iRet = iPos;
            break;
        }
        else
        {
            iPos++;
        }
    }
    return iRet;
}

void ResultListManager::DeleteToPosition(int position)
{
    if ( position < 0)
    {
        return;
    }
    std::unique_lock<std::mutex> locker(m_mtx);
    if (!m_list.empty())
    {
        for (int i = 0; i <= position; i++)
        {
            m_list.pop_front();
        }
    }
}

std::string ResultListManager::GetAllPlateString()
{
    std::unique_lock<std::mutex> locker(m_mtx);
    std::string strPlateListString;
    if (m_list.empty())
    {
        return strPlateListString;
    }
    else
    {
        for (list_Type::const_iterator it = m_list.cbegin(); it != m_list.cend(); it++)
        {
            Result_Type value = *it;
            strPlateListString.append(value->chPlateNO);
            strPlateListString.append("\n");
        }
        return strPlateListString;
    }
}

void ResultListManager::ClearALL()
{
    std::unique_lock<std::mutex> locker(m_mtx);
    if (!m_list.empty())
    {
        m_list.clear();
    }
}

