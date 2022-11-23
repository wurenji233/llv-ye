#include"std.h"
#define _AFXDLL
#include<afxinet.h>

//从URL获取字符串
std::wstring GetDataFromURL(std::wstring url,bool iserr)
{
    CInternetSession session;

    CHttpFile* file = NULL;

    CString strURL = url.c_str();

    CString strHtml = _T("");   //存放网页数据



        try {

        file = (CHttpFile*)session.OpenURL(strURL);

    }
    catch (CInternetException* m_pException) {

        file = NULL;

        m_pException->m_dwError;

        m_pException->Delete();

        session.Close();

        return std::wstring();

    }

    CString strLine;

    if (file != NULL) {

        while (file->ReadString(strLine) != NULL) {

            strHtml += strLine;

        }



    }
    else {

        return std::wstring();

    }



    session.Close();

    file->Close();

    delete file;

    file = NULL;

    return std::wstring(strHtml);
}