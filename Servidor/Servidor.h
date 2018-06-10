#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <wchar.h>
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>
#include <shellapi.h>
#include <winbase.h>

//max powerUp
#define maxClientePowerUp 3

//Velocidade base das naves enimigas
#define velociadadeNaveBasica 200 

//Velocidade base das naves enimigas
#define velociadadeNaveEsquiva 10 

//numero de naves inimigas apenas para testes
#define ninimigas1 5
#define ninimigas2 5

//Tiros
enum EnumTiros {
	
	tiroJogador= 1,
	tiroNaveEnemy,
	tiroNuclear,
	tiroBoss

};

// Estrutura Tiro
typedef struct tiroo {

	int idJogador;
	int x, y;
	int posJogador; // no array para identificar de quem é o tiro

}tiro;

// Estrutura de suporte ás naves
typedef struct naves {
	int tipo;
	int x, y;
	int vida;
	int escudo;
	int taxaDisparo;
	int velocidadeBase;
}Nave;

// extrutura Configuraçao Inicial do Jogo
typedef struct ConfiguracaoInicialJogo {				
	int MaxJogadores;
	int MaxNavesInimigas1;
	int MaxNavesInimigas2;
	int MaxNavesInimigas3;
	int taxaDeDiparoBasica;

	int MaxPowerups;
	int DuracaoPowerup;
	int VidasJogador;
}confInitJogo;

typedef struct DadosDoGame {

	Nave *NaveEnemyTipo1, *NaveEnemyTipo2, *NaveEnemyTipo3;

}DadosdoJogo;

// Extrutura Jogador
typedef struct Jogador_Info {							

	TCHAR nome[10];
	int IdJogador;
	int pontuacao;
	int tempo;
	int posicao[2];
	int vidas;
	Nave nave;
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
	int NumCliNoArray;
	BOOL inicioJogo;
	BOOL ServidorUp;
	confInitJogo initJogo;
	registryServer ServerUp; // ver se isto vai ser necessario
	HANDLE hThreadSerToGw;
	DWORD IdThreadSertoGw;
	HANDLE mutexTabuleiro;

}dataServer;

typedef struct BlocoTabServidor {

	int tipo;
	int id;
	int posArray;
	//pensar na cor

}BlocoServ;

// extrutura TOP10
typedef struct Top10 {								
	TCHAR name[10];
	int	pontuacao;
}top10;








