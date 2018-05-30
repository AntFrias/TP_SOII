#include "Servidor.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")


dataServer dadosServidor;
jogadorinfo *ArrayJogadores;

// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigas(LPVOID navesInimigas) {

	Nave *naveInimiga;
	int coord_x = CoordWindow_x, coord_y = CoordWindow_y;
	naveInimiga = ((Nave*)navesInimigas);

	do {
		
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

	} while (naveInimiga->vida > 0);

	return 0;
}
// vai preparar o Ambiente do Jogo
// fazer um array de HANDLES das threads DAS NAVES INIMIGAS 
int IniciaNavesInimigas( int NumNavesInvasoras) {

	Nave *navesInimigas;
	
	HANDLE *ArrayHandleNavesInim;

	ArrayHandleNavesInim = (HANDLE*)malloc(sizeof(HANDLE)*ninimigas);

	if (ArrayHandleNavesInim == NULL) {
		_tprintf(TEXT("\n Erro ao criar o array de Handles das Naves inimigas"));
	}

	int coord_x = CoordWindow_x, coord_y = CoordWindow_y;
	
	navesInimigas = (Nave*)malloc(sizeof(Nave) * NumNavesInvasoras);

	if (navesInimigas == NULL) {

		_tprintf(TEXT("\nErro ao criar Threads para Naves Inimigas\n"));

		return -1;
	}
	for (int i = 0; i < NumNavesInvasoras; i++) {

		navesInimigas[i].vida = 200;
		if (coord_x < CoordWindow_x + dimMapa_x) {
			gotoxy(coord_x, coord_y);
			_tprintf(TEXT("I"));
			navesInimigas[i].x = coord_x;
			navesInimigas[i].x = coord_y;
			coord_x++;

		}
		else {
			coord_x = CoordWindow_x;
			navesInimigas[i].x = coord_x;
			coord_y++;
			navesInimigas[i].x = coord_y;

			gotoxy(coord_x, coord_y);
			_tprintf(TEXT("I"));
		}
		navesInimigas[i].NaveInvasoras = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)GestorNavesInimigas,(LPVOID)&navesInimigas[i],0,&navesInimigas[i].NaveInvthreadId);

		if (navesInimigas[i].NaveInvasoras == NULL) {

			_tprintf(TEXT("\n\nErro ao Iniciar Thread para Naves Inimigas\n\n"));
			
			return -1;
		
		}
		ArrayHandleNavesInim[i] = navesInimigas[i].NaveInvasoras;
		_tprintf(TEXT("\nCriei Thread %d"), i);
		
	}
	WaitForMultipleObjects(ninimigas, ArrayHandleNavesInim, TRUE, INFINITY);
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
*/
//Mostra o nome de um cliente
void MostraNome(TCHAR *nome) {

	_tprintf(TEXT("Cliente: %s\n"), nome);

}
//verifica se o cliente é repetido 
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
//Pedido de Login
packet trataPacoteTipo1(packet *aux){

	packet resposta;
															//se for o primeiro cliente ->aloca->coloca
	if (dadosServidor.NumCliNoArray == 0) {
		ColocaCliArray(aux, dadosServidor.NumCliNoArray);
		resposta.tipo = user_login_sucesso;
	}
															//se não for vai ver se já existem algum cliente no o mesmo nome
	if (verificaPlayerNoArray(aux->dataPacket.nome)) {

		resposta.tipo = user_Login_falhou;
															//envia mensagem a dizer que este player já existe
	}
	else
	{
		if (dadosServidor.NumCliNoArray < dadosServidor.NumMaxClientes) {
			alocaColocaPlayerNoArray(aux);
			resposta.tipo = user_login_sucesso;
		}
		else {
			_tprintf(TEXT("\n\n Falta completar aqui a dizer que o Buffer está cheio e por a funçao de escrecer a resposta"));
			resposta.tipo = user_login_Limite_clientes;
			
			//envia mensagem a dizer que está tudo ocupado
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
		_tprintf(TEXT("\n\nLi um pacote do jogador %s com o ID %d"), aux->dataPacket.nome, aux->Cliente_id);
		switch (aux->tipo) {

		case user_login: //TIPO -> 1 -> LOGIN

			resposta = trataPacoteTipo1(aux); 
		}
		_tprintf(TEXT("\nVou escrever uma resposta para jogador %s com o ID %d"), aux->dataPacket.nome, aux->Cliente_id);
		//escrevebuffer(&resposta, nomeServtoGw);
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
	dadosServidor.NumMaxClientes = 5;
	dadosServidor.ServidorUp = 1;
	dadosServidor.NumCliNoArray = 0;
	
	ArrayJogadores = iniciaArrayCli();
	
	//criaStatusServerRegistry( 1 );														// cria parametro no Registry para mostrar que o servidor est� 

	CriaMemoriaPartilhada();															// cria os Buffers na memoria partilhada

	CriaSyncMemoriaGwtoServ();						// cria a syncroniza�ao que ser� usada nos Buffers	
	
	CriaSyncMemoriaServtoGw();
																				
	dadosServidor.hThreadSerToGw = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)TrataPacotesGwtoServ,(LPVOID) NULL,0,&dadosServidor.IdThreadSertoGw);
											
		_tprintf(TEXT(" \n\n\n\n\n2.iniciar Naves inimigas para jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {

		dadosServidor.initJogo.MaxNavesInimigas = ninimigas;

		IniciaNavesInimigas(dadosServidor.initJogo.MaxNavesInimigas);
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