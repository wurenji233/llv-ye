#include"std.h"


bool DownloadFile(std::wstring url, std::wstring filepath)
{
	URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL);
//    return ((URLDownloadToFileW(NULL, url.c_str(), filepath.c_str(), 0, NULL) )== S_OK);
	return true;
}