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
void trataMovimentacaoTiroInimigas(int PosTiro, int tipo, int ProprietarioMissil) {

	int tipoObjeto, x = ArrayTiros[PosTiro].x, y = ArrayTiros[PosTiro].y, y_aux = ArrayTiros[PosTiro].y;

		tipoObjeto = VerificaPosicaoJogo(&x, &y, tipo, baixo);

		switch (tipoObjeto) {

		case bloco_vazio:

			LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

			y += 1;

			ArrayTiros[PosTiro].x = x;

			ArrayTiros[PosTiro].y = y;

			preencheBlocosServidorTiro(&x, &y, PosTiro, tiroNaveEnemy, LarguraTiroDefault);

			SetEvent(GestorTiros.AtualizaTabuleiro);

			break;

		case Limite_Tabuleiro:
		
			LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

			//preencheBlocosServidorTiro(&x, &y, PosTiro, TiroExplosao, LarguraTiroDefault);

			SetEvent(GestorTiros.AtualizaTabuleiro);

			ArrayTiros[PosTiro].tipo = Tirovazio;

			GestorTiros.TotalTiros -= 1;

			break;

		case NaveJogador:

			y_aux += 1;

			if (AlteraVidaObjeto(x, y_aux, tipoObjeto, GestorTiros.AtualizaTabuleiro) == 0) {

				LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

				//preencheBlocosServidorTiro(&x, &y_aux, PosTiro, TiroExplosao, LarguraTiroDefault);

				SetEvent(GestorTiros.AtualizaTabuleiro);

				ArrayTiros[PosTiro].tipo = Tirovazio;

				GestorTiros.TotalTiros -= 1;

			}
			break;

		}
}
int ObtemPosTiroInimigo(int x, int y) {

	for (int i = 0; i < MaxTiros; i++) {

		if (ArrayTiros[i].tipo != Tirovazio && ArrayTiros[i].x == x && ArrayTiros[i].y == y) {

			return i;
		}
	}
}

void trataMovimentacaoTiroJogador(int PosTiro, int tipo, int ProprietarioMissil) {

	int tipoObjeto, x = ArrayTiros[PosTiro].x, y = ArrayTiros[PosTiro].y, y_aux = ArrayTiros[PosTiro].y, PosTiroInimigo;

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

				case Limite_Tabuleiro:

						LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

						//preencheBlocosServidorTiro(&x, &y, PosTiro, TiroExplosao, LarguraTiroDefault);
					
						//SetEvent(GestorTiros.AtualizaTabuleiro);

						ArrayTiros[PosTiro].tipo = Tirovazio;
					
						GestorTiros.TotalTiros -= 1;
					
				break;

				default:

					y_aux -= 1;

					if (tipoObjeto > bloco_vazio && tipoObjeto < NaveJogador) {
	
						if (AlteraVidaObjeto(x, y_aux, tipoObjeto, GestorTiros.AtualizaTabuleiro) == 0) {

							LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

							//preencheBlocosServidorTiro(&x, &y_aux, PosTiro, TiroExplosao, LarguraTiroDefault);

							SetEvent(GestorTiros.AtualizaTabuleiro);

							ArrayTiros[PosTiro].tipo = Tirovazio;

							GestorTiros.TotalTiros -= 1;

							break;

						}
						else {

							LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

							SetEvent(GestorTiros.AtualizaTabuleiro);

							ArrayTiros[PosTiro].tipo = Tirovazio;

							GestorTiros.TotalTiros -= 1;

							break;
						}
					}
					if (tipoObjeto == tiroNaveEnemy) {

							y_aux -= 1;

							PosTiroInimigo = ObtemPosTiroInimigo(x, y_aux);

							ArrayTiros[PosTiroInimigo].tipo = Tirovazio;

							ArrayTiros[PosTiro].tipo = Tirovazio;

							GestorTiros.TotalTiros -= 2;

							LimpaPosTabuleiroTiro(x, y_aux, bloco_vazio, LarguraTiroDefault);

							LimpaPosTabuleiroTiro(x, y, bloco_vazio, LarguraTiroDefault);

							y -= 1;

							//preencheBlocosServidorTiro(&x, &y, PosTiro, TiroExplosao, LarguraTiroDefault);

							//SetEvent(GestorTiros.AtualizaTabuleiro);

					} 
				break;
			}
}

void GestorTirosTab() {

	InicializaArrayTiros();

	do {
		
		WaitForSingleObject(GestorTiros.EventoLancaTiro, INFINITE);
	
		do {
			
			WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);
			for (int i = 0; i < MaxTiros; i++) {

				

					if (ArrayTiros[i].tipo != Tirovazio) {
					
						switch (ArrayTiros[i].tipo) {

						case tiroJogador:

							WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

							trataMovimentacaoTiroJogador(i, ArrayTiros[i].tipo, ArrayTiros[i].posProprietario);

							ReleaseMutex(GestorTiros.MutexTabuleiro);

							break;
						case tiroNaveEnemy:

							WaitForSingleObject(GestorTiros.MutexTabuleiro, INFINITE);

							trataMovimentacaoTiroInimigas(i, ArrayTiros[i].tipo, ArrayTiros[i].posProprietario);

							ReleaseMutex(GestorTiros.MutexTabuleiro);

							break;
						case tiroBoss:
							//trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
							break;
						case tiroNuclear:
							//trataMovimentacaoTiro(&ArrayTiros[i].x, &ArrayTiros[i].y, ArrayTiros[i].tipo, i);
							break;
						}
					}

				
			}
			ReleaseMutex(GestorTiros.MutexTiroArray);
		Sleep(TempoDeEnvioTabuleiro);

		} while (GestorTiros.TotalTiros > 0);

		ResetEvent(GestorTiros.EventoLancaTiro);
		
	} while (GestorTiros.ServerUp == 1);

}
//vai adicionar um tiro ao array de tiros
void AdicionaTiroArray(int x, int y, int tipo, int PosObjeto) {

	int PosTiro;

	if (GestorTiros.TotalTiros < MaxTiros){

		WaitForSingleObject(GestorTiros.MutexTiroArray, INFINITE);

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
		ReleaseMutex(GestorTiros.MutexTiroArray);
	}
	
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
