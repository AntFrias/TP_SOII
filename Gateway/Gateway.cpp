#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

//TCHAR BufGwtoBufGwtoServServ[] = TEXT("Buffer GwToSr");

dataGw dadosGw;

Thread *ThreadsCliente, ThreadsGateway[4];

clientes *arrayClientes;

void EnviaRespostaParaCliente(Packet *resposta) {

	_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}

void EnviaBroadcastPacote(Packet *resposta) {

	_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}

void LePacotesBufferServtoGw() {

	_tprintf(TEXT("\n\nCHEGUEI AQUI AO LEPACOTESDOBUFFERSERVTOGW e esta thread tem o alive a %d"), ThreadsGateway[3].Alive);  // ThreadsGateway array global

	packet *Resposta;

	do {
	
		Resposta = LerBufferServtoGw();

		_tprintf(TEXT("\n\nLi 1 Pacote"));

		if (Resposta->tipo == BroadcastPackage) {

			EnviaBroadcastPacote(Resposta);

		} else {

			EnviaRespostaParaCliente(Resposta);

		}
	} while (ThreadsGateway[3].Alive == 1);
}
// funcao que recebe pacotes do Pipe que veem do Cliente
void RecebePipeCliente(LPVOID *PosCliente) {

	int *posCliente = (int *)PosCliente;

	Packet Pacote;
	
	DWORD nbytes;

	while (ThreadsCliente[*posCliente].Alive == 1) {

		ReadFile(arrayClientes[*posCliente].hPipe, &Pacote, sizeof(Packet), &nbytes, NULL);
		
		escrevebuffer(&Pacote, nomeGwtoServ);

	} 
}
// Cria Thread para Respetivo Cliente
Thread * criaThreadParaCliente(Thread *threadsCliente, int nClientes) {

	Thread *aux;

	aux = (Thread *)malloc(sizeof(Thread)*nClientes);
	if (aux == NULL)
		return NULL;
	else
		return aux;

}
// adiciona novo Cliente ao Array de Clientes
clientes * AdicionaClienteArray(clientes *arrayClientes, int nclientes) {

	clientes *aux = NULL;
	
	aux = (clientes * ) malloc(sizeof(clientes)*(nclientes));
	
	aux = arrayClientes;

	return aux;
	
}
//adiciona mais um Handle ao Array de Handles para a Thread
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
// cria nova Thread para comunicaçao com Cliente atraves de namedPipe
HANDLE * CriaHandleParaNovaThread(HANDLE * hThreads, int nClientes) {
	
	HANDLE *aux;

	aux = (HANDLE *)malloc(sizeof(HANDLE)*nClientes);
	if (aux == NULL) {
		_tprintf(TEXT("\n\nErro a alocar Memoria Para Array de Handles da Thread "));
		exit(-1);
	}
	else {
		return aux;
	}
}

//Cria Thread Inicial para receber Primeiro Cliente
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
// cria espaço no array dos Clientes para o primeiro Cliente
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
// cria namedPipe/instancias para comunicaçao com os Clientes
HANDLE criaNamedPipe() {

	HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, MaxClientes, sizeof(packet), sizeof(packet), INFINITE, NULL);

	if (hPipe == NULL) {

		_tprintf(TEXT("\n\n Erro ao criar Pipe Para Cliente %d"), dadosGw.nClientes);
		return NULL;
	}
	else {

		_tprintf(TEXT("\n\nPipe Para Cliente %d Criado com sucesso"), dadosGw.nClientes);
	}

	if (!ConnectNamedPipe(hPipe, NULL)) {

		_tprintf(TEXT("\n\n ERRO na Ligaçao ao Cliente  (ConnectNamedPipe)\n"));

		return NULL;
	}
	else {

		_tprintf(TEXT("\n\nConectando o Cliente %d\n"), dadosGw.nClientes);

	}
	return hPipe;

}
//inicia Comunicaçao com CLiente
int criaComunicacaoClienteGateway() {

	dadosGw.nClientes = 0;

	HANDLE hPipe;

	HANDLE *hThreads;

	int PosCliente = 0;

	arrayClientes = CriaClienteInicial();					// Inicio da criaçao do array de clientes
				
	ThreadsCliente = criaThreadInicial();					// cria thread inicial para atender os Clientes

	hThreads = criaArrayHandlesThreads();					// cria inicio do Array de Handles para o waitformultipleobject

	ThreadsGateway[3].Alive = 1;

	ThreadsGateway[3].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LePacotesBufferServtoGw, (LPVOID)NULL, 0, &ThreadsGateway[3].ThreadID);
	_tprintf(TEXT("\n\nLancei a Thread que Recebe pacotes do servidor"));
	
	do {

		hPipe = criaNamedPipe();				// cria named pipe ou instancia chega Cliente

		dadosGw.nClientes++;					// adiciona um Cliente a mais para iniciar a configuraçao das Threads

		PosCliente = dadosGw.nClientes - 1;		// Posiçao onde irá ficar o novo Cliente no array das threads, handles de threads, array de clientes

		arrayClientes = AdicionaClienteArray(arrayClientes, dadosGw.nClientes);			//adiciona novo cliente ao array

		arrayClientes[PosCliente].hPipe = hPipe;			//adiciona  a instancia para o novo cliente

		ThreadsCliente = criaThreadParaCliente(ThreadsCliente, dadosGw.nClientes);		// cria thread para a comunicaçao com novo cliente

		ThreadsCliente[PosCliente].Alive = 1;				// poe a variavel da thread para ficar ativa

		hThreads = CriaHandleParaNovaThread(hThreads, dadosGw.nClientes);  // cria novo handle no array de Handles da Thread
		//inicia a thread de comunicaçao com o respetivo Cliente

		
		hThreads[PosCliente] = ThreadsCliente[PosCliente].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&PosCliente, 0, &ThreadsCliente[PosCliente].ThreadID);

		
	} while (dadosGw.nClientes < MaxClientes || dadosGw.ServerUp == 1); // trocar aqui o MAXClientes para 0 e o sinal para Maior

	WaitForMultipleObjects(NULL, hThreads, FALSE, INFINITE);

	//WaitForSingleObject(, INFINITE);

	for (int i = 0; i < dadosGw.nClientes; i++) {

		if (!DisconnectNamedPipe) {

			DisconnectNamedPipe(&arrayClientes[i].hPipe);
		}

		CloseHandle(&arrayClientes[i].hPipe);
	}

	return 0;
}
// funcao que inicia os servicos do Gateway
void IniciarGateway() {

	_tprintf(TEXT("\n\nGateway\n\n"));

	CriaMemoriaPartilhada();

	CriaSyncMemoriaGwtoServ();

	CriaSyncMemoriaServtoGw();

	dadosGw.ServerUp = 1;

	criaComunicacaoClienteGateway();

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