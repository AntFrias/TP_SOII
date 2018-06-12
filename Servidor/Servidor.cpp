#include "Servidor.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")


dataServer dadosServidor;

DadosdoJogo objectosNoMapa;

jogadorinfo *ArrayJogadores;

BlocoServ blocoServ[dimMapa_x][dimMapa_y];

void mostraTabuleiro() {

	for (int x = 0; x < dimMapa_x; x++) {
		for (int y = 0; y < dimMapa_y; y++) {

			_tprintf(TEXT("%d"), blocoServ[x][y].tipo); //tirei o espaco

		}
		_tprintf(TEXT("\n"));
	}

}
//func que lista os clientes
void mostraClinoArray() {

	for (int i = 0; i < dadosServidor.NumCliNoArray; i++) {

		_tprintf(TEXT("Cliente n:%d --- Nome %s \n"), (i + 1), ArrayJogadores[i].nome);
	}
}
// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigasTipo3(LPVOID aux) {

	Nave *naveInimigaTipo3 = (Nave*)aux;

	int nNaves = dadosServidor.initJogo.MaxNavesInimigas3;

	int coord_x = CoordWindow_x, coord_y = CoordWindow_y;

	do {

		//algortimo movimento da nave




	} while (nNaves > 0);



	return 0;
}
// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigasTipo2(LPVOID aux) {

	Nave *naveInimigaTipo2 = (Nave*)aux;

	int nNaves = dadosServidor.initJogo.MaxNavesInimigas2;

	int coord_x = CoordWindow_x, coord_y = CoordWindow_y;

	do {

		//algortimo movimento da nave




	} while (nNaves > 0);
	return 0;
}
// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigasTipo1(LPVOID aux) {

	Nave *naveInimigaTipo1 = (Nave*)aux;

	int nNaves = dadosServidor.initJogo.MaxNavesInimigas1;
	
	int coord_x = CoordWindow_x, coord_y = CoordWindow_y;

	do {

		//algortimo movimento da nave

		
		

	} while (nNaves > 0);

	return 0;
}
//aloca memoria para as naves
Nave *criaArrayNaves(int tam) {

	Nave *aux;

	aux =(Nave*) malloc(sizeof(Nave)*tam);

	if (aux == NULL) {
		_tprintf(TEXT("Erro a alocar memoria para o array de naves enimigas\n"));
		exit(-1);
	}

	return aux;

}
void preencheBlocosServidor(int x, int y, int pos) {

	for (int i = x; i < x + LarguraNaveDefault; i++) {

		for (int j = y; j < y + LarguraNaveDefault; j++) {
			
			blocoServ[i][j].tipo = NaveBasica;

			blocoServ[i][j].posArray = pos;

			escreveBufferTabuleiro(i, j, blocoServ[i][j].tipo);
		}
	}
}
int CalculaNavesPorLinhas() {

	int nNavesLinha = dadosServidor.initJogo.MaxNavesInimigas1;
	
	if (nNavesLinha > dimMapa_y / 2){
		for (int i = 1; i < dadosServidor.initJogo.MaxNavesInimigas1; i++) {
			nNavesLinha = nNavesLinha / i;

			if (nNavesLinha < dimMapa_y / 2 && nNavesLinha % i == 0) {
				return nNavesLinha = dimMapa_y / 2 - nNavesLinha;
			}
		}
	}
	else {
		nNavesLinha = (dimMapa_y) - (nNavesLinha );
		return nNavesLinha;
	}
}

void colocaNavesTab() {

	int nMaxNavesTipo1 = dadosServidor.initJogo.MaxNavesInimigas1;

	int nMaxNavesTipo2 = dadosServidor.initJogo.MaxNavesInimigas2;
	
	int contaTipo1 = 0, contaTipo2 = 0;
	
	int initPos = CalculaNavesPorLinhas() / 2;

	int endPos = CalculaNavesPorLinhas() + ( CalculaNavesPorLinhas() / 2);
	

	_tprintf(TEXT("Vou prencher o tabuleiro do servidor\n"));


		WaitForSingleObject(dadosServidor.mutexTabuleiro,NULL);

		for (int x = 0; x < dimMapa_x - 2; x += 2) {

			for (int y = initPos; y < endPos; y += 2) {

			if ( contaTipo1 < nMaxNavesTipo1){
					
					objectosNoMapa.NaveEnemyTipo1[contaTipo1].x = x;

					objectosNoMapa.NaveEnemyTipo1[contaTipo1].y = y;

					preencheBlocosServidor(x, y, contaTipo1);

					contaTipo1 += 1;
				}
			}
		}
		
		ReleaseMutex(dadosServidor.mutexTabuleiro);
		
		mostraTabuleiro();
		_tprintf(TEXT("\n\n\n\n\n"));
		mostraTabCom();

}
void limpaTabuleiro() {

	for (int x = 0; x < dimMapa_x; x++) {
		for (int y = 0; y < dimMapa_y; y++) {
			
			blocoServ[x][y].id = 0; //não existe
			blocoServ[x][y].tipo = bloco_vazio; //vazio
			blocoServ[x][y].posArray = 0; //vazio
			escreveBufferTabuleiro(x, y, bloco_vazio);
		}
	}
}
// fazer um array de HANDLES das threads DAS NAVES INIMIGAS 
int IniciaNavesInimigas() {

	Nave *navesInimigas;
	
	HANDLE hNavesEnemy[2];
	DWORD idNavesEnemy[2];

	HANDLE hNavesEnemyUlti;
	DWORD idNavesEnemyUlti;


	//int coord_x = CoordWindow_x, coord_y = CoordWindow_y;

		objectosNoMapa.NaveEnemyTipo1 = criaArrayNaves(dadosServidor.initJogo.MaxNavesInimigas1);
		objectosNoMapa.NaveEnemyTipo2 = criaArrayNaves(dadosServidor.initJogo.MaxNavesInimigas2);
		objectosNoMapa.NaveEnemyTipo3 = criaArrayNaves(dadosServidor.initJogo.MaxNavesInimigas3);
		
		limpaTabuleiro(); //ver os dois
	
		colocaNavesTab(); //ver os dois

		hNavesEnemy[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNavesInimigasTipo1, (LPVOID)NULL, 0, &idNavesEnemy[0]);
		hNavesEnemy[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNavesInimigasTipo2, (LPVOID)NULL, 0, &idNavesEnemy[1]);
		
		if (hNavesEnemy[0] == NULL || hNavesEnemy[1] == NULL) {
			_tprintf(TEXT("ERRO ao lançar naves enimigas\n"));
			return -1;
		}

		WaitForMultipleObjects(NULL,hNavesEnemy,0,INFINITE);//so depois de nao haver mais naves a nave boss entra em ação
		//TODO fazer função para colocar a utima nave no array
		hNavesEnemyUlti = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GestorNavesInimigasTipo3, (LPVOID)NULL, 0, &idNavesEnemyUlti);

		if (hNavesEnemyUlti == NULL) {
			_tprintf(TEXT("ERRO ao lançar naves enimigas\n"));
			return -1;
		}

		return 0;
}

//Verifica se o Cliente ja existe no array de jogadores
int verificaPlayerNoArray(TCHAR *nome) {

	//TODO mutex
	for (int i = 0; i < dadosServidor.NumCliNoArray; i++) {

		if (wcscmp(ArrayJogadores[i].nome,nome)==0) {
			//TODO fim mutex
			return 1; //TODO encontrou 1
		}
	}
	//TODO fim mutex
	return 0;
}
//recebe o pacote filtra a informação e coloca na pos certa do array
void ColocaCliArray(packet *aux,int pos) {

	ArrayJogadores[pos].IdJogador = aux->Cliente_id;

	ArrayJogadores[pos].pontuacao = aux->pontuacao;

	wcscpy_s(ArrayJogadores[pos].nome, aux->dataPacket.nome); 
	
	dadosServidor.NumCliNoArray += 1;

}

//aloca espaço para um novo cliente e coloca dentro do array
void alocaColocaPlayerNoArray(packet *aux) {

	jogadorinfo *arrayAux;

	arrayAux = (jogadorinfo*) malloc(sizeof(jogadorinfo) * (dadosServidor.NumCliNoArray + 1));

	if (arrayAux == NULL) {
		
		_tprintf(TEXT("Erro a criar um novo espaço no array\n"));
		exit(-1);
	}
	//mutex TODO
																// copiar o conteudo de um array para o outro
	for (int i = 0; i <dadosServidor.NumCliNoArray; i++) {
		arrayAux[i] = ArrayJogadores[i];							//passar do original para o auxiliar menos o ultimo cliente
	}
																	//Agora o nosso array já tem mais uma posicao
	ArrayJogadores = arrayAux;
																	//O numero de jogadores no array é incrementado nesta função
	ColocaCliArray(aux,dadosServidor.NumCliNoArray);
}
//Funcao que irá iniciar o jogo e as respetivas movimentacoes
void IniciarJogo() {

}
//funcao que irá iniciar o jogo apos um jogador seleccionar a opçao jogar
void IniciaAmbienteJogo(int njogadores) {

	if (njogadores == IniciaJogoIndividual)
		IniciarJogo();

}
//Funcao que vai tratar o Login de um determinado Cliente
packet trataPacoteLogin(packet *aux){

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
// Funcao que vai fazer o tratamento de pacotes
void TrataPacotesGwtoServ() {

	packet *aux, resposta;

	while (dadosServidor.ServidorUp == 1) {
		
		aux = LerBufferGwtoSer();

		switch (aux->tipo) {

		case user_login: 

			resposta = trataPacoteLogin(aux);			// trata pacote de login

			resposta.Cliente_id = aux->Cliente_id;

		case IniciaJogoIndividual:

			IniciaAmbienteJogo(IniciaJogoIndividual);

		case IniciaJogoMultiplayer:
			IniciaAmbienteJogo(IniciaJogoMultiplayer);

		}
	
		escrevebuffer(&resposta, nomeServtoGw);
	}
}
jogadorinfo * iniciaArrayCli(){ //Alocação do Array
	
	jogadorinfo *Aux;

	Aux = (jogadorinfo*)malloc(sizeof(jogadorinfo));

	if (Aux == NULL) {
		_tprintf(TEXT("Erro a Inicializar o Array de Jogadores\n"));
		return NULL;
	}
	return Aux;

}
// inicia os servi�os e a configura�ao do Servidor;
int IniciarServidor() {

	TCHAR c;
	ArrayJogadores = NULL;
	dadosServidor.NumMaxClientes = nMaxJogadores; //nMaxPlay
	dadosServidor.ServidorUp = 1;
	dadosServidor.NumCliNoArray = 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	dadosServidor.initJogo.MaxNavesInimigas1 = ninimigas1;
	dadosServidor.initJogo.MaxNavesInimigas2 = ninimigas2;
	dadosServidor.initJogo.MaxNavesInimigas3 = 1;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	dadosServidor.mutexTabuleiro = CreateMutex(NULL, FALSE, mutexAtualizaBloco);


	CriaMemoriaPartilhada();															// cria os Buffers na memoria partilhada e sincronizacao

	ArrayJogadores = iniciaArrayCli();
																				
	dadosServidor.hThreadSerToGw = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)TrataPacotesGwtoServ,(LPVOID) NULL,0,&dadosServidor.IdThreadSertoGw);
											
		_tprintf(TEXT(" \n\n\n\n\n2.iniciar Naves inimigas para jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {
		IniciaNavesInimigas();
	
	}
	
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