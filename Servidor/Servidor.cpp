#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")


//Nome para os mecanismos de sincronizaçao do servidor
TCHAR EventIniciajogo[] = TEXT("EventoIniciaJogo");

jogadorinfo *ArrayJogadores;

DadosdoJogo objectosTab;

dataServer dadosServidor;

//cenas copiar para a versão princial coisas da grafica
#include  "resource.h"//coisas da grafica
HWND hwndPrincipal;
HDC janelaImprime;
HANDLE hIniciaJogo;
DWORD idIniciaJogo;
HBITMAP Wallpaper;
HDC HdcWallpaper;
HDC janelaAux;


// funcao vai apagar o jogador do array de jogadores
jogadorinfo * apagaClienteArrayJogadores(int PosJogador) {

	jogadorinfo *aux;

	int totaljogadores = dadosServidor.NumMaxClientes, j = 0;

	if (dadosServidor.NumMaxClientes > 1) {

		dadosServidor.NumMaxClientes -= 1;

		aux = (jogadorinfo*)malloc(sizeof(jogadorinfo) * dadosServidor.NumMaxClientes);

		if (aux == NULL) {

			exit(-1);
		}

		for (int i = 0; i < totaljogadores; i++) {

			if (i != PosJogador) {

				aux[j] = ArrayJogadores[i];
				j++;
			}
		}
	}
	else {

		free(ArrayJogadores);

		return NULL;
	}

	return aux;

}
//////////////////////////////////////////////////////
int criaStatusServerRegistry(int n) {

	registryServer StatServer;

	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\TP_SOII
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\TP_SOII"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &StatServer.Chave, &StatServer.statServer) != ERROR_SUCCESS) {
		return -1;
	}
	else {
		if (n == 1) {
			StatServer.ServerUp = 1;
			RegSetValueEx(StatServer.Chave, TEXT("Servidor"), 0, REG_DWORD, (LPBYTE)&StatServer.ServerUp, sizeof(DWORD));
		}
		else {
			RegSetValueEx(StatServer.Chave, TEXT("Servidor"), 0, REG_DWORD, (LPBYTE)&StatServer.ServerUp, sizeof(DWORD));
		}
	}
	return 0;
}
//////////////////////////////////////////////////////

void mostraNaveBasica() {

	for (int i = 0; i < dadosServidor.initJogo.MaxNavesBasicas; i++) {

		_tprintf(TEXT("\npos da nave [%d][%d]"), objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y);  

	}
}

int VerificaVidaNave(int tipoObjeto, int PosObjeto) {

	int nVidas = 0;

	switch (tipoObjeto) {

	case NaveBasica:

		if (objectosTab.NaveEnemyTipo1[PosObjeto].vida > 0) {

			objectosTab.NaveEnemyTipo1[PosObjeto].vida -= 1;

			nVidas = objectosTab.NaveEnemyTipo1[PosObjeto].vida;

			return nVidas;
		}
		if (objectosTab.NaveEnemyTipo1[PosObjeto].vida == 0) {

			nVidas = objectosTab.NaveEnemyTipo1[PosObjeto].vida;

			LimpaPosTabuleiroTiro(objectosTab.NaveEnemyTipo1[PosObjeto].x, objectosTab.NaveEnemyTipo1[PosObjeto].y, bloco_vazio, LarguraNaveDefault);

			preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[PosObjeto].x, &objectosTab.NaveEnemyTipo1[PosObjeto].y, PosObjeto, bloco_vazio, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);

			return nVidas;
		}

		break;
	case NaveEsquiva:

		if (objectosTab.NaveEnemyTipo2[PosObjeto].vida > 0){
		
			objectosTab.NaveEnemyTipo2[PosObjeto].vida -= 1;
	
			nVidas = objectosTab.NaveEnemyTipo2[PosObjeto].vida;

			return nVidas;
		}
		if (objectosTab.NaveEnemyTipo2[PosObjeto].vida == 0) {
			
			nVidas = objectosTab.NaveEnemyTipo2[PosObjeto].vida;

			LimpaPosTabuleiroTiro(objectosTab.NaveEnemyTipo2[PosObjeto].x, objectosTab.NaveEnemyTipo2[PosObjeto].y, bloco_vazio, LarguraNaveDefault);

			preencheBlocosServidor(&objectosTab.NaveEnemyTipo2[PosObjeto].x, &objectosTab.NaveEnemyTipo2[PosObjeto].y, PosObjeto, bloco_vazio, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);
			
			return nVidas;
		}
		break;
	case NaveBoss:
		break;
	case NaveJogador:

		if (ArrayJogadores[PosObjeto].vidas > 0) {

			ArrayJogadores[PosObjeto].vidas -= 1;

			nVidas = objectosTab.NaveEnemyTipo2[PosObjeto].vida;

			return nVidas;
		}
		if (ArrayJogadores[PosObjeto].vidas == 0) {

			nVidas = objectosTab.NaveEnemyTipo2[PosObjeto].vida;

			LimpaPosTabuleiroTiro(ArrayJogadores[PosObjeto].posicao[0], ArrayJogadores[PosObjeto].posicao[1], bloco_vazio, LarguraNaveDefault);

			preencheBlocosServidor(&ArrayJogadores[PosObjeto].posicao[0], &ArrayJogadores[PosObjeto].posicao[1], PosObjeto, bloco_vazio, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);
			
			Packet Resposta;

			Resposta.tipo = user_exit;

			Resposta.Cliente_id = ArrayJogadores[PosObjeto].IdJogador;

			escrevebuffer(&Resposta, nomeServtoGw);

			ArrayJogadores = apagaClienteArrayJogadores(PosObjeto);
		}
		break;

	}
}
// vai alterar as posiçoes do objeto no respetivo array
void alteraPosicaoObjeto(int PosObjeto, int tipoObjeto, int *x, int *y) {
	switch (tipoObjeto) {

	case NaveJogador:
		ArrayJogadores[PosObjeto].posicao[0] = *x;
		ArrayJogadores[PosObjeto].posicao[1] = *y;
		break;
	case NaveEsquiva:
		objectosTab.NaveEnemyTipo2[PosObjeto].x = *x;
		objectosTab.NaveEnemyTipo2[PosObjeto].y = *y;
		break;
	case NaveBasica:
		objectosTab.NaveEnemyTipo1[PosObjeto].x = *x;
		objectosTab.NaveEnemyTipo1[PosObjeto].y = *y;
		break;
	case NaveBoss:
		objectosTab.NaveEnemyTipo3.x = *x;
		objectosTab.NaveEnemyTipo3.y = *y;
		break;
	case LancaTiro:
		AlteraPosicaoTiro(PosObjeto, x, y);
		break;
	}

}
void CarregaPosObjeto(int PosObjeto, int tipoObjeto, int *x, int *y) {

	switch (tipoObjeto) {

		case NaveJogador:
			*x = ArrayJogadores[PosObjeto].posicao[0];
			*y = ArrayJogadores[PosObjeto].posicao[1];
			break;
		case NaveEsquiva:
			*x = objectosTab.NaveEnemyTipo2[PosObjeto].x;
			*y = objectosTab.NaveEnemyTipo2[PosObjeto].y;
			break;
		case NaveBasica:
			*x = objectosTab.NaveEnemyTipo1[PosObjeto].x;
			*y = objectosTab.NaveEnemyTipo1[PosObjeto].y;
			break;
		case NaveBoss:
			*x = objectosTab.NaveEnemyTipo3.x;
			*y = objectosTab.NaveEnemyTipo3.y;
			break;
		case LancaTiro:
			break;
	}
}//
// Vai verificar os comandos digitados pelas naves todas do jogo
void verificaComandosJogo(int comando, int PosObjeto, int tipoObjeto) {

	int x, y;

	CarregaPosObjeto(PosObjeto, tipoObjeto, &x, &y);
	
	switch (comando)
	{
	case cima:
	
		if (VerificaPosicaoJogo(&x, &y, tipoObjeto, cima) == 1) {
	
			LimpaPosTabuleiro( x, y, bloco_vazio, LarguraNaveDefault);

			y -= 1;

			alteraPosicaoObjeto(PosObjeto, tipoObjeto, &x, &y);

			preencheBlocosServidor(&x, &y, PosObjeto, tipoObjeto, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);
		
		}
	
		break;

	case baixo:

		if (VerificaPosicaoJogo(&x, &y, tipoObjeto, baixo) == 1) {

			LimpaPosTabuleiro(x, y, bloco_vazio, LarguraNaveDefault);

			y += 1;

			alteraPosicaoObjeto(PosObjeto, tipoObjeto, &x, &y);

			preencheBlocosServidor(&x, &y, PosObjeto, tipoObjeto, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);

		}
	
		break;

	case esquerda:

		if (VerificaPosicaoJogo(&x, &y, tipoObjeto, esquerda) == 1) {

			LimpaPosTabuleiro(x, y, bloco_vazio, LarguraNaveDefault);

			x -= 1;

			alteraPosicaoObjeto(PosObjeto, tipoObjeto, &x, &y);

			preencheBlocosServidor(&x, &y, PosObjeto, tipoObjeto, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);

		}
	
		break;

	case direita:

		if (VerificaPosicaoJogo(&x, &y, tipoObjeto, direita) == 1) {

			LimpaPosTabuleiro(x, y, bloco_vazio, LarguraNaveDefault);

			x += 1;

			alteraPosicaoObjeto(PosObjeto, tipoObjeto, &x, &y);

			preencheBlocosServidor(&x, &y, PosObjeto, tipoObjeto, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);

		}
	
		break;

	case LancaTiro:
	
		AdicionaTiroArray(x, y, tipoObjeto, PosObjeto);
		
		break;

	case PowerUp1:
		break;

	case PowerUp2:
		break;

	case PowerUp3:
		break;

	}
}
int GestorNaveBoss(LPVOID aux) {

	Nave *naveInimigaTipo3 = (Nave*)aux;

	int nNaves = dadosServidor.initJogo.MaxNaveBoss;
	do {

		//algortimo movimento da nave

	} while (nNaves > 0);

	return 0;
}

int CalculaOpcaoNaveInimiga(int opcao_min, int opcao_max) {

	int opcao = 0;

	opcao = rand() % (opcao_max + 1 - opcao_min) + opcao_min;

	while (opcao < opcao_min  && opcao > opcao_max) {

		opcao = rand() % (opcao_max + 1 - opcao_min) + opcao_min;

	}
	return opcao;
}
// vai fazer a gestao de todas as naves inimigas
int GestorNaveEsquiva() {

	int nNaves = dadosServidor.initJogo.MaxNavesEsquivas;

	int opcao = 0; // opcao_min = cima, opcao_max = LancaTiro;

	WaitForSingleObject(dadosServidor.EventoIniciaJogo, INFINITE);
	
		do {
	
			for (int i = 0; i < nNaves; i++) {

				if (objectosTab.NaveEnemyTipo2[i].vida > 0) {

					opcao = CalculaOpcaoNaveInimiga(cima, direita);
				
					WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);
					
					verificaComandosJogo(opcao, i, NaveEsquiva);
				
					ReleaseMutex(dadosServidor.mutexTabuleiro);

					opcao = CalculaOpcaoNaveInimiga(LancaTiro, 100);
					
					if (opcao == LancaTiro) {

						WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);

						verificaComandosJogo(opcao, i, NaveEsquiva);

						ReleaseMutex(dadosServidor.mutexTabuleiro);
					}
				}
				Sleep(40); // alterar 
			}
		
		} while (nNaves > 0);

	return 0;
}
// vai fazer a gestao de todas as naves inimigas objectosTab.NaveEnemyTipo1[i]
void GestorNaveBasica() {//estou aqui aiai

	int flag = 0, ret = 0;

	int nNaves = dadosServidor.initJogo.MaxNavesBasicas;
	
	WaitForSingleObject(dadosServidor.EventoIniciaJogo,INFINITE);

	
	do {
	
		for (int i = 0; i < nNaves; i++) {
			WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);
			if (objectosTab.NaveEnemyTipo1[i].vida > 0) {
				
				//verifica se pode andar para a direita
					
					if (objectosTab.NaveEnemyTipo1[i].x < 38 && objectosTab.NaveEnemyTipo1[i].y < 36 && flag == 0) {// pode andar para a direita se estiver antes x=38 e y a cima de 36
																								  //verifica se pode andar para a direita
						ret = VerificaPosicaoJogo(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, NaveBasica, direita);

						if (ret) {
							//limpa a pos do tabuleiro
							LimpaPosTabuleiro(objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y, bloco_vazio, LarguraNaveDefault);

							//altera pos do objeto
							objectosTab.NaveEnemyTipo1[i].x += 1; //naveInimigaBasica[i].y;

														 //anda para a direita (coloca a pos no tabuleiro(a nave no sitio))
							preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, i, NaveBasica, LarguraNaveDefault);

							//se posicao da nave for == 38 a proxima fez anda para baixo
							SetEvent(dadosServidor.EventoAtualizaJogo);
						}
					}
					if (objectosTab.NaveEnemyTipo1[i].x == 38 && objectosTab.NaveEnemyTipo1[i].y < 36) {
						//verifica se pode andar para baixo 
						ret = VerificaPosicaoJogo(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, NaveBasica, baixo);

						flag = 1;//coloca a flag a 1
					}
					if (objectosTab.NaveEnemyTipo1[i].x > 0 && objectosTab.NaveEnemyTipo1[i].y < 36 && flag == 1) {
						//verifica se pode andar para a esquerda
						ret = VerificaPosicaoJogo(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, NaveBasica, esquerda);

						if (ret) {
							//limpa a pos do tabuleiro
							LimpaPosTabuleiro(objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y, bloco_vazio, LarguraNaveDefault);

							//altera pos do objeto
							objectosTab.NaveEnemyTipo1[i].x -= 1;//naveInimigaBasica[i].y;

														//anda para a esquerda (coloca a pos no tabuleiro(a nave no sitio))
							preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, i, NaveBasica, LarguraNaveDefault);
							SetEvent(dadosServidor.EventoAtualizaJogo);
							//se posicao da nave for == 0 a proxima fez anda para baixo
						}
					}
					if (objectosTab.NaveEnemyTipo1[i].x == 0 && objectosTab.NaveEnemyTipo1[i].y < 36) {
						//verifica se pode andar para baixo 
						ret = VerificaPosicaoJogo(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, NaveBasica, baixo);
						if (ret) {
							flag = 0;//coloca a flag a 0
						}
					}
					
					
			}
			ReleaseMutex(dadosServidor.mutexTabuleiro);
			Sleep(10);
		}
		
	} while (nNaves > 0); _tprintf(TEXT("\nsai no do\n"));

}
// funcao que calcula rand para posicionar as naves Esquivas
int CalculaPosRand(int pos_max) {

	int pos_min = 0;

	int pos = 0;

	pos = rand() % (pos_max + 1 - pos_min) + pos_min;

	while (pos < pos_min  && pos > pos_max) {

		pos = rand() % (pos_max + 1 - pos_min) + pos_min;

	}
	return pos;

}

//coloca naves Esquiva no tabuleiro 
void colocaNavesEsquiva() {

	srand(time(NULL));

	int totalNavesEsquiva = dadosServidor.initJogo.MaxNavesEsquivas;

	int contaEsquiva = 0;

	int x_min = 0, x_max = dimMapa_x - 2 , y_min = 0, y_max = dimMapa_y - 6;

	int x = 0, y = 0;

	WaitForSingleObject(dadosServidor.mutexTabuleiro, NULL);

	while (contaEsquiva < totalNavesEsquiva) {

		x = CalculaPosRand(x_max);

		y = CalculaPosRand(y_max);
	
			if (VerificaPosicaoPreencheTAb(&x, &y) == 1) {

				objectosTab.NaveEnemyTipo2[contaEsquiva].tipo = NaveEsquiva;

				objectosTab.NaveEnemyTipo2[contaEsquiva].x = x;
				
				objectosTab.NaveEnemyTipo2[contaEsquiva].y = y;

				objectosTab.NaveEnemyTipo2[contaEsquiva].vida = VidaNaveDefault;
				
				preencheBlocosServidor(&x, &y, contaEsquiva, NaveEsquiva,LarguraNaveDefault);
				
				contaEsquiva += 1;
				
			}
	}
	ReleaseMutex(dadosServidor.mutexTabuleiro);

}
//coloca naves Basicas no Tabuleiro
void colocaNavesBasicas() {

	int totalNavesBasicas = dadosServidor.initJogo.MaxNavesBasicas, totalNaves = dadosServidor.initJogo.MaxNavesBasicas;

	int contaBasica = 0, initPos = 0, endPos = 0, flag = 0;

	initPos = (dimMapa_x / 2) - (NavesPorLinha / 2);

	endPos = (dimMapa_x / 2) + (NavesPorLinha / 2);

	WaitForSingleObject(dadosServidor.mutexTabuleiro, NULL);

	for (int y = 0; y < dimMapa_y; y += 2) {

		for (int x = initPos; x < endPos; x += 2) {

			if (contaBasica < totalNavesBasicas) {

				objectosTab.NaveEnemyTipo1[contaBasica].tipo = NaveBasica;

				objectosTab.NaveEnemyTipo1[contaBasica].x = x;

				objectosTab.NaveEnemyTipo1[contaBasica].y = y;

				objectosTab.NaveEnemyTipo1[contaBasica].orientacao = direita;

				objectosTab.NaveEnemyTipo1[contaBasica].vida = VidaNaveDefault;
				
				preencheBlocosServidor(&x, &y, contaBasica, NaveBasica,LarguraNaveDefault);
				
				contaBasica += 1;		
			}
		}

		totalNaves -= 10;

		if (flag == 0 && totalNaves < NavesPorLinha / 2) {
		
			initPos = (dimMapa_x / 2) - (totalNaves / 2);
			
			endPos = (dimMapa_x / 2) + (totalNaves / 2);
			
			flag = 1;
		}
	}
	ReleaseMutex(dadosServidor.mutexTabuleiro);
}
//aloca memoria para as naves
Nave *criaArrayNaves(int tam) {

	Nave *aux;

	aux = (Nave*)malloc(sizeof(Nave)*tam);

	if (aux == NULL) {
		_tprintf(TEXT("Erro a alocar memoria para o array de naves enimigas\n"));
		exit(-1);
	}

	return aux;

}
//funcao que vai iniciar as naves no sistema;
int IniciaObjetosTabuleiro() {

	HANDLE hNavesEnemy[2];
	DWORD idNavesEnemy[2];

	HANDLE hNavesEnemyUlti;
	DWORD idNavesEnemyUlti;

	HANDLE hTirosTab;
	DWORD idTirosTab;
	

	objectosTab.NaveEnemyTipo1 = criaArrayNaves(dadosServidor.initJogo.MaxNavesBasicas);
	objectosTab.NaveEnemyTipo2 = criaArrayNaves(dadosServidor.initJogo.MaxNavesEsquivas);
	

	_tprintf(TEXT("\nVou prencher o tabuleiro do servidor\n\n"));
	
	limpaTabuleiro(); //ver os dois

	ColocaNavesTab(); //ver os dois

	hTirosTab = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorTirosTab, (LPVOID)NULL, 0, &idTirosTab);	//thread para tiros no tabuleiro

	hNavesEnemy[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNaveBasica, (LPVOID)NULL, 0, &idNavesEnemy[0]);	//thread para naves Basicas
	
	hNavesEnemy[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNaveEsquiva, (LPVOID)NULL, 0, &idNavesEnemy[1]);	//thread para naves Esquivas

	if (hNavesEnemy[0] == NULL || hNavesEnemy[1] == NULL || hTirosTab == NULL) {

		_tprintf(TEXT("ERRO ao lançar naves enimigas ou Tiros\n"));
		
		return -1;
	}

	WaitForMultipleObjects(NULL, hNavesEnemy, 0, INFINITE);//so depois de nao haver mais naves a nave boss entra em ação
														   //TODO fazer função para colocar a utima nave no array
	hNavesEnemyUlti = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNaveBoss, (LPVOID)NULL, 0, &idNavesEnemyUlti);	// cria thread para nave Boss
	
	if (hNavesEnemyUlti == NULL) {

		_tprintf(TEXT("ERRO ao lançar Thread para naves enimigas\n"));

		return -1;
	}

	WaitForSingleObject(hNavesEnemyUlti, INFINITE);

	WaitForSingleObject(hTirosTab, INFINITE);



	return 0;
}
int * CalculaPosJogador(int pos_min, int pos_max) {

	int *pos = 0;

	*pos = rand() % (pos_max + 1 - pos_min) + pos_min;

	while (*pos < pos_min  && *pos > pos_max) {

		*pos = rand() % (pos_max + 1 - pos_min) + pos_min;

	}
	return pos;

}
//funcao que irá iniciar o jogo apos um jogador seleccionar a opçao jogar
void IniciaAmbienteJogo(int pos) {

	int *x = &ArrayJogadores[pos].posicao [0], *y = &ArrayJogadores[pos].posicao[1];

		///POR AQUI O EVENTO PARA A MOVIMENTAÇÃO DAS NAVES
		WaitForSingleObject(dadosServidor.mutexTabuleiro,INFINITE);

		IniciarJogo(x,y,pos);

		ReleaseMutex(dadosServidor.mutexTabuleiro);

}//
//func que lista os clientes
void mostraClinoArray() {

	for (int i = 0; i < dadosServidor.NumCliNoArray; i++) {

		_tprintf(TEXT("Cliente n:%d --- Nome %s \n"), (i + 1), ArrayJogadores[i].nome);
	}
}
//Verifica se o Cliente ja existe no array de jogadores
int verificaPlayerNoArray(TCHAR *nome) {

	//TODO mutex
	for (int i = 0; i < dadosServidor.NumCliNoArray; i++) {

		if (wcscmp(ArrayJogadores[i].nome, nome) == 0) {
			//TODO fim mutex
			return 1; //TODO encontrou 1
		}
	}
	//TODO fim mutex
	return 0;
}
//recebe o pacote filtra a informação e coloca na pos certa do array
void ColocaCliArray(packet *aux, int pos) {

	ArrayJogadores[pos].IdJogador = aux->Cliente_id;

	ArrayJogadores[pos].pontuacao = aux->pontuacao;

	wcscpy_s(ArrayJogadores[pos].nome, aux->dataPacket.nome);

	dadosServidor.NumCliNoArray += 1;

}
//aloca espaço para um novo cliente e coloca dentro do array
void alocaColocaPlayerNoArray(packet *aux) {

	jogadorinfo *arrayAux;

	arrayAux = (jogadorinfo*)malloc(sizeof(jogadorinfo) * (dadosServidor.NumCliNoArray + 1));

	if (arrayAux == NULL) {

		_tprintf(TEXT("Erro a criar um novo espaço no array\n"));
		exit(-1);
	}
	//mutex TODO
																// copiar o conteudo de um array para o outro
	for (int i = 0; i < dadosServidor.NumCliNoArray; i++) {
		arrayAux[i] = ArrayJogadores[i];							//passar do original para o auxiliar menos o ultimo cliente
	}
	//Agora o nosso array já tem mais uma posicao
	ArrayJogadores = arrayAux;
	//O numero de jogadores no array é incrementado nesta função
	ColocaCliArray(aux, dadosServidor.NumCliNoArray);
}
//Funcao que vai tratar o Login de um determinado Cliente
packet trataPacoteLogin(packet *aux) {

	packet resposta;
	//se for o primeiro cliente ->aloca->coloca
	if (dadosServidor.NumCliNoArray == 0) {

		ColocaCliArray(aux, dadosServidor.NumCliNoArray);

		resposta.tipo = user_login_sucesso;
	}
	else {
		//se não for vai ver se já existem algum cliente no o mesmo nome
		if (verificaPlayerNoArray(aux->dataPacket.nome)) {

			resposta.tipo = user_exit;
			resposta.dataPacket.comando = user_Login_falhou;

		}
		else {

			if (dadosServidor.NumCliNoArray < dadosServidor.NumMaxClientes) {

				alocaColocaPlayerNoArray(aux);

				resposta.tipo = user_login_sucesso;
			}
			else {

				resposta.tipo = user_exit;
				resposta.dataPacket.comando = user_login_Limite_clientes;

			}
		}
	}
	wcscpy_s(resposta.dataPacket.nome, aux->dataPacket.nome);

	return resposta;
}
int VerificaPosicaoJogador(Packet *aux) {

	for (int i = 0; i < dadosServidor.NumMaxClientes; i++) {
		if (ArrayJogadores[i].IdJogador == aux->Cliente_id)
			return i;
	}
}
// Funcao que vai fazer o tratamento de pacotes
void TrataPacotesGwtoServ() {

	packet *aux, resposta;

	int PosJogador;

	while (dadosServidor.ServidorUp == 1) {
		
		aux = LerBufferGwtoSer();

		switch (aux->tipo) {

		case user_login:

			resposta = trataPacoteLogin(aux);			// trata pacote de login

			resposta.Cliente_id = aux->Cliente_id;

			escrevebuffer(&resposta, nomeServtoGw);

			break;

		case IniciaJogoMultiplayer:
			// talvez necessite de um mutex para o array de clientes
			if (dadosServidor.NumCliNoArray == 1 && dadosServidor.estadoJogo == 0) {

				dadosServidor.estadoJogo = 1;

				PosJogador = VerificaPosicaoJogador(aux);

				IniciaAmbienteJogo(PosJogador);

				SetEvent(dadosServidor.EventoIniciaJogo);

				SetEvent(dadosServidor.EventoAtualizaJogo);
			}
			else {

				PosJogador = VerificaPosicaoJogador(aux);

				IniciaAmbienteJogo(PosJogador);

				SetEvent(dadosServidor.EventoAtualizaJogo);


			}

			break;

		case AtualizacaoJogo:

			PosJogador = VerificaPosicaoJogador(aux);

			WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);

			verificaComandosJogo(aux->dataPacket.comando, PosJogador, NaveJogador);

			ReleaseMutex(dadosServidor.mutexTabuleiro);

			break;

		case user_exit:

			PosJogador = VerificaPosicaoJogador(aux);

			WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);

			preencheBlocosServidor(&ArrayJogadores[PosJogador].posicao[0], &ArrayJogadores[PosJogador].posicao[1], PosJogador, bloco_vazio, LarguraNaveDefault);

			SetEvent(dadosServidor.EventoAtualizaJogo);

			ReleaseMutex(dadosServidor.mutexTabuleiro);

			ArrayJogadores = apagaClienteArrayJogadores(PosJogador);

			resposta.tipo = user_exit;

			resposta.dataPacket.comando = user_logout;

			escrevebuffer(&resposta, nomeServtoGw);
	
			break;
		}
	}
}
// Inicia o array do Cliente
jogadorinfo * iniciaArrayCli(){ //Alocação do Array
	
	jogadorinfo *Aux;

	Aux = (jogadorinfo*)malloc(sizeof(jogadorinfo));

	if (Aux == NULL) {
		_tprintf(TEXT("Erro a Inicializar o Array de Jogadores\n"));
		return NULL;
	}
	return Aux;

}

// inicializa sincronizaçao usada para executar funçoes no servidor
void IniciaSincronizacaoServidor() {

	dadosServidor.EventoIniciaJogo = CreateEvent(NULL, TRUE, FALSE, EventIniciajogo);
	
	if (dadosServidor.EventoIniciaJogo == NULL) {
		
		exit(-1);

	}

	dadosServidor.mutexTabuleiro = CreateMutex(NULL, FALSE, mutexAtualizaBloco);

	if (dadosServidor.mutexTabuleiro == NULL) {

		exit(-1);
	}

	dadosServidor.EventoAtualizaJogo = CreateEvent(NULL, TRUE, FALSE, enviaTabClientes);

	if (dadosServidor.EventoAtualizaJogo == NULL) {

		exit(-1);

	}
}
// Funcao vai atualizar a informaçao inicial do jogo
void AtualizaInformacaoInicialJogo() {

	ArrayJogadores = NULL;
	dadosServidor.NumMaxClientes = nMaxJogadores; //nMaxPlay
	dadosServidor.ServidorUp = 1;
	dadosServidor.estadoJogo = 0;
	dadosServidor.NumCliNoArray = 0;
	//dadosServidor.initJogo.MaxNavesBasicas = ninimigas1;//comentar aqui grafica
	//dadosServidor.initJogo.MaxNavesEsquivas = ninimigas2;//comentar aqui grafica
	dadosServidor.initJogo.MaxNaveBoss = 1;

}
// inicia os servi�os e a configura�ao do Servidor;
int IniciarServidor() {

	AtualizaInformacaoInicialJogo();

	CriaMemoriaPartilhada();															// cria os Buffers na memoria partilhada e sincronizacao

	IniciaSincronizacaoServidor();

	IniciaDadosTiros(dadosServidor.mutexTabuleiro, dadosServidor.EventoAtualizaJogo);

	IniciaSincronizacaoTiros();

	ArrayJogadores = iniciaArrayCli();
																				
	dadosServidor.hThreadSerToGw = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)TrataPacotesGwtoServ,(LPVOID) NULL,0,&dadosServidor.IdThreadSertoGw); // fica a espera de pacotes no buffer
	
	IniciaObjetosTabuleiro();
	//
	//criaStatusServerRegistry (0 );
	return 1;
}
/*
int _tmain(int argc, LPTSTR argv[]) {


#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	

	IniciarServidor();

	Sleep(190000);
	return 0;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void carregaBitMaps() {

	Wallpaper = (HBITMAP)LoadImage(NULL, L"../../Imagens/imagemInicialServidor.bmp", IMAGE_BITMAP, 810, 845, LR_LOADFROMFILE);

}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	PAINTSTRUCT Ps;

	switch (uMsg) {

	case WM_DESTROY: {

		DeleteObject(Wallpaper);
		criaStatusServerRegistry(0);

		exit(0);
	}
	case WM_PAINT: {

		janelaImprime = BeginPaint(hwnd, &Ps);
		BitBlt(janelaAux, 0, 0, 810, 850, HdcWallpaper, 0, 0, SRCCOPY);
		//copia do janela auxiliar para a janela a imprimir
		BitBlt(janelaImprime, 0, 0, 810, 850, janelaAux, 0, 0, SRCCOPY);

		EndPaint(hwnd, &Ps);

		break;
	}

	case WM_CREATE: {
		
		
		//prepara janela aux 
		janelaImprime = GetDC(hwndPrincipal);
		janelaAux = CreateCompatibleDC(janelaImprime);
		HBITMAP HjanelaImprime = CreateCompatibleBitmap(janelaImprime, 810, 850);
		SelectObject(janelaAux, HjanelaImprime);
		ReleaseDC(hwndPrincipal, janelaImprime);

		// Cria uma "janela"( memory device compatible ) para cada bipmap
		HdcWallpaper = CreateCompatibleDC(janelaAux);
		SelectObject(HdcWallpaper, Wallpaper);
		
		break;
	}


	case WM_CHAR: {

		
		if ((TCHAR)wParam == 's') {
			criaStatusServerRegistry(1);
			hIniciaJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IniciarServidor, (LPVOID)NULL, 0, &idIniciaJogo);	//thread para tiros no tabuleiro
			break;
		}
		if ((TCHAR)wParam == 't') {
			criaStatusServerRegistry(0);
			exit(0);
			break;
		}

		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK DBOXServidor(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//colocar o texto por default nas caixas
		SetWindowText(GetDlgItem(hwnd, IDC_NBASICAS), TEXT("38"));
		SetWindowText(GetDlgItem(hwnd, IDC_VIDASBASICAS), TEXT("1"));
		SetWindowText(GetDlgItem(hwnd, IDC_SPEEDBASICAS), TEXT("50"));
		SetWindowText(GetDlgItem(hwnd, IDC_NESQUIVAS), TEXT("10"));
		SetWindowText(GetDlgItem(hwnd, IDC_VIDASESQUIVAS), TEXT("1"));
		
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			BOOL bSuccess;
			BOOL bSigned = FALSE;
	
			dadosServidor.initJogo.MaxNavesBasicas = GetDlgItemInt(hwnd, IDC_NBASICAS, &bSuccess, bSigned); 
			dadosServidor.initJogo.MaxNavesEsquivas = GetDlgItemInt(hwnd, IDC_NESQUIVAS, &bSuccess, bSigned);
			
			
			EndDialog(hwnd, 0);//fechar a janela 
		
			ShowWindow(hwndPrincipal, SW_SHOWDEFAULT);
			UpdateWindow(hwndPrincipal);
			
			
			break;
		}
		break;
		case IDCANCEL:

			exit(0);

			break;
		}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}


	return 0;
}



int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR LpCmdLine, int ncmdshow)
{
	
	//Registar a class da janela
	const wchar_t CLASS_NAME[] = L"Servidor";

	WNDCLASS wc = {};

	///////////////////////////////////////Configuraçoes//////////////////////////////////////////////////////////////
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_SERVIDOR), 0, DBOXServidor);
	ShowWindow(hDlg, ncmdshow);
	MSG msg = {};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//carregabitmaps para a esturuta
	carregaBitMaps();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = (HICON)LoadImage(NULL, L"../../Imagens/Logot.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	wc.hCursor = (HCURSOR)LoadImage(NULL, L"../../Imagens/Cursor.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	RegisterClass(&wc);

	// Criação da janela principal
	hwndPrincipal = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L" -> Servidor <- ",      // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

										// Size and position
		0, 0, 810, 850,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwndPrincipal == NULL)
	{
		return 0;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}