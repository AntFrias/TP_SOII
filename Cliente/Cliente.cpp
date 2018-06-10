#include "HeaderCliente.h"
#include  "resource.h"

EstruturaCli Cliente;
configur configuracoes;


void Envia() {

	DWORD  nBytesLidos;
	BOOL ret;
	packet PacoteEnvio;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov = { 0 }; // Extrutura para o so interpretar

	
		IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (IOReady == NULL) {
			//_tprintf(TEXT("Erro ao criar evento\n"));
		}

		//prenche pacote ;-)
		PacoteEnvio.tipo = 1;
		wcscpy_s(PacoteEnvio.dataPacket.nome, configuracoes.nome);

		//inicializa coisas para o named pipe
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;

		//escrever no pipe
		WriteFile(Cliente.pipe, &PacoteEnvio, sizeof(packet), &nBytesLidos, &Ov);
	
		WaitForSingleObject(IOReady, INFINITE);

		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE);

		if (!ret || !nBytesLidos) {
			
			//_tprintf(TEXT("Nao escrevi nada\n"), ret, nBytesLidos);
			//return -1;
			OutputDebugString(TEXT("ret ou nbyres = 0!!!\n"));
		}
		//_tprintf(TEXT("[CLIENTE] Enviei %d bytes ao GATEWAY ...(WriteFile)\n"), nBytesLidos);
		OutputDebugString(TEXT("Passei o enviei1!!!\n"));


}

void escuta() {

	packet PacoteRecebido;
	DWORD  nBytesLidos;
	BOOL ret;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov; // Extrutura para o so interpretar

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL); // retorno do evento handle

	if (IOReady == NULL) {
	//	_tprintf(TEXT("Erro ao criar evento\n"));
		exit(-1);
	}


	//ler do pipe
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		//_tprintf(TEXT(" N?o consegui ligar ao pipe '%s'!\n"), PIPE_NAME);
		exit(-1);
	}

	Cliente.alive = 1;
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);

	if (Cliente.pipe == NULL) {
		exit(-1);
	}

	// mudar para modo message
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	int fres;
	fres = SetNamedPipeHandleState(Cliente.pipe, &dwMode, NULL, NULL);
	//if (!fres)
		//_tprintf(TEXT("ERRO  a  mudar para message mode"));

	while (1) {

		ZeroMemory(&Ov, sizeof(Ov)); //limpar a estrutura Ov
		ResetEvent(IOReady); // para ter que o evento esta desligado 
		Ov.hEvent = IOReady; // associar handle do evento para o overlap saber

		ReadFile(Cliente.pipe, &PacoteRecebido, sizeof(packet), &nBytesLidos, &Ov); //le o pacote do named pipe
		
		WaitForSingleObject(IOReady, INFINITE); // espera pelo evento

		ret = GetOverlappedResult(Cliente.pipe, &Ov, &nBytesLidos, FALSE); // se mal d? 0 !!!
		
		if (!ret || !nBytesLidos) {
			//_tprintf(TEXT("Nao li nada\n"), ret, nBytesLidos);
			break;
		}
		//_tprintf(TEXT("\n\nRecebi este nome %s\n"), PacoteRecebido.dataPacket.nome);
		
	
		MessageBox(NULL,PacoteRecebido.dataPacket.nome,TEXT("Recebi isto"),MB_OK | MB_ICONINFORMATION); //para ver se está a buscar o nome bem

		
	}

	CloseHandle(Cliente.pipe);	// fecha pipe do cliente
	CloseHandle(IOReady);		// fecha handle dop cliente 

}


void IniciaCliente() {


	//thread escuta no pipe
	Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escuta, (LPVOID)NULL, 0, &Cliente.IDth);


}

LRESULT CALLBACK Configuracoes(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		break;
	}
	case WM_INITDIALOG: 
	{
		SetWindowText(GetDlgItem(hwnd, IDC_Nome), TEXT("Joao"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), TEXT("A"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_CIMA), TEXT("W"));		// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_BAIXO), TEXT("S"));		// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_DIREITA), TEXT("D"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP1), TEXT("Z"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP2), TEXT("X"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP3), TEXT("V"));	// por texto por default
		
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			//NOME
			TCHAR buff[11];
			GetWindowText(GetDlgItem(hwnd, IDC_Nome), buff, 10);
			wcscpy_s(configuracoes.nome, buff);
			
			//MessageBox(NULL,configuracoes.nome,TEXT("exemplo"),MB_OK | MB_ICONERROR); //para ver se está a buscar o nome bem
			
			//Teclas
			TCHAR aux[1];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux,1);
			configuracoes.ESQUERDA = aux[0];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.CIMA = aux[0];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.BAIXO = aux[0];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.DIREITA = aux[0];

			//PowerUps
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.POWERUP1 = aux[0];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.POWERUP2 = aux[0];
			GetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), aux, 1);
			configuracoes.POWERUP3 = aux[0];
			//NESTA ALTURA JA TENHO AS CONFIGURAÇÕES DENTRO DA ESTRUTURA configuracoes
			

			//TODO prencher pacote aqui e fazer com que a janela nao feche
			//até que o servidor responda que o login esteja correto e o jogo pronto
			
			
			Envia();
			
			//EndDialog(hwnd, 0); //isto é para fechar a janela 
			//PostQuitMessage(0); //isto é para fechar a janela
			break;
		}
		
		case IDCANCEL:
		
			exit(0);
		
		break;
		}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	

	return 0;
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
		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR LpCmdLine, int ncmdshow)
{

#ifdef _UNICODE
#define _tWinMain wWinMain
#else
#define _tWinMain WinMain
#endif

	IniciaCliente(); // lança a thread que escuta no named pipe
	
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Janela Principal";

	WNDCLASS wc = {};

	///////////////////////////////////////Configuraçoes//////////////////////////////////////////////////////////////
	HWND hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, Configuracoes, 0);
	ShowWindow(hDlg, ncmdshow);
	
	MSG msg = {};
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = (HICON)LoadImage(NULL, L"../../Imagens/Logot.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	wc.hCursor =(HCURSOR)LoadImage(NULL, L"../../Imagens/Cursor.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
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
	UpdateWindow(hwnd);
	
	
	// para estar sempre em loop
	msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	//espera pela thread que le
	WaitForSingleObject(Cliente.ht, INFINITE);
	
	return 0;
}


/*
TCHAR yo[512];

_stprintf_s(yo, 512, TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
OutputDebugString(yo);
*/