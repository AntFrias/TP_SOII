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

//

//max powerUp
#define maxClientePowerUp 3

//Velocidade base das naves enimigas
#define velociadadeNaveBasica 200 

//Velocidade base das naves enimigas
#define velociadadeNaveEsquiva 10 

//numero de naves inimigas apenas para testes
#define ninimigas1 38
#define ninimigas2 10

#define VidaNaveDefault 3

#define NavesPorLinha 20

//Tiros



typedef struct GestorTirosNaves {

	HANDLE MutexTiroArray;
	HANDLE EventoLancaTiro;
	HANDLE MutexTabuleiro;
	HANDLE AtualizaTabuleiro;
	int TotalTiros;
	int ServerUp = 1;
	

}gestorTiros;

// //Estrutura Tiro
typedef struct tiroo {

	int tipo;
	int x, y;
	int vida;
	int posProprietario; // no array para identificar de quem � o tiro

}tiro;

typedef struct BlocoTabServidor {

	int tipo;
	int id;
	int posArray;
	//pensar na cor

}BlocoServ;

// Estrutura de suporte �s naves
typedef struct naves {
	int tipo;
	int x, y;
	int orientacao;
	int vida;
	int escudo;
	int taxaDisparo;
	int velocidadeBase;
}Nave;

typedef struct DadosJogo {

	Nave *NaveEnemyTipo1, *NaveEnemyTipo2, NaveEnemyTipo3;

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

// extrutura Configura�ao Inicial do Jogo
typedef struct ConfiguracaoInicialJogo {
	int MaxJogadores;
	int MaxNavesBasicas;
	int MaxNavesEsquivas;
	int MaxNaveBoss;
	int taxaDisparoNaveBasica;

	int MaxPowerups;
	int DuracaoPowerup;
	int VidasJogador;
}confInitJogo;

//extrutura cria registry to put option server on = 1
typedef struct RegistryServer {

	HKEY Chave;
	DWORD statServer, ServerUp;

}registryServer;

// extrutura TOP10
typedef struct Top10 {
	TCHAR name[10];
	int	pontuacao;
}top10;

// Extrutura com os dados do servidor;
typedef struct Gestao_servidor {

	int NumMaxClientes;
	int estadoJogo;
	int NumCliNoArray;
	BOOL inicioJogo;
	BOOL ServidorUp;
	confInitJogo initJogo;
	//registryServer ServerUp; // ver se isto vai ser necessario
	HANDLE hThreadSerToGw;
	DWORD IdThreadSertoGw;
	HANDLE mutexTabuleiro;
	HANDLE EventoIniciaJogo;
	HANDLE EventoAtualizaJogo;


}dataServer;

//prototipos das fun�oes relativas �s Naves no ficheiro Naves.cpp
void colocaNavesEsquiva();
void colocaNavesBasicas();

//prototipos de fun�oes relativas ao Jogo no ficheiro jogo.cpp

//ficheiro Servidor
void alteraPosicaoObjeto(int PosObjeto, int tipoObjeto, int *x, int *y);



//ficheiro Jogo
void limpaTabuleiro();
void mostraTabuleiro();
int VerificaVidaNave(int tipoObjeto, int PosObjeto);
int VerificaPosicaoPreencheTAb(int *x, int *y);
int VerificaPosicaoJogo(int *x, int *y, int tipo, int orientacao);
void LimpaPosTabuleiro(int x, int y, int tipo, int Largura);
void preencheBlocosServidor(int *x, int *y, int pos, int tipo, int Largura);	 // esta funcao serve para preencher os blocos do tabuleiro
void ColocaNavesTab();
void IniciarJogo(int *x,int *y,int pos);	// funcao que vai inicar o jogo propriamente dito quando os jogadores decidirem come�ar a jogar
void preencheBlocosServidorTiro(int *x, int *y, int pos, int tipo, int Largura);
void LimpaPosTabuleiroTiro(int x, int y, int tipo, int Largura);


//ficheiro Tiros

int AlteraVidaObjeto(int x, int y, int tipoObjeto, HANDLE atualizaTab);
void AdicionaTiroArray(int x, int y, int tipo, int PosObjeto);
void GestorTirosTab();
void AlteraPosicaoTiro(int PosObjeto, int *x, int *y);
void InicializaArrayTiros();
void IniciaDadosTiros(HANDLE mutexTabuleiro, HANDLE AtualizaTabuleiro);
void IniciaSincronizacaoTiros();






