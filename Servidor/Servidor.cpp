#include "Servidor.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

dataServer dadosServidor;

void CriaMapViewer(){


}

// cria Buffer na memoria partilhada 
ptrbufferMsg CriaShareBuffer(HANDLE hBuffer, LPCTSTR nomeBuffer) {

	 ptrbufferMsg auxBuffer;

	 //criar a memoria partilhada 
	 hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer );
	 if (hBuffer == NULL) {
		 _tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
	 }
	 else if (GetLastError()== ERROR_ALREADY_EXISTS){
		 _tprintf(TEXT("\nEste buffer ja foi criado\n"));
	 }
	 // mapear a memoria para a mensagem
	 auxBuffer = (ptrbufferMsg)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));
	 if (auxBuffer == NULL) {
		 _tprintf(TEXT("Erro a mapear Buffer de mensagem"));
	 }
	 
	 auxBuffer->in = Buffer_size;
	 auxBuffer->out = 0;

		 return auxBuffer;

}
// funcao que vai Pedir para criar os buffers  na memoria partilhada
int criaMemoriaPartilhada(ptrbufferMsg stg, ptrbufferMsg gts) {

	//ptrbufferMsg stg, gts;
	
	stg = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	if (stg == NULL) {

		_tprintf(TEXT(" Erro a alocar Buffer de Mensagem"));
		
		return -1;
	}
	gts = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	if (gts == NULL) {

		_tprintf(TEXT("Erro a alocar Buffer de Mensagem"));
		
		return -1;
	}

	stg = CriaShareBuffer(stg, nomeSrtoGW);
	gts = CriaShareBuffer(gts, nomeGwtoSr);

	return 0;
}
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

		Sleep(1000);
		naveInimiga->vida--;
		
	} while (naveInimiga->vida > 0);
	_tprintf(TEXT(" MORRI  %d"), GetCurrentThreadId());
	return 0;
}
// verificacao se as naves estao na posicaçao correta
void verificaPosicaoNaves(Nave *naves) {

	for (int i = 0; i < ninimigas; i++) {
		//_tprintf(TEXT("%d,%d\n"), i, naves[i].x, naves[i].y);_______________________________________________
	}
}
// vai preparar o Ambiente do Jogo
// fazer um array de HANDLES das threads DAS NAVES INIMIGAS ---------------------------------------------> waitformultipleobject
int InicioJogo( int NumNavesInvasoras) {

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
		ArrayHandleNavesInim[i] = navesInimigas[i].NaveInvasoras;
		_tprintf(TEXT("\nCriei Thread %d"), i);
		
	}
	verificaPosicaoNaves(navesInimigas);
	WaitForMultipleObjects(ninimigas, ArrayHandleNavesInim, TRUE, INFINITY);
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

	bufferMsg *auxSertoGw;
	bufferMsg *auxGwtoSer;

	auxSertoGw = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	auxGwtoSer = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	TCHAR c;
	
	_tprintf(TEXT("\n\n Inicializaçao do Servidor\n\n"));

	criaStatusServerRegistry( 1 );
	
	criaMemoriaPartilhada(auxSertoGw, auxGwtoSer);
	dadosServidor.comSertoGw = auxSertoGw;
	dadosServidor.comGwtoSer = auxGwtoSer;
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

		dadosServidor.initJogo.MaxNavesInimigas = ninimigas;

		InicioJogo(dadosServidor.initJogo.MaxNavesInimigas);
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
	Sleep(90000);
	return 0;
}