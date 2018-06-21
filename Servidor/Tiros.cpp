#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

//
tiro *ArrayTiros = NULL;

gestorTiros GestorTiros;

void AlteraPosicaoTiro(int PosObjeto, int *x, int *y) {

	ArrayTiros[PosObjeto].x = *x;
	ArrayTiros[PosObjeto].x = *y;

}

int obtemPosicaoTiro() {

	for (int i = 0; i < MaxTiros; i++) {

		if (ArrayTiros[i].tipo == Tirovazio) {

			GestorTiros.TotalTiros += 1;

		}

		return i;
	}

}
void trataMovimentacaoTiro(int *x, int *y, int tipo, int PosObjeto) {

	int tipoObjeto, coord_x = *x, coord_y = *y;

	WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

	switch (tipo) {

	case tiroJogador:
		tipoObjeto = VerificaPosicaoJogo(x, y, tipo, cima);

		switch (tipoObjeto) {

		case bloco_vazio:
			LimpaPosTabuleiroTiro(coord_x, coord_y, bloco_vazio, LarguraTiroDefault);
			*y += 1;
			preencheBlocosServidorTiro(x, y, PosObjeto, tiroJogador, LarguraTiroDefault);
			break;
		}
		break;
	}

	ReleaseMutex(GestorTiros.MutexTiroArray);
}

void GestorTirosTab() {

	do {
		_tprintf(TEXT("\n\n\nCheguei aqui á Tread Gertor de Tiros e vou esperar pelo evento\n\n\n\n"));
		WaitForSingleObject(GestorTiros.EventoLancaTiro, INFINITE);
		_tprintf(TEXT("\n\n\nDesbloqueei a Thread dos tiros pelo evento\n\n\n\n"));
		do {
			WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

			for (int i = 0; i < MaxTiros; i++) {

				switch (ArrayTiros[i].tipo) {

					case tiroJogador:
						trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
						break;
					case tiroNaveEnemy:
						//trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
						break;
					case tiroBoss:
						//trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
						break;
					case tiroNuclear:
						//trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
						break;
				}
			}
			ReleaseMutex(GestorTiros.MutexTiroArray);

			Sleep(TempoDeEnvioTabuleiro);

		} while (GestorTiros.TotalTiros > 0);

	} while (GestorTiros.ServerUp == 1);

}
//vai adicionar um tiro ao array de tiros
void AdicionaTiroArray(int x, int y, int tipo, int PosObjeto) {

	int PosTiro;

	WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

		PosTiro = obtemPosicaoTiro();

		switch (tipo) {

			case NaveJogador:

				if (VerificaPosicaoJogo(&x, &y, tiroJogador, cima) == bloco_vazio) {

					y = y - 2;

					ArrayTiros[PosTiro].tipo = tiroJogador;

					ArrayTiros[PosTiro].posJogador = PosObjeto;

					alteraPosicaoObjeto(PosObjeto,LancaTiro, &x, &y);

					WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

						LimpaPosTabuleiroTiro(x, y, ArrayTiros[PosObjeto].tipo, LarguraTiroDefault);

						preencheBlocosServidorTiro(&x, &y, PosTiro, ArrayTiros[PosTiro].tipo, LarguraTiroDefault);

						SetEvent(GestorTiros.AtualizaTabuleiro);

					ReleaseMutex(GestorTiros.MutexTabuleiro);

					if (GestorTiros.TotalTiros == 1) {

						SetEvent(GestorTiros.EventoLancaTiro);
					}
				}

				break;

			case NaveEsquiva:

				if (VerificaPosicaoJogo(&x, &y, tiroNaveEnemy, baixo) == bloco_vazio) {

					y = y + 2;

					ArrayTiros[PosTiro].tipo = tiroNaveEnemy;

					ArrayTiros[PosTiro].posJogador = PosObjeto;

					alteraPosicaoObjeto(PosObjeto, LancaTiro, &x, &y);

					WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

						LimpaPosTabuleiroTiro(x, y, ArrayTiros[PosObjeto].tipo, LarguraTiroDefault);

						preencheBlocosServidorTiro(&x, &y, PosTiro, ArrayTiros[PosTiro].tipo, LarguraTiroDefault);

						SetEvent(GestorTiros.AtualizaTabuleiro);

					ReleaseMutex(GestorTiros.MutexTabuleiro);

					if (GestorTiros.TotalTiros == 1) {

						//SetEvent(GestorTiros.EventoLancaTiro);
					}
				}
				break;
		}	
	ReleaseMutex(GestorTiros.MutexTiroArray);
}
// vai iniciar o array de tiros
void InicializaArrayTiros() {

	ArrayTiros = (tiro *)malloc(sizeof(tiro)*MaxTiros);

	for (int i = 0; i < MaxTiros; i++) {

		ArrayTiros[i].tipo = Tirovazio;

		ArrayTiros[i].idJogador = 0;

		ArrayTiros[i].posJogador = 0;

		ArrayTiros[i].vida = 0;

		ArrayTiros[i].x = 0;

		ArrayTiros[i].y = 0;
	}
}
// inicia dados na estrutura dos tiros
void IniciaDadosTiros(HANDLE mutexTabuleiro, HANDLE AtualizaTabuleiro){

	GestorTiros.ServerUp = 1;

	GestorTiros.TotalTiros = 0;

	GestorTiros.MutexTabuleiro = mutexTabuleiro;

	GestorTiros.AtualizaTabuleiro = AtualizaTabuleiro;

}
// inicio sincronizaçao dos tiros
void IniciaSincronizacaoTiros(){

	GestorTiros.EventoLancaTiro = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (GestorTiros.EventoLancaTiro == NULL) {

		exit(-1);

	}

	GestorTiros.MutexTiroArray = CreateMutex(NULL, FALSE, NULL);

	if (GestorTiros.MutexTiroArray == NULL) {

		exit(-1);

	}
}
