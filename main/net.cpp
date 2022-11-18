#include"std.h"


#include<wininet.h>
#pragma comment(lib, "Wininet.lib")

bool DownloadFile(std::wstring url, std::wstring filepath)
{
    return (URLDownloadToFileW(NULL, url.c_str(), filepath.c_str(), 0, NULL) == S_OK);
}