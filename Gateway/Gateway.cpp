#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataGw *dadosGw;
synBuffer sync;

int CriaSyncMemoria() {

	sync.SemGwtoSerPack = CreateSemaphore(NULL, 0, Buffer_size, semGwLer);
	if (sync.SemGwtoSerPack == NULL) {
		_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwLer);
		return -1;
	}

	sync.SemGwtoSerPos = CreateSemaphore(NULL, Buffer_size, Buffer_size, semGwEscrever);
	if (sync.SemGwtoSerPos == NULL) {
		_tprintf(TEXT("Erro ao criar Semaforo %s"), semGwEscrever);
		return -1;
	}

	
	return 0;
}
ptrbufferMsg CriaShareBuffer(HANDLE hBuffer, LPCTSTR nomeBuffer) {  // cria Buffer na memoria partilhada 

	ptrbufferMsg auxBuffer;

	//criar a memoria partilhada 
	hBuffer = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferMsg), nomeBuffer);
	if (hBuffer == NULL) {
		_tprintf(TEXT("\nErro a criar Buffer %s na memoria Partilhada "), nomeBuffer);
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf(TEXT("\nEste buffer -> %s ja foi criado\n"),nomeBuffer);
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

int criaMemoriaPartilhada(ptrbufferMsg *stg, ptrbufferMsg *gts) { // funcao que vai Pedir para criar os buffers  na memoria partilhada

	//ptrbufferMsg stg, gts;

	*stg = CriaShareBuffer(&stg, nomeSrtoGW);
	*gts = CriaShareBuffer(&gts, nomeGwtoSr);

	return 0;
}
void escreveNoBuffer(Packet *pacoteParaEscrita) {
	
	int *out = &dadosGw->comGwtoSer->out; //change
	int *in = &dadosGw->comGwtoSer->in;

	dadosGw->comGwtoSer->array[*in] = *pacoteParaEscrita;
	
	EnterCriticalSection(&sync.MutexGwtoSer);
	_tprintf(TEXT("\naquiwewrewrewrewrew\n"));
	//*in = *in == Buffer_size - 1 ? 0 : *in += 1;
	
	if (dadosGw->comGwtoSer->out < Buffer_size - 2) {
		*out += 1 ;
	
		_tprintf(TEXT("OUT: %d\n"), *out);
	}else if (*out == Buffer_size - 2) {
		_tprintf(TEXT("bUFFER CHEIO\n"));
		*out = *in;
		*in = 0;
	}
	
	LeaveCriticalSection(&sync.MutexGwtoSer);
	
	
	_tprintf(TEXT("5\n"));
}

void escrevebufferGwToSr(LPVOID pacote) {

	int x = 10;

	Packet *auxPacote = (Packet*)pacote;

	auxPacote->tipo = 1;

	auxPacote->dataPacket.nome;
	dadosGw->comGwtoSer->in = 0;
	
	do {
		
		_fgetts(auxPacote->dataPacket.nome, 10,stdin);
		
		auxPacote->tipo = 1;
		
		WaitForSingleObject(sync.SemGwtoSerPos, INFINITE);
		
		escreveNoBuffer(auxPacote);
		
		ReleaseSemaphore(sync.SemGwtoSerPack, 1, NULL);
		
	} while (1);
	
}

void IniciarGateway() {

	bufferMsg *auxSertoGw;
	bufferMsg *auxGwtoSer;
	Packet auxPacote;

	auxSertoGw = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	auxGwtoSer = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	criaMemoriaPartilhada(&auxSertoGw, &auxGwtoSer);

	dadosGw->comSertoGw = auxSertoGw;
	dadosGw->comGwtoSer = auxGwtoSer;

	_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->in);
	_tprintf(TEXT("\n\n%d"), dadosGw->comGwtoSer->out);

	//CriaSyncMemoria();

	//criar a thread que vai "escrever no buffer"
	dadosGw->hThreadGwtoSer = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)escrevebufferGwToSr,(LPVOID)&auxPacote,0,&dadosGw->idThreadGwtoSer);
	//todo esperar que as threads todas terminem


}
void Alocacoes() {
	dadosGw = (dataGw*) malloc(sizeof(dataGw));
}
int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	

	
	_tprintf(TEXT("\nconta: %d \n"),sum(1,1));
	InitializeCriticalSection(&sync.MutexGwtoSer);
	Alocacoes();
	IniciarGateway();

	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	Sleep(90000);
	return 0;
}