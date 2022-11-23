#pragma once

#include <SDKDDKVer.h>
#ifndef _WIN32_WINNT
#define WINVER  _WIN32_WINNT_WIN10
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#include <afx.h>
#include <afxwin.h>
#endif
//#include <Windows.h>
#include<tchar.h>
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<string.h>
#include<thread>
#include<string>
#include<sstream>
#include <mmsystem.h>
#include <TlHelp32.h>

#pragma comment(lib,"winmm.lib")

