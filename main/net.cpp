#include"std.h"


bool DownloadFileFromURL(std::wstring url, std::wstring filepath)
{
    return ((URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL) )== S_OK);
}

//δʵ��
bool GetDataFromURL(std::wstring url)
{
    return false;
}