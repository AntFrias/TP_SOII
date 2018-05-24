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

#define PipeName TEXT("\\\\.\\Pipe\\PipeCliente")


typedef struct estrCli {

	int cor;
	TCHAR nome[50];
	HANDLE ht;
	DWORD IDth;
	HANDLE pipe;

}EstruturaCli;

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
