#include "HeaderGateway.h"


int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE  //UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif	
	 
	M teste;

	_tprintf(TEXT("\n\n Inicializaçao do Gateway\n\n"));

	Sleep(90000);
	return 0;
}