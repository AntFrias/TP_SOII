#include "HeaderCliente.h"

EstruturaCli Cliente;

void Envia(LPVOID NUUL) {

	DWORD  nBytesLidos;
	BOOL ret;
	packet PacoteEnvio;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov; // Extrutura para o so interpretar


	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (IOReady == NULL) {
		_tprintf(TEXT("Erro ao criar evento\n"));
	}

	do {

		//prenche pacote ;-)
		_tprintf(TEXT("Introduza o seu nome \n"));
		_fgetts(Cliente.nome, 10, stdin);
		PacoteEnvio.tipo = 1;
		wcscpy_s(PacoteEnvio.dataPacket.nome, Cliente.nome);
		
		//inicializa coisas para o named pipe
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;
		
		//escrever no pipe
		WriteFile(Cliente.pipe, &PacoteEnvio,sizeof(packet), &nBytesLidos, &Ov);
		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE);

		if (!ret || !nBytesLidos) {
			_tprintf(TEXT("Nao escrevi nada\n"), ret, nBytesLidos);
			break;
		}
		_tprintf(TEXT("[CLIENTE] Enviei %d bytes ao GATEWAY ...(WriteFile)\n"), nBytesLidos);
		

		
	} while (Cliente.alive);


}


int _tmain(int argc, LPTSTR argv[]) {


#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	
	packet PacoteRecebido;
	DWORD  nBytesLidos;
	BOOL ret;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov; // Extrutura para o so interpretar

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL); // retorno do evento handle

	if (IOReady == NULL) {
		_tprintf(TEXT("Erro ao criar evento\n"));
		exit(-1);
	}
									

	//ler do pipe
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
	_tprintf(TEXT(" Não consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
	exit(-1);
	}

	Cliente.alive = 1;
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);

	if (Cliente.pipe == NULL) {
	_tprintf(TEXT("Não consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
	exit(-1);
	}

	//escrevo no pipe
	Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Envia, (LPVOID)NULL, 0, &Cliente.IDth);


	while (1) {

		ZeroMemory(&Ov, sizeof(Ov)); //limpar a estrutura Ov
		ResetEvent(IOReady); // para ter que o evento esta desligado 
		Ov.hEvent = IOReady; // associar handle do evento para o overlap saber

		ReadFile(Cliente.pipe, &PacoteRecebido, sizeof(packet), &nBytesLidos, &Ov); //le o pacote do named pipe

		WaitForSingleObject(IOReady, INFINITE); // espera pelo evento

		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE); // se mal dá 0 !!!

		if (!ret || !nBytesLidos) {
			_tprintf(TEXT("Nao li nada\n"), ret, nBytesLidos);
			break;
		}
		_tprintf(TEXT("Recebi este nome %s\n"),PacoteRecebido.dataPacket.nome);
	}

	CloseHandle(Cliente.pipe);	// fecha pipe do cliente
	CloseHandle(IOReady);		// fecha handle dop cliente 

	WaitForSingleObject(Cliente.ht, INFINITE); //espera pela thread de cima


	return 0;
}