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
// Estrutura de suporte ás naves
typedef struct naves {
	int tipo;
	int x, y;
	int vida;
	int escudo;
	int sessionId;
	HANDLE NaveInvasoras;
	DWORD NaveInvthreadId;

}Nave;
// extrutura Configuraçao Inicial do Jogo
typedef struct ConfiguracaoInicialJogo {				

	int MaxJogadores;
	int MaxNavesInimigas;
	int MaxPowerups;
	int DuracaoPowerup;
	int Vidas;
}confInitJogo;

// Extrutura Jogador
typedef struct Jogador_Info {							

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

//extrutura cria registry to put option server on = 1
typedef struct RegistryServer {							

	HKEY Chave;
	DWORD statServer, ServerUp;
	
}registryServer;

// Extrutura com os dados do servidor;
typedef struct Gestao_servidor{							

	int NumMaxClientes;
	BOOL inicioJogo;
	BOOL ServidorUp;
	confInitJogo initJogo;
	registryServer ServerUp;
	HANDLE hThreadSerToGw;
	DWORD IdThreadSertoGw;

}dataServer;

// extrutura TOP10
typedef struct Top10 {								
	TCHAR name[10];
	int	pontuacao;
}top10;








