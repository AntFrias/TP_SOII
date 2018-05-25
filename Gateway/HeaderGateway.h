
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
#define MaxClientes 5

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

}dataGw;

typedef struct Thread {

	DWORD ThreadID;
	unsigned int ID_CLIENTE;
	HANDLE hThread;
	unsigned int Alive;

}ThreadCliente;

typedef struct Clientes {
	
	unsigned int id;
	TCHAR nome[10];
	HANDLE hPipe;

}clientes;


