// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <string>
#include <iostream>

//Defenir algumas funçoes
#if defined AcessoMemDLL
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT _declspec(dllimport)
#endif
// agora os prototipos das funções
extern "C" {
	DLL_EXPORT int sum(int a, int b);
	
}


// TODO: reference additional headers your program requires here
