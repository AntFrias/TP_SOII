#pragma once

#include <windows.h>
#include <tchar.h>



#define TAM 256

#ifdef MEMORYACCESS_EXPORTS
#define MEMORYACCESS_API __declspec(dllexport) 
#else
#define MEMORYACCESS_API __declspec(dllimport) 
#endif

//prototipos das funções