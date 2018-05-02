
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
TCHAR nomeSrtoGW[] = TEXT("Buffer SrToGw");
TCHAR nomeGwtoSr[] = TEXT("Buffer GwToSr");

//prototipos

void IniciarGateway();

//Estruturas

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
	unsigned int in, out;

}bufferMsg, *ptrbufferMsg;

typedef struct Gestao_gateway {


	int sessionID;
	bufferMsg *comSertoGw;
	bufferMsg *comGwtoSer;


}dataGw;
