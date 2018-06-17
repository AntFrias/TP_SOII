#include "HeaderCliente.h"
#include  "resource.h"

EstruturaCli Cliente;
configur configuracoes;
alteracaoTab tabAux[8];
bipm bipMaps;
packet PacoteEnvio;
hdcImg hdcDasImg;
HDC janelaImprime;
HDC janelaAux;

HWND hwndPrincipal;  // janela principal

void LimpaPacotedEnvio() {

	PacoteEnvio.Cliente_id = 999;
	PacoteEnvio.numObjetos = 999;
	PacoteEnvio.pontuacao = 999;
	PacoteEnvio.tipo = 999;

}
void TrataPacote(packet pacoteTratar) {


	switch (pacoteTratar.tipo) {
			
			case user_login_sucesso: { // Se for bem logado recebe a mensagem que vai começar pelo jogo meter depois no servidor -> user_login_sucesso e aqui tb

				MessageBox(NULL, TEXT("Login efetuado com sucesso!\nEspere que o Jogo comece "), TEXT("AGUARDE"), MB_OK | MB_ICONINFORMATION);
				MessageBox(NULL, TEXT("Deseja iniciar o Jogo?"), TEXT("AGUARDE"), MB_OK | MB_ICONINFORMATION);
				PacoteEnvio.tipo = IniciaJogoMultiplayer;
				SetEvent(Cliente.EventEnvia);
				break;
			}
			
			case user_Login_falhou: {

				//MessageBox(NULL, TEXT("Ja se encontra um jogador com o mesmo nome"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
				//exit(0);
				break;
			}

			case user_login_Limite_clientes: {

				MessageBox(NULL, TEXT("O servidor está cheio.\nTente mais tarde"), TEXT("ERRO"), MB_OK | MB_ICONERROR);
				//exit(0);
				break;
			}
		
			case AtualizacaoJogo: {
				BitBlt(janelaAux, 0, 0, 800, 800, hdcDasImg.Space, 0, 0, SRCCOPY);
				for (int i = 0; i < pacoteTratar.numObjetos; i++) {
					
					switch (pacoteTratar.dataPacket.arrayTab[i].tipo) {

						case NaveBasica: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 40, 40, hdcDasImg.Basica, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case NavesEsquiva: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 40, 40, hdcDasImg.Esquiva, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case NaveBoss: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 40, 40, hdcDasImg.Boss, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case NaveJogador: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 40, 40, hdcDasImg.Defensora1, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case tiroJogador: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 15, 40, hdcDasImg.Tiro, 0, 0, 20, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case bombaInimiga: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 15, 40, hdcDasImg.Bomba, 0, 0, 20, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						case PowerUpEscudo: {
							TransparentBlt(janelaAux, (pacoteTratar.dataPacket.arrayTab[i].x * 20), (pacoteTratar.dataPacket.arrayTab[i].y * 20), 40, 40, hdcDasImg.Presente, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
							break;
						}
						default:
							break;
							
					}
						
				}
				InvalidateRect(hwndPrincipal, NULL, TRUE);

			}
		
		default:
			return;
	}

}

void Envia() {

	DWORD  nBytesLidos;
	BOOL ret;
	HANDLE IOReady; //handle para o evento
	OVERLAPPED Ov = { 0 }; // Extrutura para o so interpretar

	
		IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (IOReady == NULL) {
			//_tprintf(TEXT("Erro ao criar evento\n"));
		}

		while (1) {
			WaitForSingleObject(Cliente.EventEnvia,INFINITE);

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
			ResetEvent(Cliente.EventEnvia);
		}
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
	Cliente.htEscuta = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)escuta, (LPVOID)NULL, 0, &Cliente.IDEscuta);

	Cliente.htEnvia = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)Envia, (LPVOID)NULL, 0, &Cliente.IDEnvia);
	Cliente.EventEnvia = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL // object name
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
	bipMaps.Space = (HBITMAP)LoadImage(NULL, L"../../Imagens/space.bmp", IMAGE_BITMAP, 800, 800, LR_LOADFROMFILE);
	bipMaps.Presente = (HBITMAP)LoadImage(NULL, L"../../Imagens/presente.bmp", IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);

}

LRESULT CALLBACK Configuracoes(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: 
		//colocar o texto por default nas caixas
		SetWindowText(GetDlgItem(hwnd, IDC_Nome), TEXT("Joao"));	
		SetWindowText(GetDlgItem(hwnd, IDC_ESQUERDA), TEXT("A"));	
		SetWindowText(GetDlgItem(hwnd, IDC_CIMA), TEXT("W"));		
		SetWindowText(GetDlgItem(hwnd, IDC_BAIXO), TEXT("S"));		
		SetWindowText(GetDlgItem(hwnd, IDC_DIREITA), TEXT("D"));	
		SetWindowText(GetDlgItem(hwnd, IDC_tiro), TEXT("V"));	
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP1), TEXT("Z"));	
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP2), TEXT("X"));	
		SetWindowText(GetDlgItem(hwnd, IDC_POWERUP3), TEXT("C"));	
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{  
			TCHAR buff[9];
			GetWindowText(GetDlgItem(hwnd, IDC_Nome), buff, 9);
			wcscpy_s(configuracoes.nome, buff);
			
			//Teclas
			TCHAR aux[2];
			//nao esquecer ... o 2 é a letra mais o \n !!
			GetDlgItemText(hwnd, IDC_CIMA, aux, 2);
			configuracoes.CIMA = aux[0];
			aux[1] = TEXT('\0');
			
			GetDlgItemText(hwnd, IDC_ESQUERDA, aux,2);
			configuracoes.ESQUERDA = aux[0];
			aux[1] = TEXT('\0');

			GetDlgItemText(hwnd, IDC_BAIXO, aux, 2);
			configuracoes.BAIXO = aux[0];
			aux[1] = TEXT('\0');

			GetDlgItemText(hwnd, IDC_DIREITA, aux, 2);
			configuracoes.DIREITA = aux[0];
			aux[1] = TEXT('\0');

			GetDlgItemText(hwnd, IDC_tiro, aux, 2);
			configuracoes.TIRO = aux[0];
			aux[1] = TEXT('\0');

			//PowerUps
			GetDlgItemText(hwnd, IDC_POWERUP1, aux, 2);
			configuracoes.POWERUP1 = aux[0];
			aux[1] = TEXT('\0');

			GetDlgItemText(hwnd, IDC_POWERUP2, aux, 2);
			configuracoes.POWERUP2 = aux[0];
			aux[1] = TEXT('\0');

			GetDlgItemText(hwnd, IDC_POWERUP3, aux, 2);
			configuracoes.POWERUP3 = aux[0];
			aux[1] = TEXT('\0');
			
			//prenche pacote
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = user_login;
			wcscpy_s(PacoteEnvio.dataPacket.nome, configuracoes.nome);
			SetEvent(Cliente.EventEnvia);


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
	PAINTSTRUCT Ps;
	//HBITMAP bmpExercising;

	////////////

	switch (uMsg)	{

	case WM_DESTROY: {
		PostQuitMessage(0);

		//eliminar as janelas feitas para cada um bipmap ... (por mais aqui ... um delete para cada um)
		DeleteObject(hdcDasImg.Wallpaper);
		DeleteObject(hdcDasImg.Space);
		DeleteObject(hdcDasImg.Basica);
		DeleteObject(hdcDasImg.Esquiva);
		DeleteObject(hdcDasImg.Boss);
		DeleteObject(hdcDasImg.Defensora1);
		DeleteObject(hdcDasImg.Tiro);
		DeleteObject(hdcDasImg.Bomba);

		//eliminar o  ecra aux -> este é o que imprime 
		DeleteDC(janelaImprime); 
		DeleteDC(janelaAux);
		return 0;
	}
	case WM_PAINT:	{

		janelaImprime = BeginPaint(hwnd, &Ps);
		//copia do janela auxiliar para a janela a imprimir
		BitBlt(janelaImprime, 0, 0, 800, 800, janelaAux, 0, 0, SRCCOPY);
		
		EndPaint(hwnd, &Ps);
		
		break;
	}

	case WM_CREATE: {

		//prepara janela aux 
		janelaImprime = GetDC(hwndPrincipal);
		janelaAux = CreateCompatibleDC(janelaImprime);
		HBITMAP teste = CreateCompatibleBitmap(janelaImprime, 800, 800);
		SelectObject(janelaAux, teste);
		ReleaseDC(hwndPrincipal, janelaImprime);

		// Cria uma "janela"( memory device compatible ) para cada bipmap
		hdcDasImg.Wallpaper = CreateCompatibleDC(janelaAux);
		hdcDasImg.Space = CreateCompatibleDC(janelaAux);
		hdcDasImg.Basica = CreateCompatibleDC(janelaAux);
		hdcDasImg.Esquiva = CreateCompatibleDC(janelaAux);
		hdcDasImg.Boss = CreateCompatibleDC(janelaAux);
		hdcDasImg.Defensora1 = CreateCompatibleDC(janelaAux);
		hdcDasImg.Defensora2 = CreateCompatibleDC(janelaAux);
		hdcDasImg.Tiro = CreateCompatibleDC(janelaAux);
		hdcDasImg.Bomba = CreateCompatibleDC(janelaAux);
		hdcDasImg.Presente = CreateCompatibleDC(janelaAux);

		//colocar o bipMap para dentro do Device Contexts
		SelectObject(hdcDasImg.Wallpaper, bipMaps.Wallpaper);
		SelectObject(hdcDasImg.Space, bipMaps.Space);
		SelectObject(hdcDasImg.Basica, bipMaps.Basica); 
		SelectObject(hdcDasImg.Esquiva, bipMaps.Esquiva);
		SelectObject(hdcDasImg.Boss, bipMaps.Boss);
		SelectObject(hdcDasImg.Defensora1, bipMaps.Defensora1);
		SelectObject(hdcDasImg.Defensora2, bipMaps.Defensora2);
		SelectObject(hdcDasImg.Tiro, bipMaps.Tiro); 
		SelectObject(hdcDasImg.Bomba, bipMaps.Bomba);
		SelectObject(hdcDasImg.Presente, bipMaps.Presente);
				
		break;
	}
	

	case WM_CHAR: {

		
		if ((TCHAR)wParam == configuracoes.CIMA) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = cima;
			SetEvent(Cliente.EventEnvia);
			break;

		}
		if ((TCHAR)wParam == configuracoes.BAIXO) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = baixo;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.ESQUERDA) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = esquerda;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.DIREITA) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = direita;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.TIRO) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = LancaTiro;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.POWERUP1) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = PowerUp1;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.POWERUP2) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = PowerUp2;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		if ((TCHAR)wParam == configuracoes.POWERUP3) {
			LimpaPacotedEnvio();
			PacoteEnvio.tipo = AtualizacaoJogo;
			PacoteEnvio.dataPacket.comando = PowerUp3;
			SetEvent(Cliente.EventEnvia);
			break;
		}
		
		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR LpCmdLine, int ncmdshow)
{	
	// lança a thread que escuta no named pipe
	IniciaCliente(); 
	
	//Registar a class da janela
	const wchar_t CLASS_NAME[] = L"Janela Principal";

	WNDCLASS wc = {};

	///////////////////////////////////////Configuraçoes//////////////////////////////////////////////////////////////
	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, Configuracoes);
	ShowWindow(hDlg, ncmdshow);
	MSG msg = {};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//carregabitmaps para a esturuta
	carregaBitMaps();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = (HICON)LoadImage(NULL, L"../../Imagens/Logot.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	wc.hCursor =(HCURSOR)LoadImage(NULL, L"../../Imagens/Cursor.cur", IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);
	RegisterClass(&wc);
	
	// Criação da janela principal
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
	//Descomentar isto para aparecer o logo no inicio
	//BitBlt(janelaAux, 0, 0, 800, 800, hdcDasImg.Wallpaper, 0, 0, SRCCOPY);
	//InvalidateRect(hwndPrincipal, NULL, TRUE);
	

	tabAux[0].tipo = NaveBasica;
	tabAux[0].x = 0;
	tabAux[0].y = 0;
	
	tabAux[1].tipo = tiroJogador;
	tabAux[1].x = 10;
	tabAux[1].y = 4;

	tabAux[2].tipo = NavesEsquiva;
	tabAux[2].x = 12;
	tabAux[2].y = 23;

	tabAux[3].tipo = NaveBasica;
	tabAux[3].x = 2;
	tabAux[3].y = 0;

	tabAux[4].tipo = NaveJogador;
	tabAux[4].x = 4;
	tabAux[4].y = 30;

	tabAux[5].tipo = PowerUpVida;
	tabAux[5].x = 20;
	tabAux[5].y = 12;

	tabAux[6].tipo = NaveJogador;
	tabAux[6].x = 30;
	tabAux[6].y = 15;

	tabAux[6].tipo = bombaInimiga;
	tabAux[6].x = 18;
	tabAux[6].y = 29;

	tabAux[7].tipo = NaveBoss;
	tabAux[7].x = 10;
	tabAux[7].y = 0;

	

	BitBlt(janelaAux, 0, 0, 800, 800, hdcDasImg.Space, 0, 0, SRCCOPY);
	TransparentBlt(janelaAux, (tabAux[0].x * 20), (tabAux[0].y * 20), 40, 40, hdcDasImg.Basica, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[1].x * 20), (tabAux[1].y * 20), 15, 40, hdcDasImg.Tiro, 0, 0, 20, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[2].x * 20), (tabAux[2].y * 20), 40, 40, hdcDasImg.Esquiva, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[3].x * 20), (tabAux[3].y * 20), 40, 40, hdcDasImg.Basica, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[4].x * 20), (tabAux[4].y * 20), 40, 40, hdcDasImg.Defensora1, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[5].x * 20), (tabAux[5].y * 20), 40, 40, hdcDasImg.Defensora2, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[6].x * 20), (tabAux[6].y * 20), 15, 40, hdcDasImg.Bomba, 0, 0, 20, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	TransparentBlt(janelaAux, (tabAux[7].x * 20), (tabAux[7].y * 20), 120, 120, hdcDasImg.Boss, 0, 0, 40, 40, RGB(255, 255, 255)); // estica e tira a cor de fundo
	
	InvalidateRect(hwndPrincipal, NULL, TRUE);



	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	

	return 0;
}


/*
TCHAR yo[512];

_stprintf_s(yo, 512, TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
OutputDebugString(yo);
*/