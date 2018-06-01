#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

//TCHAR BufGwtoBufGwtoServServ[] = TEXT("Buffer GwToSr");

dataGw dadosGw;
clientes Clientes[20];
int startIdCli = 1000;


// funcao que recebe pacotes do Pipe que veem do Cliente
void RecebePipeCliente(LPVOID *PosCliente) {

	int *posCliente = (int *)PosCliente;

	Packet Pacote;
	DWORD nbytes;
	BOOL ret;

	HANDLE IOReady;
	
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (IOReady == NULL) {
		_tprintf(TEXT("Erro no IOReady\n"));
		exit(-1);
	}
	OVERLAPPED Ov;

	while (Clientes[*posCliente].thAlive) {

		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;

		ReadFile(Clientes[*posCliente].hPipe, &Pacote, sizeof(Packet), &nbytes, &Ov);
		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(Clientes[*posCliente].hPipe, &Ov, &nbytes, FALSE);
		
		if (!ret || !nbytes) {
			_tprintf(TEXT("[Gateway] ret %d nbytes %d... (ReadFile)\n"), ret,nbytes);
			break;
		}

		escrevebuffer(&Pacote, nomeGwtoServ);

	}

	DisconnectNamedPipe(Clientes[*posCliente].hPipe);
	CloseHandle(Clientes[*posCliente].hPipe);
	CloseHandle(IOReady);
	Clientes[*posCliente].hPipe = INVALID_HANDLE_VALUE;

}
// cria namedPipe/instancias para comunicaçao com os Clientes
HANDLE criaNamedPipe() {


	HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 20, sizeof(packet), sizeof(packet), 1000, NULL);

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
void EnviaRespostaParaCliente(Packet *resposta) {

	_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}

void EnviaBroadcastPacote(Packet *resposta) {
	
	DWORD nBytes;

	BOOL ret;

	HANDLE IOReady;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (IOReady == NULL) {
		_tprintf(TEXT("Erro no IOReady\n"));
		exit(-1);
	}
	OVERLAPPED Ov;

	for (int i = 0; i < dadosGw.nClientes; i++) {
		if (Clientes[i].hPipe != INVALID_HANDLE_VALUE) {
		
			ZeroMemory(&Ov, sizeof(Ov));
			ResetEvent(IOReady);
			Ov.hEvent = IOReady;
			_tprintf(TEXT("TCHEGUI AQUI\n\n"));
			WriteFile(Clientes[i].hPipe, resposta, sizeof(Packet), &nBytes, &Ov);  //Se Write dá erro, o cliente desconectou-se
			_tprintf(TEXT("TCHEGUI AQUI1\n\n"));
			WaitForSingleObject(IOReady, INFINITE);
			_tprintf(TEXT("TCHEGUI AQUI2\n\n"));
			ret = GetOverlappedResult(Clientes[i].hPipe, &Ov, &nBytes, FALSE);

			if (!ret || !nBytes) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				DisconnectNamedPipe(Clientes[i].hPipe);	//fechar os recursos associados ao cliente que se desligou
				CloseHandle(Clientes[i].hPipe);
				CloseHandle(IOReady);
				//WaitForSingleObject(hMutex, INFINITE);
				Clientes[i].hPipe = INVALID_HANDLE_VALUE;
				//////ReleaseMutex(hMutex);
			}
		}
	}

	//_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	//_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}
void LePacotesBufferServtoGw() {

	packet *Resposta;

	do {

		Resposta = LerBufferServtoGw();
		
		_tprintf(TEXT("\n\nuser ID: %d"), Resposta->Cliente_id);
		_tprintf(TEXT("\nuser Name: %s"), Resposta->dataPacket.nome);
		
		Resposta->tipo = BroadcastPackage;

		if (Resposta->tipo == BroadcastPackage) {

			EnviaBroadcastPacote(Resposta);

		}
		else {

			//EnviaRespostaParaCliente(Resposta);

		}

	} while (dadosGw.ServerUp);

}

//inicia Comunicaçao com CLiente
int criaComunicacaoClienteGateway() {

	dadosGw.nClientes = 0;
	HANDLE hPipe;
	HANDLE hThreads[21];
	DWORD idThLeServToGw;
	int PosThLeServToGw = 0;

	hThreads[PosThLeServToGw] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LePacotesBufferServtoGw, (LPVOID)NULL, 0, &idThLeServToGw);
	_tprintf(TEXT("\n\nLancei a Thread que Recebe pacotes do servidor"));
	
	do {

		hPipe = criaNamedPipe();							// cria named pipe ou instancia chega Cliente
		
		if (hPipe != NULL) {

			dadosGw.nClientes++;								// adiciona um Cliente a mais para iniciar a configuraçao das Threads

			Clientes[dadosGw.nClientes].hPipe = hPipe;			//adiciona  a instancia para o novo cliente

			Clientes[dadosGw.nClientes].thAlive = 1;				// poe a variavel da thread para ficar ativa

			Clientes[dadosGw.nClientes].id = startIdCli + 1;

			hThreads[dadosGw.nClientes] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&dadosGw.nClientes, 0, &Clientes[dadosGw.nClientes].iDThread);
		
		}

	} while (dadosGw.ServerUp == 1); 

	WaitForMultipleObjects(NULL, hThreads, FALSE, INFINITE);

	for (int i = 0; i < dadosGw.nClientes; i++) {

		DisconnectNamedPipe(&Clientes[i].hPipe);

		CloseHandle(&Clientes[i].hPipe);
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