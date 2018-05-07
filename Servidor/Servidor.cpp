#include "Servidor.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataServer dadosServidor;


// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigas(LPVOID navesInimigas) {

	Nave *naveInimiga;

	naveInimiga = ((Nave*)navesInimigas);

	do {

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

		navesInimigas[i].vida = 5;
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

void MostraNome(TCHAR *nome) {

	_tprintf(TEXT("\n\nMensagem recebida: %s"), nome);

}
// Funcao que vai fazer o tratamento de pacotes


void TrataPacotesGwtoServ() {

	Packet *aux;

	while (1) {
		//Sleep(1000);//para debug
		aux = LerBufferGwtoSer();
	
		switch (aux->tipo) {

		case 1:
			MostraNome(aux->dataPacket.nome);

		}
	}

}

// inicia os servi�os e a configura�ao do Servidor;
int IniciarServidor() {

	dadosServidor.ServidorUp = 1;
	
	TCHAR c;
	
	_tprintf(TEXT("\n\n Inicializacao do Servidor\n\n"));

	criaStatusServerRegistry( 1 );														// cria parametro no Registry para mostrar que o servidor est� 

	CriaMemoriaPartilhada();															// cria os Buffers na memoria partilhada
	

	CriaSyncMemoria();																	// cria a syncroniza�ao que ser� usada nos Buffers
	
																						
	dadosServidor.hThreadSerToGw = CreateThread( NULL,0,(LPTHREAD_START_ROUTINE)TrataPacotesGwtoServ,(LPVOID) NULL,0,&dadosServidor.IdThreadSertoGw);
											

		_tprintf(TEXT(" 2.iniciar Naves inimigas para jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {

		dadosServidor.initJogo.MaxNavesInimigas = ninimigas;

		IniciaNavesInimigas(dadosServidor.initJogo.MaxNavesInimigas);
	}
	
	criaStatusServerRegistry (0 );
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