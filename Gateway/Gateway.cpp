#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")
//int * global_int;
//BufferMsg * auxbuff;

dataGw dadosGw;
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

	
	return 0;
}
// cria Buffer na memoria partilhada 
void criaMemoriaPartilhada(ptrbufferMsg *aux, LPCTSTR nomeBuffer) {

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

void escreveNoBuffer(Packet *pacoteParaEscrita) {
/*	++dadosGw->comGwtoSer->head;
	int head = dadosGw->comGwtoSer->head; //change
	int tail = dadosGw->comGwtoSer->tail;

	//EnterCriticalSection(&sync.MutexGwtoSer);

			dadosGw->comGwtoSer->array[tail] = *pacoteParaEscrita; 
			//	strcpy_s(dadosGw->comGwtoSer->array[tail].dataPacket.nome, pacoteParaEscrita->dataPacket.nome); 
			_tprintf(TEXT("\n   head = %d   \n"), dadosGw->comGwtoSer->head);
			_tprintf(TEXT("\naquiwewrewrewrewrew\n"));
			//*in = *in == Buffer_size - 1 ? 0 : *in += 1;
	
			/*if (dadosGw->comGwtoSer->head == Buffer_size)
				dadosGw->comGwtoSer->head = dadosGw->comGwtoSer->tail;

			dadosGw->comGwtoSer->tail = ++dadosGw->comGwtoSer->tail % Buffer_size;
			*/
	//LeaveCriticalSection(&sync.MutexGwtoSer);
	
	
	_tprintf(TEXT("5\n"));
}

void escrevebufferGwToSr() {
	_tprintf(TEXT(" 0\n "));
	packet Pacote;
	
	
	do {
		 Packet auxPacote;

		auxPacote.tipo = 1;

		
		_fgetts(auxPacote.dataPacket.nome, 10,stdin);

		dadosGw.comGwtoSer->head++;
		_tprintf(TEXT(" 1\n "));

		WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);
		
		wcscpy_s(dadosGw.comGwtoSer->array[0].dataPacket.nome, TEXT("olaolaola"));
		//escreveNoBuffer(&auxPacote);
		_tprintf(TEXT(" 2\n "));
		_tprintf(TEXT(" global_int = %s \n "), dadosGw.comGwtoSer->array[0].dataPacket.nome);
		
		ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);
		_tprintf(TEXT(" 3\n "));
	} while (1);
	
}

void IniciarGateway() {

	criaMemoriaPartilhada(&dadosGw.comGwtoSer, nomeGwtoSr);

	/*dadosGw->comSertoGw = auxSertoGw;
	dadosGw->comGwtoSer = auxGwtoSer;*/

	//_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->tail);
	//_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->head);

	CriaSyncMemoria();

	//criar a thread que vai "escrever no buffer"
	dadosGw.hThreadGwtoSer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escrevebufferGwToSr, (LPVOID)NULL, 0, &dadosGw.idThreadGwtoSer);
	//todo esperar que as threads todas terminem
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
	
	//HANDLE HtoIntSHR = NULL;
	//auxbuff = createSharedINT(HtoIntSHR, TEXT("SHRED_INT"));
	//auxbuff->head = 0;

	_tprintf(TEXT("\nconta: %d \n shared int created \n"),sum(1,1));


	//InitializeCriticalSection(&sync.MutexGwtoSer);
	
	IniciarGateway();




	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	Sleep(190000);
	return 0;
}