#include "Servidor.h"

// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigas(LPVOID navesInimigas) {

	Nave *naveInimiga;

	naveInimiga = ((Nave*) navesInimigas);

	do {

		Sleep(10000);
		naveInimiga->vida--;
		
	} while (naveInimiga->vida > 0);

	return 0;
}
// vai preparar o Ambiente do Jogo
int InicioJogo( int NumNavesInvasoras) {

	Nave *navesInimigas;

	NavesInimigas = (navesInvasoras*)malloc(sizeof(Naves) * NumNavesInvasoras);

	if (NavesInimigas == NULL) {

		_tprintf(TEXT("\nErro ao criar Threads para Naves Inimigas\n"));

		return -1;
	}

	for (int i = 0; i < NumNavesInvasoras; i++) {
		NavesInimigas[i].vida = 5;
		NavesInimigas[i].NavesInvasoras = CreateThread(NULL,
													0,      
													(LPTHREAD_START_ROUTINE)GestorNavesInimigas,
													(LPVOID)&NavesInimigas[i],
													0,
													&NavesInimigas[i].NaveInvthreadId);

		if (NavesInimigas[i].NavesInv == NULL) {

			_tprintf(TEXT("\n\nErro ao Iniciar Thread para Naves Inimigas\n\n"));
			
			return -1;
		
		}

		_tprintf(TEXT("\nCriei Thread %d\n"), i);

	}

}
// vai lançar a Thread que vai ficar a comunicar com o Gateway

//int comunicaoGateway(LPVOID partilha /*vai ficar aqui o contacto com a DLL*/) {

	//MemoriaPartilhada *Partilha;

//	Partilha = (MemoriaPartilhada *)partilha;
	
	//return 0;
//}


// inicia os serviços e a configuraçao do Servidor no registry;
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
// inicia serviços no servidor 
int IniciarServidor() {

	TCHAR c;
	
	gestao_servidor optionServidor;

	_tprintf(TEXT("\n\nOlá Eu sou o Servidor e estou a iniciar\n\n"));

	_tprintf(TEXT("\n\n Iniciar Thread Para Tratar os pedidos do Gateway\n\n"));

	criaStatusServerRegistry( 1 );
/*
	optionServidor.ComGateway = CreateThread( NULL,
												 0,
												(LPTHREAD_START_ROUTINE)comunicaoGateway,
												(LPVOID)&Partilha,
												0,
												&optionServidor.ComGatewaythreadId);
	*/											

		_tprintf(TEXT(" 2.iniciar jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {

		optionServidor.initJogo.MaxNavesInimigas = 10;

		InicioJogo(optionServidor.initJogo.MaxNavesInimigas);
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

	return 0;
}