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

enum MovimentoDaNave { //ver no lado do servidor se esta igual

	cima = 1,
	baixo,
	esquerda,
	direita,
	espaco,
	z,      //ativa powerUp1
	x,		//ativa powerUp2
	c,		//ativa powerUp3
};
enum EnumDados {

	bloco_vazio = 0,

	user_login = 1,
	user_login_sucesso,
	user_Login_falhou,
	user_login_Limite_clientes,
	max_players_atingido,

	IniciaJogoIndividual = 10,
	IniciaJogoMultiplayer,
	AtualizacaoJogo

};


//nome do Pipe de comunicaçao Servidor <-> Cliente
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

//define as dimensoes do mapa onde ocorrerá o jogo
#define dimMapa_x 20
#define dimMapa_y 30

//Tamanho do array de blocos ocupados
#define Blocoscupados 100

typedef struct configs {

	TCHAR nome[10];
	TCHAR CIMA, BAIXO, ESQUERDA, DIREITA;
	TCHAR POWERUP1, POWERUP2, POWERUP3;


}configur;

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
typedef struct alteracaoDoTab {

	int x, y;
	int tipo;
	int	cor;

}alteracaoTab;

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
