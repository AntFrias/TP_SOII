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

// extrutura Configuraçao Inicial do Jogo
typedef struct ConfiguracaoInicialJogo {

	int MaxJogadores;
	int MaxNavesInimigas;
	int MaxPowerups;
	int DuracaoPowerup;
	int Vidas;
}confInitJogo;

// Extrutura Jogador
typedef struct Clientes_Info {

	TCHAR nome[10];
	int pontuacao;
	int tempo;
	int posicao[2];
	int vidas;
	int sessionId;
	confInitJogo jogo;
	//naveDefensora nave;
	//PowerUp Powerup;
}clientesInfo;

// Extrutura com os dados do servidor;
typedef struct Gestao_servidor{
	int NumNavesInvasoras;
	int NumClients;
	int NumJogadores;
	DWORD ComGatewaythreadId;
	HANDLE ComGateway;
	BOOL inicioJogo;
	HANDLE LerMemoriaMutex;
}gestao_servidor;

// extrutura TOP10
typedef struct Top10 {
	TCHAR name[10];
	int	pontuacao;
}top10;







