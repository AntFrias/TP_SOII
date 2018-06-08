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

//Numero Max de Jogadores
#define nMaxPlay 5

//Tamanho do Buffer de mensagens na memoria partilhada
#define Buffer_size 32

// define as coordenadas no ecran para Mostrar as naves inimigas
#define CoordWindow_x 80
#define CoordWindow_y 1

// define as dimensoes do mapa onde ocorrerá o jogo
#define dimMapa_x 20 //210
#define dimMapa_y 30 //255
#define areaDoBloco 45

//Tamanho do array de blocos ocupados
#define Blocoscupados 100

// Mensages trocadas entre Servidor e Gateway
enum EnumDados {

	bloco_vazio = 0,

	user_login = 1,
	user_login_sucesso,
	user_Login_falhou,
	user_login_Limite_clientes,



	IniciaJogoIndividual = 10,
	IniciaJogoMultiplayer,
	AtualizacaoJogo



};
// informaçao acerca dos objetos do Mapa
enum OjectosTabuleiro {

	NaveBasica = 1,
	NavesEsquiva,
	NaveBoss,
	NaveJogador,

	PowerUpEscudo,
	PowerUpGelo,
	PowerUpBateria,
	PowerUpMais,
	PowerUpVida,
	PowerUpAlcool,
	PowerUpNuclear

};
enum MovimentoDaNave { //ver no lado do cliente se está igual

	cima = 1,
	baixo,
	esquerda,
	direita,
	espaco,
	z,      //ativa powerUp1
	x,		//ativa powerUp2
	c,		//ativa powerUp3
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funçoes estruras de suporte a memoria partilhada TAbuleiro que serao enviadas ao Cliente
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct BlocoMemPartilhada {

	int tipo;
	int Cor;
	
}Bloco;

typedef struct TabuleiroMemPartilhada{

	Bloco blocos[dimMapa_x][dimMapa_y];
	int items;
	int alteracoes;

}TabMemPartilhada;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funçoes estruras de suporte a memoria partilhada 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct synbuff {

	HANDLE SemGwtoServComItem;
	HANDLE SemGwtoServSemItem;
	HANDLE MutexGwtoSer;
	HANDLE SemServtoGwComItem;
	HANDLE SemServtoGwSemItem;
	HANDLE MutexServtoGw;
	HANDLE MutexAtualizaTab;

}synBuffer;
typedef struct alteracaoDoTab {

	int x, y;
	int tipo;
	int	cor;

}alteracaoTab;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funçoes estruras de suporte para envio de pacotes e dados
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Packet {

	int tipo;
	DWORD Cliente_id;
	int pontuacao;
	int numObjetos;

	union datPacket
	{
		TCHAR nome[10];
		int movimento;
		int tiro;
		//PowerUpp PowerUp;
		alteracaoTab arrayTab[Blocoscupados];

	}dataPacket;

}packet;

typedef struct BufferMsg {

	packet array[Buffer_size];

	int tail;
	int head;

}bufferMsg, *ptrbufferMsg;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined AcessoMemDLL
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT _declspec(dllimport)
#endif


extern "C" {

	DLL_EXPORT EnumDados;
	DLL_EXPORT OjectosTabuleiro;

	DLL_EXPORT TCHAR mutexAtualizaBloco[];

	DLL_EXPORT TCHAR nomeGwtoServ[];
	DLL_EXPORT TCHAR nomeServtoGw[];


	DLL_EXPORT void CriaMemoriaPartilhada();
	DLL_EXPORT void escrevebuffer(packet *aux, TCHAR *nomeBuffer);
	DLL_EXPORT Packet * LerBufferGwtoSer();
	DLL_EXPORT Packet * LerBufferServtoGw();
	DLL_EXPORT Packet * LerBufferTabuleiro();
	DLL_EXPORT void mostraTabCom();
	DLL_EXPORT void tabServCPYtabCom(int x, int y, int tipo);
	DLL_EXPORT void gotoxy(int x, int y);

}

// TODO: reference additional headers your program requires here
