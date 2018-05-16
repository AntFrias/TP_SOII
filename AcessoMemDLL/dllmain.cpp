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

	ptrbufferMsg comGwtoServ;
	packet *comServtoGw;
	MaPa Mapa[dimMapa_x][dimMapa_y];
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// Cria Sincronizaçao de dos Buffers
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int CriaSyncMemoria() {

		sync.SemGwtoServComItem = CreateSemaphore(NULL, 0, Buffer_size, semGwLer);
		if (sync.SemGwtoServComItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwLer);
			return -1;
		}

		sync.SemGwtoServSemItem = CreateSemaphore(NULL, Buffer_size, Buffer_size, semGwEscrever);
		if (sync.SemGwtoServSemItem == NULL) {
			_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwEscrever);
			return -1;
		}

		InitializeCriticalSection(&sync.MutexGwtoSer);

		return 0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// cria Buffer na memoria partilhada 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*void criaMapeamentoMemoriaMapa(MaPa *aux, LPCTSTR nomeMapa) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeMapa);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar Mapa %s na memoria Partilhada "), nomeMapa);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste Mapa -> %s ja foi criado\n"), nomeMapa);

		}
		// mapear a memoria para a mensagem
		aux = (MaPa**)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(MaPa)*(dimMapa_x*dimMapa_y));

		if (aux == NULL) {

			_tprintf(TEXT("Erro a mapear Mapa"));
		}

	}*/
	void criaMapeamentoMemoriaServToGw(packet *aux, LPCTSTR nomeBuffer) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer);

		if (hBuffer == NULL) {
			_tprintf(TEXT("\nErro a criar slot %s na memoria Partilhada "), nomeBuffer);
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			_tprintf(TEXT("\nEste slot -> %s ja foi criado\n"), nomeBuffer);

		}
		// mapear a memoria para a mensagem
		aux = (packet*)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(packet));

		if (aux == NULL) {

			_tprintf(TEXT("Erro a mapear slot de mensagem"));
		}

	}
	void criaMapeamentoMemoriaGwToServ(ptrbufferMsg *aux, LPCTSTR nomeBuffer) {

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
		*aux = (ptrbufferMsg)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));

		if (*aux == NULL) {

			_tprintf(TEXT("Erro a mapear Buffer de mensagem"));
		}

		(*aux)->tail = 0;

		(*aux)->head = Buffer_size;

	}

	void CriaMemoriaPartilhada() {

		criaMapeamentoMemoriaGwToServ(&comGwtoServ, nomeGwtoServ);
		criaMapeamentoMemoriaServToGw(comServtoGw, nomeServtoGW);
		//criaMapeamentoMemoriaMapa(nomeMapa);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
												// Funçoes para escrever no buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// funcao que posiciona no buffer onde irá escrever o pacote
	void escreveNoBuffer(Packet *pacoteParaEscrita) {

		EnterCriticalSection(&sync.MutexGwtoSer);

			comGwtoServ->array[comGwtoServ->tail] = *pacoteParaEscrita;
		
				wcscpy_s(comGwtoServ->array[comGwtoServ->tail].dataPacket.nome, pacoteParaEscrita->dataPacket.nome);

				if (comGwtoServ->head == Buffer_size)
					comGwtoServ->head = comGwtoServ->tail;
				/////////////////////////////////PARA_FINS_DE_DEBUG///////////////////////////////////////////////////////////////
				//_tprintf(TEXT("Escrevi %s na pos %d"), comGwtoServ->array[comGwtoServ->tail].dataPacket.nome, comGwtoServ->tail);
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			comGwtoServ->tail = ++comGwtoServ->tail % Buffer_size;

		LeaveCriticalSection(&sync.MutexGwtoSer);

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

			WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);

				EnterCriticalSection(&sync.MutexGwtoSer);

					aux = &comGwtoServ->array[comGwtoServ->head];
					/////////////////////////////////PARA_FINS_DE_DEBUG///////////////////////////////////////////////////////////////
					//_tprintf(TEXT("Escrevi %s na pos %d"), comGwtoServ->array[comGwtoServ->head].dataPacket.nome, comGwtoServ->head);
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					comGwtoServ->head = ++comGwtoServ->head % Buffer_size;

					if (comGwtoServ->head == comGwtoServ->tail) {
						comGwtoServ->head = Buffer_size;
					}
				LeaveCriticalSection(&sync.MutexGwtoSer);

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


