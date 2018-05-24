#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataGw dadosGw;

void EncaminhamentoPacotesGwtoServ() {


}

void RecebePipeCliente(Clientes *Cliente) {

	Packet Pacote;
	
	DWORD nbytes;

	do {

		ReadFile(Cliente->hPipe, &Pacote, sizeof(Packet), &nbytes, NULL);
		
		escrevebufferGwToSr(&Pacote);

	} while (Cliente->Thread.Alive == 1);

}

clientes * criaSuporteComunicacaoCliente(clientes *arrayClientes, int nclientes) {

	clientes *aux;

	aux = (clientes *) malloc(sizeof(clientes)*nclientes);
	
	for (int i = 0; i < nclientes ; i++ ){

		aux[i] = arrayClientes[i];

	}
	return aux;
}

int IniciaNamedPipe() {

	dadosGw.nClientes = 0;

	clientes *arrayClientes = NULL;

	HANDLE hPipe;

	do {

		hPipe = CreateNamedPipe(PipeName, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, MaxClientes, sizeof(packet), sizeof(packet), INFINITE, NULL);

		if (hPipe == NULL) {

			_tprintf(TEXT("\n Erro ao criar Pipe Para Cliente %d"), dadosGw.nClientes);
			return -1;
		}
		else {

			_tprintf(TEXT("\nPipe Para Cliente %d Criado com sucesso"), dadosGw.nClientes);
		}


		if (!ConnectNamedPipe(hPipe, NULL)) {

			_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe\n"));

			return - 1 ;
		}

		dadosGw.nClientes++;

		arrayClientes = criaSuporteComunicacaoCliente( arrayClientes, dadosGw.nClientes );

		arrayClientes[ dadosGw.nClientes - 1 ].hPipe = hPipe;
	
		arrayClientes[ dadosGw.nClientes - 1 ].Thread.hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&arrayClientes[ dadosGw.nClientes - 1 ], 0, &arrayClientes[dadosGw.nClientes - 1].Thread.ThreadID );

	} while (dadosGw.nClientes < MaxClientes);

	// fazer o array de threads e por aqui o waitformultipleobjetct

}


// fun�ao que inicia os servi�os do Gateway
void IniciarGateway() {

	_tprintf(TEXT("\n\nGateway\n\n"));

	CriaMemoriaPartilhada();

	CriaSyncMemoria();

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