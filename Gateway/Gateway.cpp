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

	DWORD nBytes;
	BOOL ret;
	
	for (int i = 0; i < dadosGw.nClientes; i++) {

		ZeroMemory(&arrayClientes[i].Ov, sizeof(arrayClientes[i].Ov));
		ResetEvent(arrayClientes[i].IOReady);
		arrayClientes[i].Ov.hEvent = arrayClientes[i].IOReady;


		WriteFile(arrayClientes[i].hPipe, resposta,sizeof(packet), &nBytes, &arrayClientes[i].Ov);  //Se Write dá erro, o cliente desconectou-se
		WaitForSingleObject(arrayClientes[i].IOReady, INFINITE);
		
		
		ret = GetOverlappedResult(arrayClientes[i].hPipe, &arrayClientes[i].Ov, &nBytes, FALSE);

		if (!ret || !nBytes) {
			_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
			DisconnectNamedPipe(arrayClientes[i].hPipe);	//fechar os recursos associados ao cliente que se desligou
			CloseHandle(arrayClientes[i].hPipe);
			//WaitForSingleObject(hMutex, INFINITE);
			arrayClientes[i].hPipe = INVALID_HANDLE_VALUE;
			//////ReleaseMutex(hMutex);
		}


	}

	//_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	//_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}

void LePacotesBufferServtoGw() {

	_tprintf(TEXT("\n\nCHEGUEI AQUI AO LEPACOTESDOBUFFERSERVTOGW e esta thread tem o alive a %d"), ThreadsGateway[3].Alive);  // ThreadsGateway array global

	packet *Resposta;
	//int i = 5; //debug
	do {

		Resposta = LerBufferServtoGw();
		Resposta->tipo = BroadcastPackage;
		_tprintf(TEXT("\n\nLi 1 Pacote"));

		if (Resposta->tipo == BroadcastPackage) {

			EnviaBroadcastPacote(Resposta);

		}
		else {

			EnviaRespostaParaCliente(Resposta);

		}
	
		//	i--;
	//} while (i > 0); //debug
	} while (ThreadsGateway[3].Alive == 1);
	
	//fecha tudo no fim
	for (int j = 0; j < dadosGw.nClientes; j++) {
		if (arrayClientes[j].hPipe != INVALID_HANDLE_VALUE)
			if (!DisconnectNamedPipe(arrayClientes[j].hPipe)) {
				_tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
				exit(-1);
			}
	}


	for (int j = 0; j < dadosGw.nClientes; j++) {
		if (arrayClientes[j].hPipe != INVALID_HANDLE_VALUE)
			CloseHandle(arrayClientes[j].hPipe);
	}
	for (int j = 0; j < dadosGw.nClientes; j++) {
		CloseHandle(arrayClientes[j].IOReady);
	}

}
// funcao que recebe pacotes do Pipe que veem do Cliente
void RecebePipeCliente(LPVOID *PosCliente) {

	int *posCliente = (int *)PosCliente;

	Packet Pacote;
	DWORD nbytes;
	BOOL ret;

	arrayClientes[*posCliente].IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);


	while (ThreadsCliente[*posCliente].Alive == 1) {

		ZeroMemory(&arrayClientes[*posCliente].Ov, sizeof(arrayClientes[*posCliente].Ov));
		ResetEvent(&arrayClientes[*posCliente].IOReady);
		arrayClientes[*posCliente].Ov.hEvent = arrayClientes[*posCliente].IOReady;


		ReadFile(arrayClientes[*posCliente].hPipe, &Pacote, sizeof(Packet), &nbytes, &arrayClientes[*posCliente].Ov);
		WaitForSingleObject(arrayClientes[*posCliente].IOReady, INFINITE);
		ret = GetOverlappedResult(arrayClientes[*posCliente].hPipe, &arrayClientes[*posCliente].Ov, &nbytes, FALSE);
		
		if (!ret ) {
			_tprintf(TEXT("[Gateway] ret %d... (ReadFile)\n"), ret);
			break;
		}
		if (!nbytes) {
			_tprintf(TEXT("[Gateway] nbytes %d (ReadFile)\n"), nbytes);
			break;
		}

		escrevebuffer(&Pacote, nomeGwtoServ);

	}

	DisconnectNamedPipe(arrayClientes[*posCliente].hPipe);
	CloseHandle(arrayClientes[*posCliente].hPipe);
	CloseHandle(arrayClientes[*posCliente].IOReady);
	arrayClientes[*posCliente].hPipe = INVALID_HANDLE_VALUE;

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


	HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 5, sizeof(packet), sizeof(packet), 1000, NULL);

	if (hPipe == INVALID_HANDLE_VALUE) {

		_tprintf(TEXT("\n\n Erro ao criar Pipe Para Cliente %d"), dadosGw.nClientes);
		return NULL;//aqui deve fechar o programa exit(-1)
	}
	else {

		_tprintf(TEXT("\n\nPipe Para Cliente %d Criado com sucesso"), dadosGw.nClientes);
	}

	if (!ConnectNamedPipe(hPipe, NULL)) {//bloqueante

		_tprintf(TEXT("\n\n ERRO na Ligaçao ao Cliente  (ConnectNamedPipe)\n"));

		return NULL;//aqui deve fechar o programa exit(-1)
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

		//epa esta condição tem que ser trocada ... pois se algum cliente se desligar já não se consegue ligar mais nenhum TODO

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