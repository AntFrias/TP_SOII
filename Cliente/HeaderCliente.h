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

enum EnumDados {

	user_login = 1,				//Pedido de login pelo Cliente
	user_login_sucesso,			//inicio das respostas do login
	user_Login_falhou,
	user_login_Limite_clientes,

	IniciaJogoMultiplayer = 10,
	JogoIniciado,

	AtualizacaoJogo = 20,			// resposta ao pedido de inicio de jogo
	cima,
	baixo,
	esquerda,
	direita,
	LancaTiro,			// manda tiro 
	PowerUp1,       //ativa powerUp1
	PowerUp2,		//ativa powerUp2
	PowerUp3		//ativa powerUp3

};

//nome do Pipe de comunicašao Servidor <-> Cliente
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

#define escala 20

//Tamanho do array de blocos ocupados
#define Blocoscupados 100

typedef struct img {

	HBITMAP Wallpaper;
	HBITMAP Basica;
	HBITMAP Bomba;
	HBITMAP Defensora1;
	HBITMAP Defensora2;
	HBITMAP Esquiva;
	HBITMAP Tiro;
	HBITMAP Boss;
	HBITMAP Space;

}bipm;

typedef struct hcds {

	HDC Wallpaper;
	HDC Basica;
	HDC Bomba;
	HDC Defensora1;
	HDC Defensora2;
	HDC Esquiva;
	HDC Tiro;
	HDC Boss;
	HDC Space;


}hdcImg;

typedef struct configs {

	TCHAR nome[10];
	TCHAR TIRO;
	TCHAR CIMA, BAIXO, ESQUERDA, DIREITA;
	TCHAR POWERUP1, POWERUP2, POWERUP3;


}configur;

typedef struct estrCli {

	int cor;
	int alive;
	TCHAR nome[50];
	HANDLE htEscuta,htEnvia;
	DWORD IDEscuta, IDEnvia;
	HANDLE pipe, EventEnvia;


}EstruturaCli;

typedef struct BlocoMemPartilhada {

	int tipo;
	int Cor;

}Bloco;
// informašao acerca dos objetos do Mapa
enum OjectosTabuleiro {

	bloco_vazio = 0,
	NaveBasica = 1,
	NavesEsquiva,
	NaveBoss,
	NaveJogador,
	tiroJogador,
	bombaInimiga,

	PowerUpEscudo,
	PowerUpGelo,
	PowerUpBateria,
	PowerUpMais,
	PowerUpVida,
	PowerUpAlcool,
	PowerUpNuclear

};
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
		int comando;
		//PowerUpp PowerUp;
		alteracaoTab arrayTab[Blocoscupados];

	}dataPacket;

}packet;
