
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>
#include <shellapi.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

//Inicio dos IDs do Cliente
int startIdCli = 1000;

//prototipos

void IniciarGateway();

//Estruturas

typedef struct Gestao_gateway {

	int sessionID;
	HANDLE hThreadGwtoSer;
	HANDLE *ArrayThreadClientes;
	DWORD idThreadGwtoSer;
	int nClientes;
	unsigned int ServerUp;
	HANDLE EventoIniciaJogo;


}dataGw;

typedef struct ArrayClientes {
	
	int id;
	HANDLE hPipe;
	DWORD iDThread;
	unsigned int thAlive;

}clientes;





