// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
//criar detallhes para as funçoes
#define AcessoMemDLL


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
//aqui podemos fazer a funções que declaramos no .h
extern "C" {

	//Nome do Buffer Server to Gateway
	TCHAR nomeServtoGw[] = TEXT("Buffer SrToGw");
	TCHAR nomeGwtoServ[] = TEXT("Buffer GwToSr");
	TCHAR nomeMapa[] = TEXT("Mapa");

	// Nome dos Semaforos para comunicacao nos Buffers de mensagens
	TCHAR semServLer[] = TEXT("semaforoServLer");
	TCHAR semGwEscrever[] = TEXT("semaforoGwEscrever");
	TCHAR mutexGwtoSer[] = TEXT("mutexGwtoSer");


	TCHAR semGwLer[] = TEXT("semaforoGwLer");
	TCHAR SemServEscrever[] = TEXT("semaforoServEscrever");
	TCHAR mutexServtoGw[] = TEXT("mutexServtoGw");

	synBuffer sync;			// estao guardados todos os mecanismos de sincronizaçao entre Gateway e Servidor

	bufferMsg *comGwtoServ, *comServtoGw;		// estao guardados os Buffers de Co municaçao entre Gateway e Servidor

	MaPa *Mapa;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cria Sincronizaçao do buffer ServtoGw
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cria Sincronizaçao do buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// cria Buffer na memoria partilhada 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void criaMapeamentoMemoriaMapa(LPCTSTR nMapa) {

		HANDLE hBuffer = NULL;
		//criar a memoria partilhada 
		hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MaPa), nMapa);

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
	// cria mapeamento de memoria para GwtoServ
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
	// funçao que vai criar os mapeamentos de memoria necessária
	void CriaMemoriaPartilhada() {

		criaMapeamentoMemoriaGwToServ(nomeGwtoServ);
		criaMapeamentoMemoriaServToGw(nomeServtoGw);
		criaMapeamentoMemoriaMapa(nomeMapa);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para escrever no buffer ServtoGw
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void escreveNoBufferServtoGw(Packet *pacoteParaEscrita) {

		WaitForSingleObject(sync.MutexServtoGw, INFINITE);//mudar aqui

		comServtoGw->array[comServtoGw->tail] = *pacoteParaEscrita;

		wcscpy_s(comServtoGw->array[comServtoGw->tail].dataPacket.nome, pacoteParaEscrita->dataPacket.nome);  // isto deve ser retirado daqui

		if (comServtoGw->head == Buffer_size) {

			comServtoGw->head = comGwtoServ->tail;
		}

		comServtoGw->tail = ++comServtoGw->tail % Buffer_size;

		ReleaseMutex(sync.MutexGwtoSer);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para escrever no buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes que preenche o pacote do buffer
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void escrevebuffer(packet *aux, TCHAR *nomeBuffer) {
		
		aux->Cliente_id = (DWORD) GetThreadId;

		if (_tcscmp(nomeBuffer, nomeGwtoServ) == 0) {

			WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);

			escreveNoBufferGwtoServ(aux);

			ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);
		}
		else {

			WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);

			escreveNoBufferServtoGw(aux);

			ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);

		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Funçoes para ler pacotes do buffer GwtoServ
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Packet * LerBufferGwtoSer() {

		Packet *aux;

		WaitForSingleObject(sync.SemGwtoServComItem, INFINITE);

		WaitForSingleObject(sync.MutexGwtoSer, INFINITE);

		aux = &comGwtoServ->array[comGwtoServ->head];

		comGwtoServ->head = ++comGwtoServ->head % Buffer_size;

		if (comGwtoServ->head == comGwtoServ->tail) {

			comGwtoServ->head = Buffer_size;
		}
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


