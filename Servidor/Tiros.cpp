#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

//
tiro ArrayTiros[MaxTiros];

gestorTiros GestorTiros;

void AlteraPosicaoTiro(int PosObjeto, int *x, int *y) {

	ArrayTiros[PosObjeto].x = *x;
	ArrayTiros[PosObjeto].x = *y;

}

int obtemPosicaoTiro() {

	for (int i = 0; i < MaxTiros; i++) {

		if (ArrayTiros[i].tipo == Tirovazio) {

			GestorTiros.TotalTiros += 1;

			return i;

		}

		
	}

}
void trataMovimentacaoTiroJogador(int PosTiro, int tipo, int ProprietarioMissil) {

	int tipoObjeto, x = ArrayTiros[PosTiro].x, y = ArrayTiros[PosTiro].y;

	WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

		tipoObjeto = VerificaPosicaoJogo(&x, &y, tipo, cima);
		
			switch (tipoObjeto) {

				case bloco_vazio:

					LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

					y -= 1;
	
					ArrayTiros[PosTiro].x = x;

					ArrayTiros[PosTiro].y = y;

					preencheBlocosServidorTiro(&x, &y, PosTiro, tiroJogador, LarguraTiroDefault);

					SetEvent(GestorTiros.AtualizaTabuleiro);

				break;

				case fim_Tabuleiro:

					LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

					preencheBlocosServidorTiro(&x, &y, PosTiro, TiroExplosao, LarguraTiroDefault);
					
					SetEvent(GestorTiros.AtualizaTabuleiro);
					
					ArrayTiros[PosTiro].tipo = Tirovazio;
					_tprintf(TEXT("Antes de o tiro morrer eram %d"), GestorTiros.TotalTiros);
					GestorTiros.TotalTiros -= 1;
					_tprintf(TEXT("DEpois de o tiro morrer eram %d"), GestorTiros.TotalTiros);
				break;
			}
	
	ReleaseMutex(GestorTiros.MutexTabuleiro);

}

void GestorTirosTab() {

	InicializaArrayTiros();

	do {
		
		WaitForSingleObject(GestorTiros.EventoLancaTiro, INFINITE);
	
		do {
			
			for (int i = 0; i < MaxTiros; i++) {

				WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

					if (ArrayTiros[i].tipo != Tirovazio) {
					
						switch (ArrayTiros[i].tipo) {

						case tiroJogador:
							trataMovimentacaoTiroJogador(i, ArrayTiros[i].tipo, ArrayTiros[i].posProprietario);
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
			}
			

			Sleep(TempoDeEnvioTabuleiro);

		} while (GestorTiros.TotalTiros > 0);

		ResetEvent(GestorTiros.EventoLancaTiro);

	} while (GestorTiros.ServerUp == 1);

}
//vai adicionar um tiro ao array de tiros
void AdicionaTiroArray(int x, int y, int tipo, int PosObjeto) {

	int PosTiro;

	WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

	if (GestorTiros.TotalTiros < MaxTiros){

			PosTiro = obtemPosicaoTiro();

			switch (tipo) {

				case NaveJogador:
		
					if (VerificaPosicaoJogo(&x, &y, tiroJogador, cima) == bloco_vazio) {

						y = y - 2;

						ArrayTiros[PosTiro].tipo = tiroJogador;

						ArrayTiros[PosTiro].posProprietario = PosObjeto;

						ArrayTiros[PosTiro].x = x;

						ArrayTiros[PosTiro].y = y;

						WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

						preencheBlocosServidorTiro(&x, &y, PosTiro, ArrayTiros[PosTiro].tipo, LarguraTiroDefault);

						SetEvent(GestorTiros.AtualizaTabuleiro);

						ReleaseMutex(GestorTiros.MutexTabuleiro);

					}
			
					break;

				case NaveEsquiva:

					if (VerificaPosicaoJogo(&x, &y, tiroNaveEnemy, baixo) == bloco_vazio) {

						y = y + 2;

						ArrayTiros[PosTiro].tipo = tiroNaveEnemy;

						ArrayTiros[PosTiro].posProprietario = PosObjeto;

						ArrayTiros[PosTiro].x = x;

						ArrayTiros[PosTiro].y = y;

						WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

							preencheBlocosServidorTiro(&x, &y, PosTiro, ArrayTiros[PosTiro].tipo, LarguraTiroDefault);

							SetEvent(GestorTiros.AtualizaTabuleiro);

						ReleaseMutex(GestorTiros.MutexTabuleiro);

					}
					break;
			}

		if (GestorTiros.TotalTiros == 1) {

			SetEvent(GestorTiros.EventoLancaTiro);
		}
	}
	ReleaseMutex(GestorTiros.MutexTiroArray);
}
// vai iniciar o array de tiros
void InicializaArrayTiros() {

	//ArrayTiros = (tiro *)malloc(sizeof(tiro)*MaxTiros);

	for (int i = 0; i < MaxTiros; i++) {

		ArrayTiros[i].tipo = Tirovazio;

		ArrayTiros[i].posProprietario = 0;

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

	GestorTiros.EventoLancaTiro = CreateEvent(NULL, 0, 0, NULL);

	if (GestorTiros.EventoLancaTiro == NULL) {

		exit(-1);

	}

	GestorTiros.MutexTiroArray = CreateMutex(NULL, FALSE, NULL);

	if (GestorTiros.MutexTiroArray == NULL) {

		exit(-1);

	}
}
