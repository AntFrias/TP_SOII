#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")
int * global_int;
BufferMsg * auxbuff;


dataGw *dadosGw;
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
ptrbufferMsg CriaShareBuffer(HANDLE hBuffer2, LPCTSTR nomeBuffer) {  // cria Buffer na memoria partilhada 

	ptrbufferMsg auxBuffer;
	HANDLE hBuffer;
	//criar a memoria partilhada 
	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer);
	if (hBuffer == NULL) {
		_tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste buffer -> %s ja foi criado\n"),nomeBuffer);
	}
	// mapear a memoria para a mensagem
	//auxBuffer = (ptrbufferMsg)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));
	dadosGw->comGwtoSer = (ptrbufferMsg)MapViewOfFile(hBuffer, FILE_MAP_WRITE, 0, 0, sizeof(bufferMsg));
	if (dadosGw->comGwtoSer  == NULL) {
		_tprintf(TEXT("Erro a mapear Buffer de mensagem"));
	}

	//dadosGw->comGwtoSer->tail  = 0; 
	dadosGw->comGwtoSer->head = Buffer_size;
	//auxBuffer->in = Buffer_size;
	//auxBuffer->out = 0;

	return NULL;

}

int criaMemoriaPartilhada(ptrbufferMsg *stg, ptrbufferMsg *gts) { // funcao que vai Pedir para criar os buffers  na memoria partilhada

	//ptrbufferMsg stg, gts;

	//*stg = CriaShareBuffer(&stg, nomeSrtoGW);
	*gts = CriaShareBuffer(&gts, nomeGwtoSr);

	return 0;
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

void escrevebufferGwToSr(LPVOID pacote) {

	int x = 10;

	
	
	do {
		 Packet auxPacote;// = (Packet*)pacote;

		//auxPacote.tipo = 1;

		//auxPacote.dataPacket.nome;

		_fgetts(auxPacote.dataPacket.nome, 10,stdin);
		
		///auxPacote.tipo = x++;
		auxbuff->head++;

		WaitForSingleObject(sync.SemGwtoServSemItem, INFINITE);
		
		wcscpy_s(auxbuff->array[0].dataPacket.nome, TEXT("olaolaola"));
		//escreveNoBuffer(&auxPacote);
		_tprintf(TEXT(" global_int = %s \n "), auxbuff->array[0].dataPacket.nome);
		
		ReleaseSemaphore(sync.SemGwtoServComItem, 1, NULL);
		
	} while (1);
	
}

void IniciarGateway() {

	bufferMsg *auxSertoGw;
	bufferMsg *auxGwtoSer;
	Packet auxPacote;

	/*auxSertoGw = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	auxGwtoSer = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	*/

	//criaMemoriaPartilhada(&auxSertoGw, &auxGwtoSer);

	/*dadosGw->comSertoGw = auxSertoGw;
	dadosGw->comGwtoSer = auxGwtoSer;*/

	//_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->tail);
	//_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->head);

	CriaSyncMemoria();

	//criar a thread que vai "escrever no buffer"
	dadosGw->hThreadGwtoSer = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)escrevebufferGwToSr,(LPVOID)&auxPacote,0,&dadosGw->idThreadGwtoSer);
	//todo esperar que as threads todas terminem


}
void Alocacoes() {
	dadosGw = (dataGw*) malloc(sizeof(dataGw));
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

	_tprintf(TEXT("\nconta: %d \n shared int created \n"),sum(1,1));


	InitializeCriticalSection(&sync.MutexGwtoSer);
	
	IniciarGateway();




	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	Sleep(190000);
	return 0;
}