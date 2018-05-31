#include "HeaderCliente.h"

EstruturaCli Cliente;

void Envia(LPVOID NUUL) {

	DWORD n;
	packet PacoteEnvio;
	
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (Cliente.pipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
		Sleep(10000);
		exit(-1);
	}

	do {

		_tprintf(TEXT("Introduza o seu nome \n"));
		_fgetts(Cliente.nome, 10, stdin);

		PacoteEnvio.tipo = 1;
		wcscpy_s(PacoteEnvio.dataPacket.nome, Cliente.nome);

		if (!WriteFile(Cliente.pipe, &PacoteEnvio, sizeof(Packet), &n, NULL)) {
			_tprintf(TEXT("Mandei um pacote! %d de tamanho \n"), n);
		}

		
	} while (_tcscmp(Cliente.nome, TEXT("fim")));


}


int _tmain(int argc, LPTSTR argv[]) {


#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	
	packet PacoteRecebido;
	DWORD  nBytesLidos;
	BOOL ret;


	//escrevo no pipe
	Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Envia, (LPVOID)NULL, 0, &Cliente.IDth);






	//ler do pipe
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT(" Não consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
		exit(-1);
	}
	
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (Cliente.pipe == NULL) {
		_tprintf(TEXT("Não consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
		exit(-1);
	}


	while (1) {
		ret = ReadFile(Cliente.pipe, &PacoteRecebido, sizeof(packet), &nBytesLidos, NULL);

		if (!ret || !nBytesLidos) { //n sei se este if é assim 
			_tprintf(TEXT("Li mal o pacote\n"));
			
		}

		if (ret != 0 && nBytesLidos > 0) { //n sei se este if é assim
			_tprintf(TEXT("Li bem o pacote\n"));
			//chama função trata pacote cli
		}
	

		
	}

	CloseHandle(Cliente.pipe);

	WaitForSingleObject(Cliente.ht, INFINITE);


	return 0;
}