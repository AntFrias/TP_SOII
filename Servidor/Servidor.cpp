#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")


//Nome para os mecanismos de sincronizaçao do servidor
TCHAR EventIniciajogo[] = TEXT("EventoIniciaJogo");


jogadorinfo *ArrayJogadores;

DadosdoJogo objectosTab;

dataServer dadosServidor;

void mostraNaveBasica() {

	for (int i = 0; i < dadosServidor.initJogo.MaxNavesBasicas; i++) {

		_tprintf(TEXT("\npos da nave [%d][%d]"), objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y);  

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
		objectosTab.NaveEnemyTipo3[PosObjeto].x = *x;
		objectosTab.NaveEnemyTipo3[PosObjeto].y = *y;
		break;
	case tiroJogador:
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
			*x = objectosTab.NaveEnemyTipo3[PosObjeto].x;
			*y = objectosTab.NaveEnemyTipo3[PosObjeto].y;
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

		AdicionaTiroArray(PosObjeto, ArrayJogadores[PosObjeto].posicao[0], ArrayJogadores[PosObjeto].posicao[1], dadosServidor.mutexTabuleiro, dadosServidor.EventoAtualizaJogo);
		
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
// vai fazer a gestao de todas as naves inimigas
int GestorNaveEsquiva() {

	int nNaves = dadosServidor.initJogo.MaxNavesEsquivas;

	int opcao = 0, opcao_min = cima, opcao_max = LancaTiro;

	do {

		WaitForSingleObject(dadosServidor.EventoIniciaJogo, INFINITE);
	
		for (int i = 0; i < nNaves; i++) {

			if (objectosTab.NaveEnemyTipo2[i].vida > 0) {
				
				opcao = rand() % (opcao_max + 1 - opcao_min) + opcao_min;

				while (opcao < opcao_min  && opcao > opcao_max) {

					opcao = rand() % (opcao_max + 1 - opcao_min) + opcao_min;

				}
				WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);
					
				verificaComandosJogo(opcao, i, NaveEsquiva);
				
				ReleaseMutex(dadosServidor.mutexTabuleiro);
			}
			Sleep(1000); // alterar 
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

			if (objectosTab.NaveEnemyTipo1[i].vida > 0) {
				
				//verifica se pode andar para a direita
					WaitForSingleObject(dadosServidor.mutexTabuleiro, INFINITE);
					objectosTab.NaveEnemyTipo1[i].orientacao = VerificaPosicaoJogo(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, NaveBasica, objectosTab.NaveEnemyTipo1[i].orientacao);
				
					switch (objectosTab.NaveEnemyTipo1[i].orientacao) {
						case direita: {
							//limpa a pos do tabuleiro
							LimpaPosTabuleiro(objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y, bloco_vazio, LarguraNaveDefault);
							//altera pos do objeto
							objectosTab.NaveEnemyTipo1[i].x += 1;
							//anda para a direita (coloca a pos no tabuleiro(a nave no sitio))
							preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, i, NaveBasica, LarguraNaveDefault);

							break;

						}
						case baixo: {
							//limpa a pos do tabuleiro
							LimpaPosTabuleiro(objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y, bloco_vazio, LarguraNaveDefault);
							//altera pos do objeto
							objectosTab.NaveEnemyTipo1[i].y += 2;
							//anda para a direita (coloca a pos no tabuleiro(a nave no sitio))
							preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, i, NaveBasica, LarguraNaveDefault);

							break;

						}
						case esquerda: {
							//limpa a pos do tabuleiro
							LimpaPosTabuleiro(objectosTab.NaveEnemyTipo1[i].x, objectosTab.NaveEnemyTipo1[i].y, bloco_vazio, LarguraNaveDefault);
							//altera pos do objeto
							objectosTab.NaveEnemyTipo1[i].x -= 1;
							//anda para a direita (coloca a pos no tabuleiro(a nave no sitio))
							preencheBlocosServidor(&objectosTab.NaveEnemyTipo1[i].x, &objectosTab.NaveEnemyTipo1[i].y, i, NaveBasica, LarguraNaveDefault);

							break;

						}
					}
					ReleaseMutex(dadosServidor.mutexTabuleiro);
			}
			SetEvent(dadosServidor.EventoAtualizaJogo);
			Sleep(1000);
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

				contaEsquiva += 1;
				
				preencheBlocosServidor(&x, &y, contaEsquiva, NaveEsquiva,LarguraNaveDefault);
	
				contaEsquiva++;
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
	objectosTab.NaveEnemyTipo3 = criaArrayNaves(dadosServidor.initJogo.MaxNaveBoss);

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

			resposta.tipo = user_Login_falhou;

		}
		else {

			if (dadosServidor.NumCliNoArray < dadosServidor.NumMaxClientes) {

				alocaColocaPlayerNoArray(aux);

				resposta.tipo = user_login_sucesso;
			}
			else {

				resposta.tipo = user_login_Limite_clientes;

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

	dadosServidor.EventoIniciaJogo = CreateEvent(NULL, 0, 0, EventIniciajogo);
	
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
	dadosServidor.initJogo.MaxNavesBasicas = ninimigas1;
	dadosServidor.initJogo.MaxNavesEsquivas = ninimigas2;
	dadosServidor.initJogo.MaxNaveBoss = 1;

}
// inicia os servi�os e a configura�ao do Servidor;
int IniciarServidor() {

	AtualizaInformacaoInicialJogo();

	CriaMemoriaPartilhada();															// cria os Buffers na memoria partilhada e sincronizacao

	IniciaSincronizacaoServidor();

	IniciaDadosTiros();

	IniciaSincronizacaoTiros();

	InicializaArrayTiros();

	ArrayJogadores = iniciaArrayCli();
																				
	dadosServidor.hThreadSerToGw = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)TrataPacotesGwtoServ,(LPVOID) NULL,0,&dadosServidor.IdThreadSertoGw); // fica a espera de pacotes no buffer
	
	IniciaObjetosTabuleiro();
	//
	//criaStatusServerRegistry (0 );
	return 0;
}
int _tmain(int argc, LPTSTR argv[]) {


#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	

	IniciarServidor();

	Sleep(190000);
	return 0;
}















// inicia os servi�os e a configura�ao do Servidor no registry;
/*int criaStatusServerRegistry(int n) {

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



//naves enimigas cod algoritmo
gotoxy(naveInimiga->x, naveInimiga->y);

//if (navePodeMexer(naveInimiga)==1) {

if (coord_x < CoordWindow_x + dimMapa_x) {
	gotoxy(coord_x, coord_y);
	_tprintf(TEXT("I"));
	naveInimiga->x = coord_x;
	naveInimiga->x = coord_y;
	coord_x++;

}
else {
	coord_x = CoordWindow_x;
	naveInimiga->x = coord_x;
	coord_y++;
	naveInimiga->x = coord_y;

	gotoxy(coord_x, coord_y);
	_tprintf(TEXT("I"));
}
naveInimiga->vida--;



*/


//verifica se o cliente é repetido 