#include "Servidor.h"

// vai fazer a gestao de todas as naves inimigas
int GestorNavesInimigas(LPVOID navesInimigas) {

	navesInvasoras *naveInimiga;

	naveInimiga = ((NavesInvasoras*) navesInimigas);

	return 0;
}//.(/
// vai preparar o Ambiente do Jogo
int InicioJogo( int NumNavesInvasoras) {

	navesInvasoras *NavesInimigas;

	NavesInimigas = (NavesInvasoras*)malloc(sizeof(NavesInvasoras) * NumNavesInvasoras);

	if (NavesInimigas == NULL) {

		_tprintf(TEXT("\nErro ao criar Threads para Naves Inimigas\n"));
		return -1;
	}

	for (int i = 0; i < NumNavesInvasoras; i++) {

		NavesInimigas[i].NavesInv = CreateThread(NULL,
													0,    
													(LPTHREAD_START_ROUTINE)GestorNavesInimigas,
													(LPVOID)&NavesInimigas[i],
													0,
													&NavesInimigas[i].NaveInvthreadId);

		if (NavesInimigas[i].NavesInv == NULL) {

			_tprintf(TEXT("\n\nErro ao Iniciar Thread para Naves Inimigas\n\n"));
			
			return -1;
		
		}

		_tprintf(TEXT("\nCriei Thread %d\n"), i);

	}

}
// vai lançar a Thread que vai ficar a comunicar com o Gateway

int comunicaoGateway(LPVOID partilha /*vai ficar aqui o contacto com a DLL*/) {

	MemoriaPartilhada *Partilha;

	Partilha = (MemoriaPartilhada *)partilha;
	
	return 0;
}
// inicia os serviços e a configuraçao do Servidor;

int InicioServidor() {

	TCHAR c;
	
	Gestor optionServidor;
	
	MemoriaPartilhada Partilha;

	_tprintf(TEXT("\n\nOlá Eu sou o Servidor e estou a inicar\n\n"));

	_tprintf(TEXT("\n\n Iniciar Thread Para Tratar os pedidos do Gateway\n\n"));

	optionServidor.ComGateway = CreateThread( NULL,
												 0,
												(LPTHREAD_START_ROUTINE)comunicaoGateway,
												(LPVOID)&Partilha,
												0,
												&optionServidor.ComGatewaythreadId);
												

		_tprintf(TEXT(" 2.iniciar jogo ? "));

	_tscanf_s(TEXT("%c"), &c, 1);
	
	if (c == 's' || c == 'S') {

		optionServidor.NumNavesInvasoras = 10;

		InicioJogo(optionServidor.NumNavesInvasoras);
	}
	
	return 0;
}

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	

	InicioServidor();

	return 0;
	
}