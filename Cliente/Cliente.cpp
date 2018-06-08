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
		WriteFile(Cliente.pipe, &PacoteEnvio, sizeof(packet), &nBytesLidos, &Ov);

		WaitForSingleObject(IOReady, INFINITE);

		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE);

		if (!ret || !nBytesLidos) {
			_tprintf(TEXT("Nao escrevi nada\n"), ret, nBytesLidos);
			break;
		}
		_tprintf(TEXT("[CLIENTE] Enviei %d bytes ao GATEWAY ...(WriteFile)\n"), nBytesLidos);



	} while (Cliente.alive);


}

void escuta() {

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
		_tprintf(TEXT(" N?o consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
		exit(-1);
	}

	Cliente.alive = 1;
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);

	if (Cliente.pipe == NULL) {
		_tprintf(TEXT("N?o consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
		exit(-1);
	}

	// mudar para modo message
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	int fres;
	fres = SetNamedPipeHandleState(Cliente.pipe, &dwMode, NULL, NULL);
	if (!fres)
		_tprintf(TEXT("ERRO  a  mudar para message mode"));

	while (1) {

		ZeroMemory(&Ov, sizeof(Ov)); //limpar a estrutura Ov
		ResetEvent(IOReady); // para ter que o evento esta desligado 
		Ov.hEvent = IOReady; // associar handle do evento para o overlap saber

		ReadFile(Cliente.pipe, &PacoteRecebido, sizeof(packet), &nBytesLidos, &Ov); //le o pacote do named pipe

		WaitForSingleObject(IOReady, INFINITE); // espera pelo evento

		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE); // se mal d? 0 !!!

		if (!ret || !nBytesLidos) {
			_tprintf(TEXT("Nao li nada\n"), ret, nBytesLidos);
			break;
		}
		_tprintf(TEXT("\n\nRecebi este nome %s\n"), PacoteRecebido.dataPacket.nome);
	}

	CloseHandle(Cliente.pipe);	// fecha pipe do cliente
	CloseHandle(IOReady);		// fecha handle dop cliente 




}


void IniciaCliente() {


	//thread escrevo no pipe
	//Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Envia, (LPVOID)NULL, 0, &Cliente.IDth);

	//le do pipe
	//escuta();

	//espera pela thread que escreve
	//WaitForSingleObject(Cliente.ht, INFINITE);




}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int ncmdshow)
{

#ifdef _UNICODE
#define _tWinMain wWinMain
#else
#define _tWinMain WinMain
#endif


	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Janela Principal";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = (HICON)LoadImage(NULL, L"../../Imagens/Logot.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L" -> Space Invader <- ",      // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

										// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, ncmdshow);

	// Run the message loop.

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//IniciaCliente();
	return 0;
}