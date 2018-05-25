#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataGw dadosGw;

Thread *ThredsCliente;

clientes *arrayClientes;

void RecebePipeCliente(LPVOID *PosCliente) {

	int *posCliente = (int *)PosCliente;

	Packet Pacote;
	
	DWORD nbytes;

	while (arrayClientes[*posCliente].Thread.Alive == 1) {

		ReadFile(arrayClientes[*posCliente].hPipe, &Pacote, sizeof(Packet), &nbytes, NULL);
		
		escrevebufferGwToSr(&Pacote);

	} 
}
Thread * criaThreadParaCliente(Thread *threadsCliente, int nClientes) {

	Thread *aux;

	aux = (Thread *)malloc(sizeof(Thread)*nClientes);
	if (aux == NULL)
		return NULL;
	else
		return aux;

}

clientes * AdicionaClienteArray(clientes *arrayClientes, int nclientes) {

	clientes *aux = NULL;
	
	aux = (clientes * ) malloc(sizeof(clientes)*(nclientes));
	
	aux = arrayClientes;

	return aux;
	
}
HANDLE * criaArrayHandlesThreads() {

	HANDLE *aux;

	aux = (HANDLE *)malloc(sizeof(HANDLE));
	if (aux == NULL) {
		_tprintf(TEXT("\n\nErro a alocar Memoria Para Array de Thread "));
		exit(-1);
	}
	else {
		return aux;
	}
}

Thread * criaThreadInicial() {

	Thread *aux;

	aux = (Thread *)malloc(sizeof(Thread));
	if (aux == NULL) {
		_tprintf(TEXT("\n\nErro a alocar Memoria Para Estrutura Thread que irá atende o Cliente"));
		exit(-1);
	}
	else {
		return aux;
	}
}
Clientes * CriaClienteInicial() {

	Clientes *aux;

	aux = (clientes *)malloc(sizeof(Clientes));
	if (aux == NULL) {
		_tprintf(TEXT("\n\nErro a alocar Memoria para Array de Clientes..!"));
		exit(-1);
	}	else {
		return aux;
	}
}
int IniciaNamedPipe() {

	dadosGw.nClientes = 0;

	HANDLE hPipe;

	HANDLE *hThreads;

	int PosCliente = 0;

	arrayClientes = CriaClienteInicial();

	ThredsCliente = criaThreadInicial();

	hThreads = criaArrayHandlesThreads();

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

			return -1;
		}
		else {

			_tprintf(TEXT("\n\nConectando o Cliente %d\n"), dadosGw.nClientes);

		}

		dadosGw.nClientes++;

		arrayClientes = AdicionaClienteArray(arrayClientes, dadosGw.nClientes);

		arrayClientes[dadosGw.nClientes - 1].hPipe = hPipe;

		ThredsCliente = criaThreadParaCliente(ThredsCliente, dadosGw.nClientes);

		arrayClientes[dadosGw.nClientes - 1].Thread.Alive = 1;

		PosCliente = dadosGw.nClientes - 1;

		hThreads[dadosGw.nClientes - 1] = ThredsCliente[dadosGw.nClientes - 1].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&PosCliente, 0, &ThredsCliente[dadosGw.nClientes - 1].ThreadID);

	} while (dadosGw.nClientes < MaxClientes || dadosGw.ServerUp == 1);

	WaitForMultipleObjects(20, hThreads, FALSE, INFINITE);

	for (int i = 0; i < dadosGw.nClientes; i++) {

		if ( !DisconnectNamedPipe)
		
			DisconnectNamedPipe(&arrayClientes[i].hPipe);

		CloseHandle(&arrayClientes[i].hPipe);
	}

	return 0;

}

// fun�ao que inicia os servi�os do Gateway
void IniciarGateway() {

	_tprintf(TEXT("\n\nGateway\n\n"));

	CriaMemoriaPartilhada();

	CriaSyncMemoria();

	dadosGw.ServerUp = 1;

	IniciaNamedPipe();

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