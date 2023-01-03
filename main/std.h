#pragma once

#include <SDKDDKVer.h>
#ifndef WINVER
#define WINVER  _WIN32_WINNT_WIN10
#endif

#include<Windows.h>
#include<winhttp.h>
#include<WinInet.h>
#include<tchar.h>
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<thread>
#include<string>
#include<sstream>
#include<vector>
#include<algorithm>
#include<mmsystem.h>
#include<TlHelp32.h>
#include<wininet.h>
#include <winternl.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"wininet.lib")

