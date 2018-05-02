
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

typedef struct MemoriaPartilhada {

	HANDLE ptrGWtoServer;
	HANDLE ptrServertoGW;
	HANDLE ptrMapJogo;


}M;