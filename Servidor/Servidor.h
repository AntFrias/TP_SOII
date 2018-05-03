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

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//									POR NA DLL
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Nome do Buffer Server to Gateway
TCHAR nomeSrtoGW[] = TEXT("Buffer SrToGw");
TCHAR nomeGwtoSr[] = TEXT("Buffer GwToSr");

// Nome dos Semaforos para comunicacao nos Buffers de mensagens
TCHAR semGwLer[] = TEXT("semaforoGwLer");
TCHAR semGwEscrever[] = TEXT("semaforoGwEscrever");
TCHAR mutexGwtoSer[] = TEXT("mutexGwtoSer");

TCHAR semSerLer[] = TEXT("semaforoSerLer");
TCHAR semSerEscrever[] = TEXT("semaforoSerEscrever");
TCHAR mutexSertoGw[] = TEXT("mutexSertoGw");

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

typedef struct synbuff {

	HANDLE SemGwtoSerPack;
	HANDLE SemGwtoSerPos;
	HANDLE MutexGwtoSer;

	HANDLE SemSerLer;
	HANDLE SemSerEscrever;
	HANDLE MutexSertoGw;

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

typedef struct BufferMsg {						    // Representaçao do Buffer 

	packet array[Buffer_size];
	int in, out;

}bufferMsg, *ptrbufferMsg;

											
typedef struct nav {								// Estrutura para ser apagada -> vai para a DLL
	int tipo;
	int x, y;
	int vida;
	int escudo;
	int sessionId;
	HANDLE NaveInvasoras;
	DWORD NaveInvthreadId;

}Nave;

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ConfiguracaoInicialJogo {				// extrutura Configuraçao Inicial do Jogo

	int MaxJogadores;
	int MaxNavesInimigas;
	int MaxPowerups;
	int DuracaoPowerup;
	int Vidas;
}confInitJogo;


typedef struct Jogador_Info {							// Extrutura Jogador

	TCHAR nome[10];
	int pontuacao;
	int tempo;
	int posicao[2];
	int vidas;
	int sessionId;
	confInitJogo jogo;
	//naveDefensora nave;
	//PowerUp Powerup;
}jogadorinfo;


typedef struct RegistryServer {							//extrutura cria registry to put option server on = 1

	HKEY Chave;
	DWORD statServer, ServerUp;
	
}registryServer;


typedef struct Gestao_servidor{							// Extrutura com os dados do servidor;

	int NumMaxClientes;
	BOOL inicioJogo;
	BOOL ServidorUp;
	confInitJogo initJogo;
	registryServer ServerUp;
	bufferMsg *comSertoGw;
	bufferMsg *comGwtoSer;
	HANDLE hThreadSerToGw;
	DWORD IdThreadSertoGw;

}dataServer;


typedef struct Top10 {								// extrutura TOP10
	TCHAR name[10];
	int	pontuacao;
}top10;








