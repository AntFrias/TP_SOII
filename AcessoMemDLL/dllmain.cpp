// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
//criar detallhes para as funçoes
#define AcessoMemDLL


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
//aqui podemos fazer a funções que declaramos no .h
extern "C" {
	
	int sum(int a, int b) {
		int soma = 0;
			soma = a + b;
		return soma;
	}

}


