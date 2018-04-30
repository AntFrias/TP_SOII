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

// Extrutura com os dados do servidor;
typedef struct Gestao_servidor{
	int NumNavesInvasoras;
	int NumClients;
	int NumJogadores;
	DWORD ComGatewaythreadId;
	HANDLE ComGateway;
	BOOL inicioJogo;
	HANDLE LerMemoriaMutex;
	

}Gestor;
// extrutura das naves invasoras
typedef struct NavesInvasoras {
	DWORD NaveInvthreadId;																				
	HANDLE NavesInv;
	int tipoNave;
	int Alive;

}navesInvasoras;
// ideia da primeira extrutura com dados para por na DLL acerca dos semaforos entre o servidor e o gateway para lerem o que está no 
// vector circular
typedef struct DadosMemoriaPartilhada {

	HANDLE LerMemoriaMutex;
	HANDLE EscreveMemoriaMutex;

}MemoriaPartilhada;

// extrutura TOP10
typedef struct Top10 {
	TCHAR name[10];
	int	pontuacao;
}top10;







