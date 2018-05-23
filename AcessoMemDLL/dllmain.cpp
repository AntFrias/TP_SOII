// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
//criar detallhes para as funçoes
#define AcessoMemDLL


BOOL APIENTRY DllMain( HMODULE hModule,
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
//aqui podemos fazer a funções que declaramos no .h
extern "C" {

	//Nome do Buffer Server to Gateway
TCHAR nomeServtoGW[] = TEXT("Buffer SrToGw");
TCHAR nomeGwtoServ[] = TEXT("Buffer GwToSr");
TCHAR nomeMapa[] = TEXT("Mapa");


// Nome dos Semaforos para comunicacao nos Buffers de mensagens
TCHAR semGwLer[] = TEXT("semaforoGwLer");
TCHAR semGwEscrever[] = TEXT("semaforoGwEscrever");
TCHAR mutexGwtoSer[] = TEXT("mutexGwtoSer");

TCHAR semSerLer[] = TEXT("semaforoSerLer");
TCHAR semSerEscrever[] = TEXT("semaforoSerEscrever");
TCHAR mutexSertoGw[] = TEXT("mutexSertoGw");

	synBuffer sync;

	bufferMsg *comGwtoServ;
	packet *comServtoGw;
	MaPa *Mapa;
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// Cria Sincronizaçao de dos Buffers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int CriaSyncMemoria() {

		sync.SemGwtoServComItem = CreateSemaphore(NULL, 0, Buffer_size, semGwLer);
		if (sync.SemGwtoServComItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo %s\n"), semGwLer);
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

		//InitializeCriticalSection(&sync.MutexGwtoSer);
		return 0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// cria Buffer na memoria partilhada 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void criaMapeamentoMemoriaMapa(LPCTSTR nomeMapa) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MaPa), nomeMapa);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar Mapa %s na memoria Partilhada "), nomeMapa);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste Mapa -> %s ja foi criado\n"), nomeMapa);

		}
		// mapear a memoria para a mensagem
		Mapa = (MaPa *)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(Bloco)*(dimMapa_x*dimMapa_y));

		if (Mapa == NULL) {

			_tprintf(TEXT("Erro a mapear Mapa"));
		}
	}
	void criaMapeamentoMemoriaServToGw(LPCTSTR nomeBuffer) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(packet), nomeBuffer);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar slot %s na memoria Partilhada "), nomeBuffer);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste slot -> %s ja foi criado\n"), nomeBuffer);

		}
		// mapear a memoria para a mensagem
		comServtoGw = (packet*)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(packet));

		if (comServtoGw == NULL) {

			_tprintf(TEXT("Erro a mapear slot de mensagem"));
		}

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
		criaMapeamentoMemoriaServToGw(nomeServtoGW);
		criaMapeamentoMemoriaMapa(nomeMapa);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// Funçoes para escrever no buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// funcao que posiciona no buffer onde irá escrever o pacote
	void escreveNoBuffer(Packet *pacoteParaEscrita) {

		int in;

		WaitForSingleObject(sync.MutexGwtoSer,INFINITE);

		in = comGwtoServ->tail;

		if (comGwtoServ->head == Buffer_size)
			comGwtoServ->head = comGwtoServ->tail;
		/////////////////////////////////PARA_FINS_DE_DEBUG///////////////////////////////////////////////////////////////
		//_tprintf(TEXT("Escrevi %s na pos %d"), comGwtoServ->array[comGwtoServ->tail].dataPacket.nome, comGwtoServ->tail);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		comGwtoServ->tail = ++comGwtoServ->tail % Buffer_size;

		comGwtoServ->array[in] = *pacoteParaEscrita;

		ReleaseMutex(sync.MutexGwtoSer);

		wcscpy_s(comGwtoServ->array[in].dataPacket.nome, pacoteParaEscrita->dataPacket.nome);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes que preenche o pacote do buffer
	void escrevebufferGwToSr(Packet *aux) {

		while (1) {

			aux->tipo = 1;

			_fgetts(aux->dataPacket.nome, 10, stdin);

			WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);

			escreveNoBuffer(aux);

			ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);

		} 

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para ler pacotes do buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Packet * LerBufferGwtoSer() {

		Packet *aux;

		int out;

			WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);

			WaitForSingleObject(sync.MutexGwtoSer, INFINITE);

			out = comGwtoServ->head; //aux para libertar mais cedo

			comGwtoServ->head = ++comGwtoServ->head % Buffer_size;

			if (comGwtoServ->head == comGwtoServ->tail) {
				comGwtoServ->head = Buffer_size;
			}

			
			/////////////////////////////////PARA_FINS_DE_DEBUG///////////////////////////////////////////////////////////////
			//_tprintf(TEXT("Escrevi %s na pos %d"), comGwtoServ->array[comGwtoServ->head].dataPacket.nome, comGwtoServ->head);
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			aux = &comGwtoServ->array[out];// depois de ja ter a posiçao e libertado o mutex "escreve"
			
			ReleaseMutex(sync.MutexGwtoSer);

			ReleaseSemaphore(sync.SemGwtoServSemItem, 1, NULL);
	
	return aux;
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
}


