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
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>
#include <shellapi.h>
#include <winbase.h>



//Tamanho do Buffer de mensagens na memoria partilhada
#define Buffer_size 32
//numero de naves inimigas apenas para testes
#define ninimigas 100

// define as coordenadas no ecran para Mostrar as naves inimigas
#define CoordWindow_x 80
#define CoordWindow_y 1
// define as dimensoes do mapa onde ocorrerá o jogo
#define dimMapa_x 20
#define dimMapa_y 30

typedef struct Block {

	int tipo;
	int IdObjecto;
	int CorObjeto;
	
}Bloco;

typedef struct map {

	Bloco blocos[dimMapa_x][dimMapa_y];

}MaPa;

typedef struct synbuff {

	HANDLE SemGwtoServComItem;
	HANDLE SemGwtoServSemItem;
	HANDLE MutexGwtoSer;
	
	//CRITICAL_SECTION MutexGwtoSer;//dar outro nome

	//HANDLE SemSerLer;
	//HANDLE SemSerEscrever;
	//CRITICAL_SECTION MutexSertoGw;

}synBuffer;

typedef struct Packet {

	int tipo;
	int session_id;
	int pontuacao;

	union datPacket
	{
		TCHAR nome[10];
		int movimento;
		int tiro;
		//PowerUpp PowerUp;
	}dataPacket;

}packet;

typedef struct BufferMsg {

	packet array[Buffer_size];

	int tail;
	int head;

}bufferMsg, *ptrbufferMsg;

//Defenir algumas funçoes

#if defined AcessoMemDLL
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT _declspec(dllimport)
#endif


// agora os prototipos das funções
extern "C" {

	DLL_EXPORT int CriaSyncMemoria();
	DLL_EXPORT void CriaMemoriaPartilhada();
	DLL_EXPORT void escrevebufferGwToSr(Packet *aux);
	DLL_EXPORT 	Packet * LerBufferGwtoSer();
	DLL_EXPORT void gotoxy(int x, int y);

}

// TODO: reference additional headers your program requires here
