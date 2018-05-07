#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")


dataGw dadosGw;

void EncaminhamentoPacotesGwtoServ() {

	Packet Pacote;

	escrevebufferGwToSr(&Pacote);
}

// funçao que inicia os serviços do Gateway
void IniciarGateway() {

	Packet Pacote;

	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	CriaMemoriaPartilhada();

	CriaSyncMemoria();

	dadosGw.hThreadGwtoSer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncaminhamentoPacotesGwtoServ, (LPVOID)NULL, 0, &dadosGw.idThreadGwtoSer);

}

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	
	IniciarGateway();

	Sleep(190000);
	return 0;
}