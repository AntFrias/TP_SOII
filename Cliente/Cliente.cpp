#include "HeaderCliente.h"
#include  "resource.h"

EstruturaCli Cliente;
configur configuracoes;
alteracaoTab tabAux[5];
bipm bipMaps;


HWND hwndPrincipal;  // janela principal

void TrataPacote(packet pacoteTratar) {


	switch (pacoteTratar.tipo) {
			
			case user_login_sucesso: { // Se for bem logado recebe a mensagem que vai começar pelo jogo meter depois no servidor -> user_login_sucesso e aqui tb

				MessageBox(NULL, TEXT("Login efetuado com sucesso!\nEspere que o Jogo comece "), TEXT("AGUARDE"), MB_OK | MB_ICONINFORMATION);
				
				break;
			}
			
			case user_Login_falhou: {

				MessageBox(NULL, TEXT("Ja se encontra um jogador com o mesmo nome"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
				exit(0);
				break;
			}

			case max_players_atingido: {

				MessageBox(NULL, TEXT("O servidor está cheio.\nTente mais tarde"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
				exit(0);
				break;
			}
		
		default:
			return;
	}

}

void Envia(packet PacoteEnvio) {

	DWORD  nBytesLidos;
	BOOL ret;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov = { 0 }; // Extrutura para o so interpretar

	
		IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (IOReady == NULL) {
			//_tprintf(TEXT("Erro ao criar evento\n"));
		}

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
		MessageBox(NULL, TEXT("Provavelmente o Gateway não está ligado"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
		exit(-1);
		
	}

	Cliente.alive = 1;
	Cliente.pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0 | FILE_FLAG_OVERLAPPED, NULL);

	if (Cliente.pipe == NULL) {
		MessageBox(NULL, TEXT("O Pipe do Cliente é invalido"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
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
			MessageBox(NULL, TEXT("Não consigui ver nada"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
			//_tprintf(TEXT("Nao li nada\n"), ret, nBytesLidos);
			break;
		}
		//_tprintf(TEXT("\n\nRecebi este nome %s\n"), PacoteRecebido.dataPacket.nome);
		
		TrataPacote(PacoteRecebido);

	}

	CloseHandle(Cliente.pipe);	// fecha pipe do cliente
	CloseHandle(IOReady);		// fecha handle dop cliente 

}


void IniciaCliente() {


	//thread escuta no pipe
	Cliente.ht = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escuta, (LPVOID)NULL, 0, &Cliente.IDth);
	Cliente.EventJogar = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("PodeJogar")  // object name
	);



}


void carregaBitMaps() {


	bipMaps.Wallpaper = (HBITMAP)LoadImage(NULL, L"../../Imagens/imagemInicial.bmp", IMAGE_BITMAP, 800, 800, LR_LOADFROMFILE); 
	bipMaps.Basica = (HBITMAP)LoadImage(NULL, L"../../Imagens/Basica.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.Bomba = (HBITMAP)LoadImage(NULL, L"../../Imagens/Bomba.bmp", IMAGE_BITMAP, 20, 40, LR_LOADFROMFILE);
	bipMaps.Defensora1 = (HBITMAP)LoadImage(NULL, L"../../Imagens/Defensora1.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.Defensora2 = (HBITMAP)LoadImage(NULL, L"../../Imagens/Defensora2.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.Esquiva = (HBITMAP)LoadImage(NULL, L"../../Imagens/Esquiva.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
	bipMaps.Tiro = (HBITMAP)LoadImage(NULL, L"../../Imagens/tiro.bmp", IMAGE_BITMAP, 20, 40, LR_LOADFROMFILE);
	bipMaps.Boss = (HBITMAP)LoadImage(NULL, L"../../Imagens/boss.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);

}

LRESULT CALLBACK Configuracoes(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	//case WM_DESTROY:
	//	PostQuitMessage(0);
	//	return 0;
	/*
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		break;
	}
	*/
	case WM_INITDIALOG: 
		SetWindowText(GetDlgItem(hwnd, IDC_Nome), TEXT("Joao"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), TEXT("A"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_CIMA), TEXT("W"));		// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_BAIXO), TEXT("S"));		// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_DIREITA), TEXT("D"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_tiro), TEXT("V"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP1), TEXT("Z"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP2), TEXT("X"));	// por texto por default
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP3), TEXT("C"));	// por texto por default
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			packet PacoteLogin;
			//NOME
			TCHAR buff[11];
			GetWindowText(GetDlgItem(hwnd, IDC_Nome), buff, 10);
			wcscpy_s(configuracoes.nome, buff);
			
			//MessageBox(NULL,configuracoes.nome,TEXT("exemplo"),MB_OK | MB_ICONERROR); //para ver se está a buscar o nome bem
			
			//Teclas
			TCHAR aux[2];

			GetDlgItemText(hwnd, IDC_CIMA, aux, 2);
			//GetWindowText(GetDlgItem(hwnd, IDC_Nome), aux, 1);
			configuracoes.CIMA = aux[0];//TCHAR ('w');//aux[0];
			//aux[0] = 'w';
			aux[1] = TEXT('\0');
			//MessageBox(NULL, aux, TEXT("LETRA"), MB_OK);
			

			GetDlgItemText(hwnd, IDC_ESQUERDA, aux,1);
			configuracoes.ESQUERDA = aux[0];

			GetDlgItemText(hwnd, IDC_BAIXO, aux, 1);
			configuracoes.BAIXO = aux[0];

			GetDlgItemText(hwnd, IDC_DIREITA, aux, 1);
			configuracoes.DIREITA = aux[0];

			GetDlgItemText(hwnd, IDC_tiro, aux, 1);
			configuracoes.TIRO = aux[0];

			//PowerUps
			GetDlgItemText(hwnd, IDC_POWERUP1, aux, 1);
			configuracoes.POWERUP1 = aux[0];

			GetDlgItemText(hwnd, IDC_POWERUP2, aux, 1);
			configuracoes.POWERUP2 = aux[0];

			GetDlgItemText(hwnd, IDC_POWERUP3, aux, 1);
			configuracoes.POWERUP3 = aux[0];
			//NESTA ALTURA JA TENHO AS CONFIGURAÇÕES DENTRO DA ESTRUTURA configuracoes
			

			//TODO prencher pacote aqui e fazer com que a janela nao feche
			//até que o servidor responda que o login esteja correto e o jogo pronto
			
			//prenche pacote ;-)
			PacoteLogin.tipo = user_login;
			wcscpy_s(PacoteLogin.dataPacket.nome, configuracoes.nome);
			
			Envia(PacoteLogin); ///  com reservas

			EndDialog(hwnd, 0); //isto é para fechar a janela 
			//PostQuitMessage(0); //isto é para fechar a janxxxx -> o ciclo de mensagens
			ShowWindow(hwndPrincipal, SW_SHOWDEFAULT);
			UpdateWindow(hwndPrincipal);

			break;
		}
		break;
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

	/////////////TESTE
	HDC hDC, MemDCExercising;
	PAINTSTRUCT Ps;
	HBITMAP bmpExercising;

	////////////

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		hDC = BeginPaint(hwnd, &Ps);

		// Load the bitmap from the resource
		//ja carreguei antes
		//bmpExercising = (HBITMAP)LoadImage(NULL, L"../../Imagens/imagemInicial.bmp", IMAGE_BITMAP, 800, 800, LR_LOADFROMFILE);
		
		// Create a memory device compatible with the above DC variable
		MemDCExercising = CreateCompatibleDC(hDC);
		
		// Select the new bitmap
		//SelectObject(MemDCExercising, bmpExercising);

		// Copy the bits from the memory DC into the current dc
		//BitBlt(hDC, 0, 0, 800, 800, MemDCExercising, 0, 0, SRCCOPY);

		// Restore the old bitmap
		//DeleteDC(MemDCExercising);
		//DeleteObject(bmpExercising);
		EndPaint(hwnd, &Ps);
		break;
	}
	case WM_CHAR: {
		packet enviaTecla;

		if ((TCHAR)wParam == configuracoes.CIMA) {
			exit(0);
		}

		/*switch (wParam) {
		
		case (TCHAR)configuracoes.CIMA: {
			
			enviaTecla.tipo = 20 ;//alterar aqui
			enviaTecla.dataPacket.movimento = cima;
			
			Envia(enviaTecla);
			
			break;
		}
		case configuracoes.BAIXO: {

			enviaTecla.tipo = 20 ;//altear aqui
			enviaTecla.dataPacket.movimento = baixo;

			Envia(enviaTecla);
			break;
		}
		case configuracoes.ESQUERDA: {

			enviaTecla.tipo = 20; //alterar aqui
			enviaTecla.dataPacket.movimento = esquerda;

			Envia(enviaTecla);
			break;
		}
		case configuracoes.DIREITA: {

			enviaTecla.tipo = 20; //alterar aqui
			enviaTecla.dataPacket.movimento = cima;

			Envia(enviaTecla);
			break;
		}
		case configuracoes.TIRO: {

			enviaTecla.tipo = 20 
			enviaTecla.dataPacket.movimento = espaco; // alterar isto com o frias

			Envia(enviaTecla);
			break;
		}
		case configuracoes.POWERUP1: {

			enviaTecla.tipo = 20;
			enviaTecla.dataPacket.movimento = z;

			Envia(enviaTecla);
			break;
		}
		case configuracoes.POWERUP2: {

			enviaTecla.tipo = 20 ;
			enviaTecla.dataPacket.movimento = x;

			Envia(enviaTecla);
			break;
		}
		case configuracoes.POWERUP3: {

			enviaTecla.tipo = 20 ;
			enviaTecla.dataPacket.movimento = c;

			Envia(enviaTecla);
			break;
		}
		

		default:
			//nao faz nada se não houver correspondencia

		}*/

		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR LpCmdLine, int ncmdshow)
{//o professor colocou assim _tWinMain




	MessageBox(NULL,TEXT("jgjgjgjgj"),TEXT("LETRA"),MB_OK);

	tabAux[0].tipo = NaveBasica;
	tabAux[0].x = 0;
	tabAux[0].y = 0;

	tabAux[1].tipo = tiroJogador;
	tabAux[1].x = 10;
	tabAux[1].y = 4;

	tabAux[4].tipo = NaveJogador;
	tabAux[4].x = 4;
	tabAux[4].y = 12;

	tabAux[2].tipo = NavesEsquiva;
	tabAux[2].x = 12;
	tabAux[2].y = 23;

	tabAux[3].tipo = NaveBasica;
	tabAux[3].x = 2;
	tabAux[3].y = 0;

	IniciaCliente(); // lança a thread que escuta no named pipe
	
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Janela Principal";

	WNDCLASS wc = {};

	///////////////////////////////////////Configuraçoes//////////////////////////////////////////////////////////////
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, Configuracoes);
	ShowWindow(hDlg, ncmdshow);



	MSG msg = {};
	
	/*while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	carregaBitMaps();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = (HICON)LoadImage(NULL, L"../../Imagens/Logot.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	wc.hCursor =(HCURSOR)LoadImage(NULL, L"../../Imagens/Cursor.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	RegisterClass(&wc);
	
	// Create the window.
	hwndPrincipal = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L" -> Space Invader <- ",      // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

										// Size and position
		0, 0, 800, 800,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwndPrincipal == NULL)
	{
		return 0;
	}

	//ShowWindow(hwnd, ncmdshow);
	//UpdateWindow(hwnd);
	
	
	// para estar sempre em loop
	//msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	//espera pela thread que le
	//WaitForSingleObject(Cliente.ht, INFINITE);

	return 0;
}


/*
TCHAR yo[512];

_stprintf_s(yo, 512, TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
OutputDebugString(yo);
*/