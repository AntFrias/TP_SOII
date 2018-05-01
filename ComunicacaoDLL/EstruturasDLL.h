#pragma once
#include <Windows.h>
#include <tchar.h>

typedef struct nav {
	int tipo;
	int x, y;
	int vida;
	int escudo;
	int sessionId;
	HANDLE NaveInvasoras;
	DWORD NaveInvthreadId;

}Nave;

typedef struct PoUP {

	int powerUp;
	int duracao;
	int ocorrencia;

}PowerUpp;

typedef struct mi {

	int x, y;
	int tipo;
	int session_id;

}Misil;

typedef struct EleMapa {

	int objeto;

	union mapObject {
		Nave NaveEnimiga;
		Nave NaveDefensora;
		Misil Missil;
		PowerUpp PowerUp;
	}mapaObject;

}Elemento;

typedef struct Packet {

	int tipo;
	int session_id;
	int pontuacao;

	union datPacket
	{
		TCHAR nome[10];
		int movimento;
		int tiro;
		PowerUpp PowerUp;
	}dataPacket;

}packet;

typedef struct AuxSinc {

	//handles para os semaforaos 

}AuxSincronize;
