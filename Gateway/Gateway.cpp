#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataGw dadosGw;

void EncaminhamentoPacotesGwtoServ() {


}

void RecebePipeCliente(LPVOID *Cliente) {

	Clientes * aux = (Clientes *)Cliente;

	Packet Pacote;
	
	DWORD nbytes;

	_tprintf(TEXT("\n\nCheguei aqui"));

	while (aux->Thread.Alive == 1) {

		ReadFile(aux->hPipe, &Pacote, sizeof(Packet), &nbytes, NULL);
		
		_tprintf(TEXT("\n\nCheguei aqui e li um pacote"));
		
		escrevebufferGwToSr(&Pacote);

	} 
	Sleep(10000);
}

clientes * criaSuporteComunicacaoCliente(clientes *arrayClientes, int nclientes) {

	clientes *aux = NULL;
	
	aux = (clientes * ) malloc(sizeof(clientes)*(nclientes));
	
	aux = arrayClientes;

	return aux;
	
}

int IniciaNamedPipe() {

	dadosGw.nClientes = 0;

	clientes *arrayClientes = NULL;

	HANDLE hPipe;

	arrayClientes = (clientes *)malloc(sizeof(Clientes));

	if (arrayClientes == NULL) {
	
		_tprintf(TEXT("\n\nErro a alocar memoria para inicio do Array de Clientes"));
		
		return -1;
	}

	do {

		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, MaxClientes, sizeof(packet), sizeof(packet), INFINITE, NULL);

		if (hPipe == NULL) {

			_tprintf(TEXT("\n\n Erro ao criar Pipe Para Cliente %d"), dadosGw.nClientes);
			return -1;
		}
		else {

			_tprintf(TEXT("\n\nPipe Para Cliente %d Criado com sucesso"), dadosGw.nClientes);
		}

		if (!ConnectNamedPipe(hPipe, NULL)) {

			_tprintf(TEXT("\n\n ERRO na Ligaçao ao Cliente  (ConnectNamedPipe)\n"));

			return - 1 ;
		}
		else {

			_tprintf(TEXT("\n\nConectando o Cliente %d\n"), dadosGw.nClientes);

		}

		dadosGw.nClientes++;
	
		arrayClientes = criaSuporteComunicacaoCliente( arrayClientes, dadosGw.nClientes );

		arrayClientes[ dadosGw.nClientes - 1 ].hPipe = hPipe;
	
		arrayClientes[dadosGw.nClientes - 1].Thread.Alive = 1;

		
		arrayClientes[ dadosGw.nClientes - 1 ].Thread.hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&arrayClientes[ dadosGw.nClientes - 1 ], 0, &arrayClientes[dadosGw.nClientes - 1].Thread.ThreadID );

	} while (dadosGw.nClientes < MaxClientes || dadosGw.ServerUp == 1);

	WaitForSingleObject(arrayClientes->Thread.hThread, INFINITE);
	// fazer o array de threads e por aqui o waitformultipleobjetct


}



// fun�ao que inicia os servi�os do Gateway
void IniciarGateway() {

	_tprintf(TEXT("\n\nGateway\n\n"));

	CriaMemoriaPartilhada();

	CriaSyncMemoria();

	dadosGw.ServerUp = 1;

	IniciaNamedPipe();

	//dadosGw.hThreadGwtoSer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EncaminhamentoPacotesGwtoServ, (LPVOID)NULL, 0, &dadosGw.idThreadGwtoSer);

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