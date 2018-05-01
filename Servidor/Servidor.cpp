#include "Servidor.h"

// funcao dada nas aulas para posicionar o cursor num determinado sitio;
void gotoxy(int x, int y) {
	static HANDLE hStdout = NULL;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	if (!hStdout)
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}
// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigas(LPVOID navesInimigas) {

	Nave *naveInimiga;

	naveInimiga = ((Nave*)navesInimigas);

	do {

		Sleep(10000);
		naveInimiga->vida--;

	} while (naveInimiga->vida > 0);

	return 0;
}
// verificacao se as naves estao na posicaçao correta
void verificaPosicaoNaves(Nave *naves) {

	for (int i = 0; i < ninimigas; i++) {
		_tprintf(TEXT("%d,%d\n"), i, naves[i].x, naves[i].y);
	}
}
// vai preparar o Ambiente do Jogo
int InicioJogo( int NumNavesInvasoras) {

	Nave *navesInimigas;

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
		navesInimigas[i].NaveInvasoras = CreateThread(NULL,
													0,      
													(LPTHREAD_START_ROUTINE)GestorNavesInimigas,
													(LPVOID)&navesInimigas[i],
													0,
													&navesInimigas[i].NaveInvthreadId);

		if (navesInimigas[i].NaveInvasoras == NULL) {

			_tprintf(TEXT("\n\nErro ao Iniciar Thread para Naves Inimigas\n\n"));
			
			return -1;
		
		}
		
		_tprintf(TEXT("\nCriei Thread %d"), i);

	}
	verificaPosicaoNaves(navesInimigas);
	return 0;
}
// vai lançar a Thread que vai ficar a comunicar com o Gateway
/*int comunicaoGateway(LPVOID partilha vai ficar aqui o contacto com a DLL) {
int comunicaoGateway(LPVOID partilha vai ficar aqui o contacto com a DLL) {

	MemoriaPartilhada *Partilha;

	Partilha = (MemoriaPartilhada *)partilha;

	return 0;
}*/
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
// inicia os serviços e a configuraçao do Servidor;
int IniciarServidor() {

	TCHAR c;
	
	gestao_servidor optionServidor;

	_tprintf(TEXT("\n\n Inicializaçao do Servidor\n\n"));

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

		optionServidor.initJogo.MaxNavesInimigas = ninimigas;

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
	Sleep(50000);
	return 0;
}