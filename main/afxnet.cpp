#include"std.h"

#include<afxinet.h>
//δʵ��
std::wstring GetDataFromURL(std::wstring url)
{
    CInternetSession session;

    CHttpFile* file = NULL;

    CString strURL = " http://www.20abcd.com";

    CString strHtml = "";   //�����ҳ����



        try {

        file = (CHttpFile*)session.OpenURL(strURL);

    }
    catch (CInternetException* m_pException) {

        file = NULL;

        m_pException->m_dwError;

        m_pException->Delete();

        session.Close();

        return NULL;

    }

    CString strLine;

    if (file != NULL) {

        while (file->ReadString(strLine) != NULL) {

            strHtml += strLine;

        }



    }
    else {

        return NULL;

    }



    session.Close();

    file->Close();

    delete file;

    file = NULL;

    return NULL;
}