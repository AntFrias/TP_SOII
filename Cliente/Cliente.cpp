#include "HeaderCliente.h"



EstruturaCli Cliente;



void Envia(LPVOID NUUL) {

	DWORD n;
	packet PacoteEnvio;
	
	Cliente.pipe = CreateFile(PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (Cliente.pipe == INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
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
	
	
	Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Envia, (LPVOID)NULL, 0, &Cliente.IDth);


	WaitForSingleObject(Cliente.ht, INFINITE);




	return 0;
}