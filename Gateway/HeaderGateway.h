
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

//Define
#define Buffer_size 32 
TCHAR nomeGwtoSr[] = TEXT("Buffer GwToSr");
TCHAR nomeSrtoGW[] = TEXT("Buffer SrToGw");


TCHAR semGwLer[] = TEXT("semaforoGwLer");
TCHAR semGwEscrever[] = TEXT("semaforoGwEscrever");
TCHAR mutexGwtoSer[] = TEXT("mutexGwtoSer");

TCHAR semSerLer[] = TEXT("semaforoSerLer");
TCHAR semSerEscrever[] = TEXT("semaforoSerEscrever");
TCHAR mutexSertoGw[] = TEXT("mutexSertoGw");
////////////////////////////////////////////

//prototipos

void IniciarGateway();

//Estruturas

typedef struct synbuff {

	HANDLE SemGwtoServComItem;
	HANDLE SemGwtoServSemItem;
	CRITICAL_SECTION MutexGwtoSer;

	HANDLE SemSerLer;
	HANDLE SemSerEscrever;
	CRITICAL_SECTION MutexSertoGw;

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
	//packet array;
	int tail;
	int head;

}bufferMsg, *ptrbufferMsg;

typedef struct Gestao_gateway {

	int sessionID;
	HANDLE hThreadGwtoSer;
	DWORD idThreadGwtoSer;
	bufferMsg *comSertoGw;
	bufferMsg *comGwtoSer; 

}dataGw;
