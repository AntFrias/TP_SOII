// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
//criar detallhes para as funçoes
#define AcessoMemDLL

//
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
////aqui podemos fazer a funções que declaramos no .h   
extern "C" {

	//Nome do Buffer Server to Gateway
	TCHAR nomeServtoGw[] = TEXT("Buffer SrToGw");
	TCHAR nomeGwtoServ[] = TEXT("Buffer GwToSr");
	TCHAR nomeTabuleiro[] = TEXT("Tabuleiro");

	// Nome dos Semaforos para comunicacao nos Buffers de mensagens
	TCHAR semServLer[] = TEXT("semaforoServLer");
	TCHAR semGwEscrever[] = TEXT("semaforoGwEscrever");
	TCHAR mutexGwtoSer[] = TEXT("mutexGwtoSer");


	TCHAR semGwLer[] = TEXT("semaforoGwLer");
	TCHAR SemServEscrever[] = TEXT("semaforoServEscrever");
	TCHAR mutexServtoGw[] = TEXT("mutexServtoGw");

	TCHAR mutexAtualizaBloco[] = TEXT("mutexEscreverBloco");
	TCHAR enviaTabClientes[] = TEXT("EventoEnviaTabClientes");

	synBuffer sync;			// estao guardados todos os mecanismos de sincronizaçao entre Gateway e Servidor

	bufferMsg *comGwtoServ, *comServtoGw;		// estao guardados os Buffers de Co municaçao entre Gateway e Servidor

	TabMemPartilhada *comTabuleiro;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// cria Buffers na memoria partilhada  e sua sincronizacao
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int criaSyncMemoriaTabuleiro() {

		sync.MutexAtualizaTab = CreateMutex(NULL, FALSE, mutexAtualizaBloco);
		if (sync.MutexAtualizaTab == NULL) {
			_tprintf(TEXT("Erro ao criar MUTEX: %s\n"), mutexAtualizaBloco);
			return -1;
		}
		return 0;
	}
	int CriaSyncMemoriaServtoGw() {

		sync.SemServtoGwComItem = CreateSemaphore(NULL, 0, Buffer_size, semGwLer );
		if (sync.SemServtoGwComItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo %s\n"), semGwLer);
			return -1;
		}

		sync.SemServtoGwSemItem = CreateSemaphore(NULL, Buffer_size, Buffer_size, SemServEscrever);
		if (sync.SemServtoGwSemItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo: %s\n"), SemServEscrever);
			return -1;
		}


		sync.MutexServtoGw = CreateMutex(NULL, FALSE, mutexServtoGw);
		if (sync.MutexServtoGw == NULL) {
			_tprintf(TEXT("Erro ao criar MUTEX: %s\n"), mutexServtoGw);
			return -1;
		}

		return 0;
	}
	int CriaSyncMemoriaGwtoServ() {

		sync.SemGwtoServComItem = CreateSemaphore(NULL, 0, Buffer_size, semServLer);
		if (sync.SemGwtoServComItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo %s\n"), semServLer);
			return -1;
		}

		sync.SemGwtoServSemItem = CreateSemaphore(NULL, Buffer_size, Buffer_size, semGwEscrever);
		if (sync.SemGwtoServSemItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo: %s\n"), semGwEscrever);
			return -1;
		}


		sync.MutexGwtoSer = CreateMutex(NULL, FALSE, mutexGwtoSer);
		if (sync.MutexGwtoSer == NULL) {
			_tprintf(TEXT("Erro ao criar MUTEX: %s\n"), mutexGwtoSer);
			return -1;
		}

		return 0;
	}
	void criaMapeamentoMemoriaTabuleiro(LPCTSTR nomeBuffer) {//TODO apagar isto meter no server

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TabMemPartilhada), nomeBuffer);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar comTabuleiro %s na memoria Partilhada "), nomeBuffer);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste comTabuleiro -> %s ja foi criado\n"), nomeBuffer);

		}
		// mapear a memoria para a mensagem
		comTabuleiro = (TabMemPartilhada *)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(Bloco)*(dimMapa_x*dimMapa_y));

		if (comTabuleiro == NULL) {

			_tprintf(TEXT("Erro a mapear comTabuleiro"));
		}
	}
	void criaMapeamentoMemoriaServToGw(LPCTSTR nomeBuffer) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
			
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste buffer -> %s ja foi criado\n"), nomeBuffer);

		}
		// mapear a memoria para a mensagem
		comServtoGw = (bufferMsg *)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));

		if (comServtoGw == NULL) {

			_tprintf(TEXT("Erro a mapear Buffer de mensagem"));
			
		}

		comServtoGw->tail = 0;

		comServtoGw->head = Buffer_size;

	}
	void criaMapeamentoMemoriaGwToServ(LPCTSTR nomeBuffer) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste buffer -> %s ja foi criado\n"), nomeBuffer);

		}
		// mapear a memoria para a mensagem
		comGwtoServ = (bufferMsg *)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));

		if (comGwtoServ == NULL) {

			_tprintf(TEXT("Erro a mapear Buffer de mensagem"));
		}

		comGwtoServ->tail = 0;

		comGwtoServ->head = Buffer_size;

	}
	void CriaMemoriaPartilhada() {

		criaMapeamentoMemoriaGwToServ(nomeGwtoServ);
		criaMapeamentoMemoriaServToGw(nomeServtoGw);
		criaMapeamentoMemoriaTabuleiro(nomeTabuleiro);

		CriaSyncMemoriaGwtoServ();
		CriaSyncMemoriaServtoGw();
		criaSyncMemoriaTabuleiro();

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para escrever nos buffers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void escreveBufferTabuleiro(int x, int y, int tipoObjeto, int flag) {

		comTabuleiro->blocos[y][x].Flag = flag; 
		comTabuleiro->blocos[y][x].tipo = tipoObjeto;
		
		comTabuleiro->alteracoes = 1;

	}
	void escreveNoBufferServtoGw(Packet *pacoteParaEscrita) {

		WaitForSingleObject(sync.MutexServtoGw, INFINITE);//mudar aqui

		comServtoGw->array[comServtoGw->tail] = *pacoteParaEscrita;
		
		wcscpy_s(comServtoGw->array[comServtoGw->tail].dataPacket.nome, pacoteParaEscrita->dataPacket.nome);  // isto deve ser retirado daqui

		if (comServtoGw->head == Buffer_size) {

			comServtoGw->head = comServtoGw->tail;
		}

		comServtoGw->tail = ++comServtoGw->tail % Buffer_size;

		ReleaseMutex(sync.MutexServtoGw);

	}
	void escreveNoBufferGwtoServ(Packet *pacoteParaEscrita) {

		WaitForSingleObject(sync.MutexGwtoSer, INFINITE);//mudar aqui

		comGwtoServ->array[comGwtoServ->tail] = *pacoteParaEscrita;

		wcscpy_s(comGwtoServ->array[comGwtoServ->tail].dataPacket.nome, pacoteParaEscrita->dataPacket.nome);

		if (comGwtoServ->head == Buffer_size) {   

			comGwtoServ->head = comGwtoServ->tail;	
		}

		comGwtoServ->tail = ++comGwtoServ->tail % Buffer_size;

		ReleaseMutex(sync.MutexGwtoSer);

	}
	void escrevebuffer(packet *aux, TCHAR *nomeBuffer) {
		
		

		if (_tcscmp(nomeBuffer, nomeGwtoServ) == 0) {

			WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);

			escreveNoBufferGwtoServ(aux);

			ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);

		} else {

			WaitForSingleObject(sync.SemServtoGwSemItem, INFINITE);

			escreveNoBufferServtoGw(aux);

			ReleaseSemaphore(sync.SemServtoGwComItem, 1, NULL);

		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para ler pacotes dos buffer ServtoGw
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Packet * LerBufferServtoGw() {

		Packet * aux;

		
		WaitForSingleObject(sync.SemServtoGwComItem, INFINITE);
		
		WaitForSingleObject(sync.MutexServtoGw, INFINITE);
		
		aux = &comServtoGw->array[comServtoGw->head];
		
		comServtoGw->head = ++comServtoGw->head % Buffer_size;

		if (comServtoGw->head == comServtoGw->tail) {

			comServtoGw->head = Buffer_size;
		}

		ReleaseMutex(sync.MutexServtoGw);

		ReleaseSemaphore(sync.SemServtoGwSemItem, 1, NULL);

		return aux;
	}
	Packet * LerBufferGwtoSer() {

		Packet *aux;

		WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);

		WaitForSingleObject(sync.MutexGwtoSer, INFINITE);
		_tprintf(TEXT("Cliente %d  %s \n"),comGwtoServ->array[comGwtoServ->head].Cliente_id, comGwtoServ->array[comGwtoServ->head].dataPacket.nome);
		aux = &comGwtoServ->array[comGwtoServ->head];

		comGwtoServ->head = ++comGwtoServ->head % Buffer_size;

		if (comGwtoServ->head == comGwtoServ->tail) {

			comGwtoServ->head = Buffer_size;
		}
		ReleaseMutex(sync.MutexGwtoSer);

		ReleaseSemaphore(sync.SemGwtoServSemItem, 1, NULL);

		return aux;
	}
	Packet * LerBufferTabuleiro() {

		Packet aux;

		comTabuleiro->items = 0;

		aux.tipo = AtualizacaoJogo;
	
		if ( comTabuleiro->alteracoes == 1){

			WaitForSingleObject(sync.MutexAtualizaTab, INFINITE);

				for (int i = 0; i < dimMapa_y; i++) {

					for (int j = 0; j < dimMapa_x; j++) {

						if (comTabuleiro->blocos[i][j].Flag == 1) {

							aux.dataPacket.arrayTab[comTabuleiro->items].tipo = comTabuleiro->blocos[i][j].tipo;
							
							aux.dataPacket.arrayTab[comTabuleiro->items].x = j;

							aux.dataPacket.arrayTab[comTabuleiro->items].y = i;
	
							comTabuleiro->items += 1;
						}
					}
				}
			aux.numObjetos = comTabuleiro->items;

			comTabuleiro->items = 0;

			comTabuleiro->alteracoes = 0;
				
			ReleaseMutex(sync.MutexAtualizaTab);
	
		return &aux;
		}
		
	return NULL;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//mosta alteraçoes no tabuleiro 
	void mostraTabCom() {

		for (int x = 0; x < dimMapa_x; x++) {
			for (int y = 0; y < dimMapa_y; y++) {

				_tprintf(TEXT("%d"), comTabuleiro->blocos[x][y].tipo);

			}
			_tprintf(TEXT("\n"));
		}

	}
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

	void ativaAlteracoesTab() {

		comTabuleiro->alteracoes = 1;
	}
}


