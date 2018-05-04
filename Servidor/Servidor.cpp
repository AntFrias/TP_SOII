#include "Servidor.h"
#include "../AcessoMemDLL/stdafx.h"

#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int * global_int;
BufferMsg * auxbuff;

dataServer *dadosServidor;
synBuffer sync;


int CriaSyncMemoria() {



	_tprintf(TEXT("\nVAI CRIAR O SEMAPHORE :  %s"), semGwLer);



	sync.SemGwtoServComItem = CreateSemaphore(NULL, 0, Buffer_size, semGwLer);
	if (sync.SemGwtoServComItem == NULL) {
		_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwLer);
		return -1;
	}



	_tprintf(TEXT("\nVAI CRIAR O SEMAPHORE :  %s"), semGwEscrever);

	sync.SemGwtoServSemItem = CreateSemaphore(NULL, Buffer_size, Buffer_size, semGwEscrever);
	if (sync.SemGwtoServSemItem == NULL) {
		_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwEscrever);
		return -1;
	}

	InitializeCriticalSection(&sync.MutexGwtoSer);

	return 0;
}


void CriaMapViewer(){


}

// cria Buffer na memoria partilhada 
ptrbufferMsg CriaShareBuffer(HANDLE hBuffer2, LPCTSTR nomeBuffer) {

	 ptrbufferMsg auxBuffer;
	 HANDLE hBuffer;
	 //criar a memoria partilhada 
	 hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer );

	 if (hBuffer == NULL) {
		 _tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
	 }
	 else if (GetLastError()== ERROR_ALREADY_EXISTS){
		 _tprintf(TEXT("\nEste buffer -> %s ja foi criado\n"), nomeBuffer);
		 
	 }
	 // mapear a memoria para a mensagem
	 dadosServidor->comGwtoSer = (ptrbufferMsg)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));

	 if (dadosServidor->comGwtoSer == NULL) {

		 _tprintf(TEXT("Erro a mapear Buffer de mensagem"));
	 }
	 
	// dadosServidor->comGwtoSer->tail = 0;

	 dadosServidor->comGwtoSer->head = Buffer_size;

		 return NULL;

}
// funcao que vai Pedir para criar os buffers  na memoria partilhada
int criaMemoriaPartilhada(ptrbufferMsg *stg, ptrbufferMsg *gts) {

	
	//*stg = CriaShareBuffer(&stg, nomeSrtoGW);

	*gts = CriaShareBuffer(&gts, nomeGwtoSr);

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

		naveInimiga->vida--;
		
	} while (naveInimiga->vida > 0);

	return 0;
}
// vai preparar o Ambiente do Jogo
// fazer um array de HANDLES das threads DAS NAVES INIMIGAS ---------------------------------------------> waitformultipleobject
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

void MostraNome(TCHAR nome[10]) {

	_tprintf(TEXT("\n\nMensagem recebida: %s"), nome);

}
// Funcao que vai fazer o tratamento de pacotes
void TrataPacoteLido(Packet *PacoteaTratar) {
	/*
	switch (PacoteaTratar->tipo) {

	case 1:
		//MostraNome(PacoteaTratar->dataPacket.nome);

	}*/
}
// Funcao que vai ficar a ler os pacotes do Buffer --> GwtoSer
void LeituraPacotesBuffer() {
	_tprintf(TEXT("\n\n\nLeituraPacotesBuffer\n\n"));
	 Packet PacoteLido;

	/*int head = dadosServidor->comGwtoSer->head; //aqui change
	int tail = dadosServidor->comGwtoSer->tail;*/

	//EnterCriticalSection(&sync.MutexGwtoSer);

			//PacoteLido = dadosServidor->comGwtoSer->array[head];
			_tprintf(TEXT(" head = %d    \n"), auxbuff->head);
			//TrataPacoteLido(&PacoteLido);
	
			//*out = *out == Buffer_size - 1 ? 0 : *out += 1;
			
			/*
			dadosServidor->comGwtoSer->head = ++dadosServidor->comGwtoSer->head % Buffer_size;

			//QUEUE is empty now
			if (dadosServidor->comGwtoSer->head == dadosServidor->comGwtoSer->tail)
				dadosServidor->comGwtoSer->head = Buffer_size;*/
	
	//LeaveCriticalSection(&sync.MutexGwtoSer); 
}
// funcao que vai estar a ler do Buffer GwtoSer
void LerBufferGwtoSer() {
	_tprintf(TEXT("LerBufferGwtoSer\n\n"));
	int count = 0.; 

		while (1) {
			
			_tprintf(TEXT("\n\n\n WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);  %d \n\n"), ++count);
			WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);

			_tprintf(TEXT("\n O global_int = %s \n"), auxbuff->array[0].dataPacket.nome);

			ReleaseSemaphore(sync.SemGwtoServSemItem, 1 , NULL);
			//Sleep(1000);
		}
}
// inicia os servi�os e a configura�ao do Servidor;
int IniciarServidor() {

	_tprintf(TEXT("\nconta: %d \n"), sum(1, 1)); 

	dadosServidor->ServidorUp = 1;

	bufferMsg *auxSertoGw;
	bufferMsg *auxGwtoSer;

	//auxSertoGw = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	//auxGwtoSer = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	TCHAR c;
	
	_tprintf(TEXT("\n\n Inicializa�ao do Servidor\n\n"));

	criaStatusServerRegistry( 1 );										// cria parametro no Registry para mostrar que o servidor est� 
	//criaMemoriaPartilhada(&auxSertoGw, &auxGwtoSer);						// cria os Buffers na memoria partilhada
	/*dadosServidor->comSertoGw = auxSertoGw;								// APonta os buffers par os pontos na estrutura Gestao do Servidor
	dadosServidor->comGwtoSer = auxGwtoSer;*/

	//_tprintf(TEXT("\n\n%d"), dadosServidor->comGwtoSer->tail);
	//_tprintf(TEXT("\n\n%d"), dadosServidor->comGwtoSer->head);

	CriaSyncMemoria();													// cria a syncroniza�ao que ser� usada nos Buffers
																		// inicia a thread que ir� tratar os pedidos enviados pelo GW
	dadosServidor->hThreadSerToGw = CreateThread( NULL,
												 0,
												(LPTHREAD_START_ROUTINE)LerBufferGwtoSer,
												(LPVOID) NULL,
												0,
												&dadosServidor->IdThreadSertoGw);
											

		_tprintf(TEXT(" 2.iniciar jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {

		dadosServidor->initJogo.MaxNavesInimigas = ninimigas;

		IniciaNavesInimigas(dadosServidor->initJogo.MaxNavesInimigas);
	}
	
	criaStatusServerRegistry (0 );
	return 0;
}

void Alocacoes() {
	dadosServidor = (dataServer*)malloc(sizeof(dataServer));
}


bufferMsg * createSharedINT(HANDLE hMemory, LPCTSTR shareMemName)
{
	bufferMsg * ptrInt;

	//shared memory				 
	hMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), shareMemName);

	if (hMemory == NULL) {
		_tprintf(TEXT("\t[ERROR] Unable to create MsgSystem file mapping (%d)\n"), GetLastError());
		return NULL;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		_tprintf(TEXT("\t[OK] MessageSystem already exists (%d)\n"), GetLastError());
	}

	// Map the memory
	ptrInt = (bufferMsg *)MapViewOfFile(hMemory, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));

	if (ptrInt == NULL)
	{
		_tprintf(TEXT("\t[ERROR] Unable to create shared MsgSystem (%d)\n"), GetLastError());
		return NULL;
	}

	return ptrInt;
}





int _tmain(int argc, LPTSTR argv[]) {


#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	

	_tprintf(TEXT("side of packet %d "), sizeof(Packet));
	Alocacoes();

	HANDLE HtoIntSHR = NULL;
	auxbuff = createSharedINT(HtoIntSHR, TEXT("SHRED_INT"));
	auxbuff->head = 0;

	IniciarServidor();
	Sleep(190000);
	return 0;
}