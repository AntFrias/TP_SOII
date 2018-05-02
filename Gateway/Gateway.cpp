#include "HeaderGateway.h"


dataGw dadosGw;

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

int criaMemoriaPartilhada(ptrbufferMsg stg, ptrbufferMsg gts) { // funcao que vai Pedir para criar os buffers  na memoria partilhada

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
void escreveNoBuffer(Packet *pacoteParaEscrita) {

	// função que escreve no buffer


}
void escrevebufferGwToSr(LPVOID pacote) {

	int x = 10;

	Packet *auxPacote = (Packet*)pacote;

	auxPacote->tipo = 1;

	auxPacote->dataPacket.nome;

	_tprintf(TEXT("escreve coias"));
	
	escreveNoBuffer(auxPacote);



}

void IniciarGateway() {

	bufferMsg *auxSertoGw;
	bufferMsg *auxGwtoSer;
	Packet auxPacote;

	auxSertoGw = (ptrbufferMsg)malloc(sizeof(bufferMsg));
	auxGwtoSer = (ptrbufferMsg)malloc(sizeof(bufferMsg));

	criaMemoriaPartilhada(auxSertoGw, auxGwtoSer);

	dadosGw.comSertoGw = auxSertoGw;
	dadosGw.comGwtoSer = auxGwtoSer;

	//criar a thread que vai "escrever no buffer"
	dadosGw.hThreadGwtoSer = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)escrevebufferGwToSr,(LPVOID)&auxPacote,0,&dadosGw.idThreadGwtoSer);
	//todo esperar que as threads todas terminem


}
int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	 
	IniciarGateway();

	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	Sleep(90000);
	return 0;
}