#include "algorithm_data_file.h"

AlgorithmDataFile::AlgorithmDataFile(wstring fileName)
:
DataFile(fileName)
{
}

AlgorithmDataFile::~AlgorithmDataFile()
{
}

AlgorithmList &AlgorithmDataFile::GetAlgorithmList()
{
    return m_AlgorithmList;
}

RC AlgorithmDataFile::InsertAlgorithm(const Algorithm &algorithm)
{
    RC rc;

    if (m_Lines.find(algorithm.GetName()) != m_Lines.end())
    {
        return RC::ALGORITHM_INSERT_ERROR;
    }

    wostringstream ost;
    ost << algorithm.GetName()
        << TEXT(" ")
        << algorithm.GetDllFileName()
        << TEXT(" ")
        << algorithm.GetFuncName()
        << TEXT(" ")
        << algorithm.GetIconFileName()
        << endl;
    m_Lines[algorithm.GetName()] = ost.str();

    CHECK_RC(Save());

    return rc;
}

RC AlgorithmDataFile::RemoveAlgorithm(wstring name)
{
    RC rc;

    if (m_Lines.find(name) == m_Lines.end())
    {
        return RC::ALGORITHM_REMOVE_ERROR;
    }

    m_Lines.erase(name);

    CHECK_RC(Save());

    return rc;
}

RC AlgorithmDataFile::ParseLine(const wstring &line)
{
    RC rc;

    wistringstream ist(line);

    wstring name, dllFileName, funcName, iconFileName;
    ist >> name >> dllFileName >> funcName >> iconFileName;

    ParamNameList paramNameList;
    while (!ist.eof())
    {
        wstring paramName;
        ist >> paramName;
        if (!paramName.empty())
        {
            paramNameList.push_back(paramName);
        }
    }

    m_AlgorithmList.push_back(Algorithm(name, dllFileName, funcName, iconFileName, paramNameList));

    return rc;
}