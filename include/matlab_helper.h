/**
* @file
* @brief MatLab��������ͷ�ļ�.
* @author ruoxi
*
* ������MatLab��������.
*/

#ifndef __MATLAB_HELPER_H__
#define __MATLAB_HELPER_H__

#include "algorithm.h"

typedef mxArray Array;

namespace MatLabHelper
{
    typedef bool (*InitializeFunc)(void);
    /** @brief ʵ����DLL�е��㷨��ʼ��������ǰ׺. */
    extern LPCSTR InitializeFuncPrefix;

    /** @brief ʵ����DLL�е���ں�����ǰ׺. */
    extern LPCSTR FuncPrefix;

    typedef void (*PrintStackFunc)(void);
    extern LPCSTR PrintStackFuncPrefix;

    typedef void (*TerminateFunc)(void);
    /** @brief ʵ����DLL�е��㷨���ٺ�����ǰ׺. */
    extern LPCSTR TerminateFuncPrefix;

    Array *CreateDoubleArray(UINT32 x, UINT32 y,
        const char *content,
        UINT32 width, UINT32 height,
        UINT32 startX, UINT32 startY);
    Array *CreateDoubleArray(UINT32 x, UINT32 y,
        const double *content,
        UINT32 width, UINT32 height,
        UINT32 startX, UINT32 startY);
    Array *CreateDoubleArray(UINT32 x, UINT32 y,
        const char *content,
        UINT32 size, UINT32 start);
    Array *CreateDoubleArray(UINT32 x, UINT32 y,
        const double *content,
        UINT32 size, UINT32 start);
    void DestroyArray(Array *a);

    RC RunFunc(wstring dllFileName, wstring funcName, vector<Array *> &outputList, vector<Array *> &inputList);
    RC RealRunFunc(HINSTANCE algorithmDllHandle, string fullFuncName, vector<Array *> &outputList, vector<Array *> &inputList, bool &result);
};

#endif // __MATLAB_HELPER_H__