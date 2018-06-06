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

//Tamanho do array de blocos ocupados
#define Blocoscupados 100

typedef struct estrCli {

	int cor;
	int alive;
	TCHAR nome[50];
	HANDLE ht;
	DWORD IDth;
	HANDLE pipe;

}EstruturaCli;

typedef struct BlocoMemPartilhada {

	int tipo;
	int Cor;

}Bloco;

typedef struct Packet {

	int tipo;
	DWORD Cliente_id;
	int pontuacao;

	union datPacket
	{
		TCHAR nome[10];
		int movimento;
		int tiro;
		//PowerUpp PowerUp;
		Bloco tabuleiro[Blocoscupados];

	}dataPacket;

}packet;
