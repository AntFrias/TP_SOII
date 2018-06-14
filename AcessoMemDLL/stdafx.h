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
#define nMaxJogadores 5

//Tamanho do Buffer de mensagens na memoria partilhada
#define Buffer_size 32

// define as coordenadas no ecran para Mostrar as naves inimigas
#define CoordWindow_x 80
#define CoordWindow_y 1

// define as dimensoes do mapa onde ocorrerá o jogo
#define dimMapa_x 40 //210
#define dimMapa_y 40 //255
#define LarguraNaveDefault 2 // 1 porque é o valor que vamos incrementar ao valor atual para que no total a nave tenha tamanho 2

////Tamanho do array de blocos ocupados que vai ser enviado ao Cliente
#define Blocoscupados 100




// Mensages trocadas entre Servidor < - > Gateway < - > Cliente
enum EnumDados {

	user_login = 1,				//Pedido de login pelo Cliente
	user_login_sucesso,			//inicio das respostas do login
	user_Login_falhou,
	user_login_Limite_clientes,

	IniciaJogoIndividual = 10,	// pedido de inicio de jogo pelo cliente
	IniciaJogoMultiplayer,
	AtualizacaoJogo,			// resposta ao pedido de inicio de jogo

	ComandosJogador = 20,		// pedido pelo cliente ao ter digitado teclas
	cima,
	baixo,
	esquerda,
	direita,
	Tiro,			// manda tiro 
	PowerUp1,       //ativa powerUp1
	PowerUp2,		//ativa powerUp2
	PowerUp3		//ativa powerUp3

};
// informaçao acerca dos objetos do Mapa

enum OjectosTabuleiro {

	bloco_vazio = 0,
	NaveBasica = 1,
	NaveEsquiva,
	NaveBoss,
	NaveJogador,
	tiroJogador = 1,
	tiroNaveEnemy,
	tiroNuclear,  // missil divide-se em 2
	tiroBoss,    // lança 2 misseis de cada vez nos extremos da nave

	PowerUpEscudo,
	PowerUpGelo,
	PowerUpBateria,
	PowerUpMais,
	PowerUpVida,
	PowerUpAlcool,
	PowerUpNuclear

};

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

typedef struct alteracaoDoTab {

	int x, y;
	int tipo;
	int	cor;

}alteracaoTab;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funçoes estruras de suporte para envio de pacote com dados
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Packet {

	int tipo;
	DWORD Cliente_id;
	int pontuacao;
	int numObjetos;

	union datPacket
	{
		TCHAR nome[10];
		int comando;
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

	DLL_EXPORT void escreveBufferTabuleiro(int x, int y, int tipoObjeto);
	DLL_EXPORT void CriaMemoriaPartilhada();
	DLL_EXPORT void escrevebuffer(packet *aux, TCHAR *nomeBuffer);
	DLL_EXPORT Packet * LerBufferGwtoSer();
	DLL_EXPORT Packet * LerBufferServtoGw();
	DLL_EXPORT Packet * LerBufferTabuleiro();
	DLL_EXPORT void mostraTabCom();
	DLL_EXPORT void gotoxy(int x, int y);

}

// TODO: reference additional headers your program requires here
