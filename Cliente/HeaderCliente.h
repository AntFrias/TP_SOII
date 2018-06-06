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


//nome do Pipe de comunicaçao Servidor <-> Cliente
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

//define as dimensoes do mapa onde ocorrerá o jogo
#define dimMapa_x 20
#define dimMapa_y 30


typedef struct estrCli {

	int cor;
	int alive;
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
