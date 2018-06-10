#include "HeaderGateway.h"
#include "../AcessoMemDLL/stdafx.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

dataGw dadosGw;

clientes Clientes[nMaxPlay];

// funcao que recebe pacotes do Pipe que veem do Cliente
void RecebePipeCliente(LPVOID *Cli) {
	

	clientes *cli = (clientes *)Cli;

	Packet Pacote;

	DWORD nbytes;
	
	BOOL ret;

	HANDLE IOReady = NULL;
	
	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (IOReady == NULL) {
		_tprintf(TEXT("Erro no IOReady\n"));
		exit(-1);
	}
	OVERLAPPED Ov;
	
	while (cli->thAlive) {
		
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;
		ReadFile(cli->hPipe, &Pacote, sizeof(Packet), &nbytes, &Ov);
	
		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(cli->hPipe, &Ov, &nbytes, FALSE);
		if (!ret || !nbytes) {
			_tprintf(TEXT("[Gateway] ret %d nbytes %d... (ReadFile)gat\n"), ret, nbytes);
	
		}
		//mutex todo
		Pacote.Cliente_id = cli->id;
		
		escrevebuffer(&Pacote, nomeGwtoServ);
		
	}

	DisconnectNamedPipe(cli->hPipe);
	CloseHandle(cli->hPipe);
	CloseHandle(IOReady);
	cli->hPipe = INVALID_HANDLE_VALUE;

}
// cria namedPipe/instancias para comunicaçao com os Clientes
HANDLE criaNamedPipe() {


	HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, nMaxPlay, sizeof(packet), sizeof(packet), 1000, NULL);

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

	return hPipe;

}
//Funcao que vai buscar o Handle do Pipe para enviar a mensagem para um determinado Cliente
HANDLE getPipeDoCli(packet *resposta) {
	
	for (int i = 0; i < dadosGw.nClientes; i++) {

		if (Clientes[i].id == resposta->Cliente_id) {
			return Clientes[i].hPipe;
		}
	}
	return NULL;
}
//Envia Resposta para apenas 1 Cliente
void EnviaRespostaParaCliente(Packet *resposta) {

	DWORD nBytes;

	BOOL ret;

	HANDLE IOReady = NULL;

	HANDLE hPipe = NULL;

	hPipe = getPipeDoCli(resposta);
	_tprintf(TEXT("o id %d nome do cli %s\n"),resposta->Cliente_id,resposta->dataPacket.nome);
	if(hPipe == NULL) {

		_tprintf(TEXT("\n Erro a Enviar resposta do Gateway para o Cliente"));
	}

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (IOReady == NULL) {

		_tprintf(TEXT("Erro no IOReady\n"));

		exit(-1);
	}
	OVERLAPPED Ov;
	
		//if (hPipe != INVALID_HANDLE_VALUE) {

			ZeroMemory(&Ov, sizeof(Ov));

			ResetEvent(IOReady);
			
			Ov.hEvent = IOReady;
			
			WriteFile(hPipe, resposta, sizeof(Packet), &nBytes, &Ov);  //Se Write dá erro, o cliente desconectou-se
			
			WaitForSingleObject(IOReady, INFINITE);
			
			ret = GetOverlappedResult(hPipe, &Ov, &nBytes, FALSE);
		//}
	

}
//Funcao que envia Broadcast de Pacotes para todos os Clientes
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

			WriteFile(Clientes[i].hPipe, resposta, sizeof(Packet), &nBytes, &Ov);  //Se Write dá erro, o cliente desconectou-se
			
			WaitForSingleObject(IOReady, INFINITE);
			
			ret = GetOverlappedResult(Clientes[i].hPipe, &Ov, &nBytes, FALSE);

			/*if (!ret || !nBytes) {
				_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
				DisconnectNamedPipe(Clientes[i].hPipe);	//fechar os recursos associados ao cliente que se desligou
				CloseHandle(Clientes[i].hPipe);
				CloseHandle(IOReady);
				//WaitForSingleObject(hMutex, INFINITE);
				Clientes[i].hPipe = INVALID_HANDLE_VALUE;
				//////ReleaseMutex(hMutex);
			}*/
		}
	}

	//_tprintf(TEXT("\n\nuser ID: %d"), resposta->Cliente_id);
	//_tprintf(TEXT("\nuser Name: %s"), resposta->dataPacket.nome);
}
//funcao que vai iniciar o ambiente de jogo apos o cliente dizer que quer iniciar o jogo
void iniciaThreadJogo() {

	Packet *resposta;

	do {

		resposta = LerBufferTabuleiro();

		if (resposta != NULL) {
		
			resposta->tipo = AtualizacaoJogo;
		
			EnviaBroadcastPacote(resposta);
		}

		Sleep(40); //25frames

	} while (dadosGw.ServerUp == 1);
}
// Funcao que le Pacotes do buffer servidor --> Gateway
void LePacotesBufferServtoGw() {

	packet *Resposta;

	HANDLE hThredEnviaTabuleiro;

	DWORD idThreadEnviaTabuleiro;

	do {
	
		Resposta = LerBufferServtoGw();
		// se a opçao do jogador for jogar individual irá lançar a thread que irá enviar lhe o jogo atualizado;
		//falta por a versao de iniciar jogo Multiplayer aqui
		if (Resposta->tipo == IniciaJogoIndividual ) {

			hThredEnviaTabuleiro = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)iniciaThreadJogo, (LPVOID)NULL, 0, &idThreadEnviaTabuleiro);

			if (hThredEnviaTabuleiro == NULL) {
			
				_tprintf(TEXT("\n\nErro ao criar Thread que ira enviar o Jogo atualizado para os Clientes"));
				
				exit(-1);
			}
		}
		switch (Resposta->tipo) {

		default:

			EnviaRespostaParaCliente(Resposta);
		
		}
		
	} while (dadosGw.ServerUp);

	WaitForSingleObject(&idThreadEnviaTabuleiro, INFINITE);

}
//inicia Comunicaçao com CLiente
int criaComunicacaoClienteGateway() {

	dadosGw.nClientes = 0;
	HANDLE hPipe;
	HANDLE hThreads[21];
	DWORD idThLeServToGw;
	int pos;
	

	for (int i = 0; i < 21; i++) {
		hThreads[i] = INVALID_HANDLE_VALUE;
	}

	hThreads[20] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LePacotesBufferServtoGw, (LPVOID)NULL, 0, &idThLeServToGw);

	do {
	
		hPipe = criaNamedPipe();							// cria named pipe ou instancia chega Cliente

		if (hPipe != NULL) {

			
			Clientes[dadosGw.nClientes].hPipe = hPipe;			//adiciona  a instancia para o novo cliente

			Clientes[dadosGw.nClientes].thAlive = 1;				/// poe a variavel da thread para ficar ativa
	
			Clientes[dadosGw.nClientes].id = startIdCli +=  1;

			pos = dadosGw.nClientes;

			hThreads[dadosGw.nClientes] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecebePipeCliente, (LPVOID)&Clientes[dadosGw.nClientes], 0, &Clientes[dadosGw.nClientes].iDThread);  //rever indice
			
		}
		//Sleep(10); // Sleep para obrigar a esperar que a thread se crie antes de incrementar o indice;

		dadosGw.nClientes++;

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

	CriaMemoriaPartilhada();			// cria memoria partilhada e Mecanismos de sincronizaçao

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