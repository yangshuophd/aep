/**
* @file
* @brief �ڲ��㷨���cpp�ļ�.
* @author ruoxi
*
* ʵ�����ڲ��㷨�����.
*/

#include "internal_algorithm.h"

#include "algorithm_helper.h"

#include <algorithm>

using namespace std;

InternalAlgorithm::InternalAlgorithm()
{
}

InternalAlgorithm::InternalAlgorithm(UINT32 algorithmId,
                                     wstring name,
                                     wstring dllFileName,
                                     wstring funcName)
                                     :
m_AlgorithmId(algorithmId),
m_Name(name),
m_DllFileName(dllFileName),
m_FuncName(funcName)
{
}

InternalAlgorithm::~InternalAlgorithm()
{
}

UINT32 InternalAlgorithm::GetTypeId()
{
    return s_ComponentId;
}

void InternalAlgorithm::Destroy()
{
    delete this;
}

void *InternalAlgorithm::GetInterface(UINT32 iid)
{
    void *iface;

    if (CIID_ICOMPONENT == iid)
    {
        iface = static_cast<IComponent *>(this);
    }
    else if (CIID_IALGORITHM == iid)
    {
        iface = static_cast<IAlgorithm *>(this);
    }
    else
    {
        iface = NULL;
    }
    return iface;
}

UINT32 InternalAlgorithm::GetId()
{
    return m_Id;
}

void InternalAlgorithm::SetId(UINT32 id)
{
    m_Id = id;
}

wstring InternalAlgorithm::GetName()
{
    return m_Name;
}

void InternalAlgorithm::SetName(wstring name)
{
    m_Name = name;
}

void InternalAlgorithm::GetAttributeList(AttributeList &attributeList)
{
}

RC InternalAlgorithm::GetAttribute(UINT32 aid, void *attr)
{
    RC rc;

    return rc;
}

RC InternalAlgorithm::SetAttribute(UINT32 aid, void *attr)
{
    RC rc;

    return rc;
}

bool InternalAlgorithm::Connect(IComponent *component)
{
    IParam *param = (IParam *)component->GetInterface(CIID_IPARAM);
    if (NULL != param)
    {
        if (find(m_ParamList.begin(), m_ParamList.end(), param) == m_ParamList.end())
        {
            m_ParamList.push_back(param);
        }
        return true;
    }
    return false;
}

UINT32 InternalAlgorithm::GetAlgorithmId()
{
    return m_AlgorithmId;
}

void InternalAlgorithm::SetAlgorithmId(UINT32 algorithmId)
{
    m_AlgorithmId = algorithmId;
}

RC InternalAlgorithm::Run()
{
    RC rc;

    CHECK_RC(AlgorithmHelper::RunFunc(m_DllFileName, m_FuncName, m_ParamList));

    return rc;
}

LPCWSTR InternalAlgorithm::s_ComponentName = TEXT("�ڲ��㷨");