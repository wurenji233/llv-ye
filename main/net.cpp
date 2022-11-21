#include"std.h"

#include<wininet.h>


#pragma comment(lib,"URLmon")
#pragma comment(lib, "Wininet.lib")

bool DownloadFileFromURL(std::wstring url, std::wstring filepath)
{
    return ((URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL) )== S_OK);
}

