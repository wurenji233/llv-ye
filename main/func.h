#pragma once

#include"std.h"



bool DownloadFileFromURL(std::wstring url, std::wstring filepath);
void GetPrivileges();
void ReadPHYSICALDRIVE(unsigned int id);
void RegAutoStart();
void MakeBlueScreen(unsigned int errid);
void PlaySoundFile(std::wstring soundname, bool sync);