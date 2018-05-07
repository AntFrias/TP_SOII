
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


//prototipos

void IniciarGateway();

//Estruturas



typedef struct Gestao_gateway {


	int sessionID;
	HANDLE hThreadGwtoSer;
	DWORD idThreadGwtoSer;

}dataGw;
