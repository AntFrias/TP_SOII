#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

BlocoServ blocoServ[dimMapa_x][dimMapa_y];

// funcao que vai limpar os tabuleiros do servidor e da memoria partilhada
void limpaTabuleiro() {

	for (int y = 0; y < dimMapa_y; y++) {
		for (int x = 0; x < dimMapa_x; x++) {

			blocoServ[y][x].id = 0; //n�o existe
			blocoServ[y][x].tipo = bloco_vazio; //vazio
			blocoServ[y][x].posArray = 0; //vazio
			escreveBufferTabuleiro(x, y, bloco_vazio);
		}
	}
}
// mostra tabuleiro do Servidor
void mostraTabuleiro() {

	for (int y = 0; y < dimMapa_y; y++) {
		for (int x = 0; x < dimMapa_y; x++) {

			_tprintf(TEXT("%d"), blocoServ[y][x].tipo); //tirei o espaco

		}
		_tprintf(TEXT("\n"));
	}

}
//vai verificar posi�oes vazias para preencher com naves Enimigas antes de come�ar o jogo
int VerificaPosicaoPreencheTAb(int *x, int *y) {

	if (blocoServ[*y][*x].tipo == bloco_vazio && blocoServ[*y][*x + 1].tipo == bloco_vazio && blocoServ[*y + 1][*x].tipo == bloco_vazio && blocoServ[*y + 1][*x + 1].tipo == bloco_vazio)
		return 1;
	else
		return 0;

}
// verifica posicao da nave Esquiva
int VerificaPosicaoNaveEsquiva(int *x, int *y, int orientacao) {

	switch (orientacao)
	{
	case cima:
		if (*x + 1 < dimMapa_x && *y - 1 >= 0) {

			if (blocoServ[*x][*y - 1].tipo == bloco_vazio && blocoServ[*x + 1][*y - 1].tipo == bloco_vazio) {

				return 1;

			}
			else {

				return 0;
			}
		}
		break;

	case baixo:
		if (*x + 1 < dimMapa_x && *y + 1 < dimMapa_y - 4) {

			if (blocoServ[*x][*y + 1].tipo == bloco_vazio && blocoServ[*x + 1][*y + 1].tipo == bloco_vazio) {

				return 1;

			}
			else {

				return 0;
			}
		}
		break;

	case esquerda:
		if (*x - 1 >= 0 && *y + 1 < dimMapa_y - 4) {

			if (blocoServ[*x - 1][*y].tipo == bloco_vazio && blocoServ[*x - 1][*y + 1].tipo == bloco_vazio) {

				return 1;

			}
			else {

				return 0;

			}
		}
		break;

	case direita:
		if (*x + 1 < dimMapa_x && *y + 1 < dimMapa_y - 4) {

			if (blocoServ[*x + 1][*y].tipo == bloco_vazio && blocoServ[*x + 1][*y + 1].tipo == bloco_vazio) {

				return 1;

			}
			else {

				return 0;
			}
		}
		break;
	}
	return 0;
}
// verifica posicao da nave jogador dentro da area onde se pode movimentar
int VerificaPosNaveJogador(int *x, int *y, int orientacao) {

	switch (orientacao)
	{
	case cima:
		if (*y - 1 >= dimMapa_y - RegiaoNaveJogador) {
			if (blocoServ[*y - 1][*x].tipo == bloco_vazio && blocoServ[*y - 1][*x + 1].tipo == bloco_vazio) {
				return 1;
			}
			else {
				return 0;
			}
		}
		break;
	case baixo:
		if (*y + 2 < dimMapa_y) {
			
			if (blocoServ[*y + 2][*x].tipo == bloco_vazio && blocoServ[*y + 2][*x + 1].tipo == bloco_vazio) {
			
				return 1;
			}
			else {
				return 0;
			}
		}
		break;

	case esquerda:
		if (*x - 1 >= 0) {

			if (blocoServ[*y][*x - 1].tipo == bloco_vazio && blocoServ[*y + 1][*x - 1].tipo == bloco_vazio) {
				return 1;
			}
			else {
				return 0;
			}
		}
		break;

	case direita:
		if (*x + 2 < dimMapa_x) {
			if (blocoServ[*y][*x + 2].tipo == bloco_vazio && blocoServ[*y + 1][*x + 2].tipo == bloco_vazio) {
				return 1;
			}
			else {
				return 0;
			}
		}
		break;
	}
	return 0;
}
// funcao que verifica a posicacao para onde o jogador quer-se movimentar
int VerificaPosicaoJogo( int *x, int *y, int tipo, int orientacao) {
	int Flag;
	switch (tipo)
	{
		case NaveEsquiva:
			Flag = VerificaPosicaoNaveEsquiva(x, y, orientacao);
			return Flag;
			break;
		case NaveJogador:
			Flag = VerificaPosNaveJogador(x, y, orientacao);
			return Flag;
			break;
		case NaveBasica:
			break;
		case NaveBoss:
			break;
		case LancaTiro:
			break;
		case PowerUp1:
			break;
	}
	return 0;
}
// funcao vai limpar as posicoes do tabuleiro //
void LimpaPosTabuleiro(int x, int y, int tipo, int Largura) {

	for (int i = y; i < y + Largura; i++) {

		for (int j = x; j < x + Largura; j++) {    

			blocoServ[i][j].tipo = tipo;
			
			escreveBufferTabuleiro(j, i, blocoServ[i][j].tipo);
		}
	}
}
// preenche Bocos no tabuleiro do servidor e no bufferTabuleiro
void preencheBlocosServidor(int *x, int *y, int pos, int tipo, int Largura) {

	for (int i = *y; i < *y + Largura; i++) {

		for (int j = *x; j < *x + Largura; j++) {

			blocoServ[i][j].tipo = tipo;

			blocoServ[i][j].posArray = pos;

			escreveBufferTabuleiro(j, i, blocoServ[i][j].tipo);
		}
	}
}
// funcao que vai colocar as naves antes do jogo come�ar no tabuleiro
void ColocaNavesTab() {
	
	colocaNavesBasicas();

	colocaNavesEsquiva();
	
	mostraTabuleiro();
	_tprintf(TEXT("\n\n"));
	mostraTabCom();

}
//Funcao que ir� iniciar o jogo e as respetivas movimentacoes quando o jogador decidir jogar
void IniciarJogo(int *x, int *y,int pos) {

	int posX = 0, posY = dimMapa_y - 2, x_min = 0, x_max = dimMapa_x - 1;
	
	do{
	
		do {

			posX = rand() % (x_max + 1 - x_min) + x_min;

		} while (posX < 0 && posX < dimMapa_x);


	}while (VerificaPosicaoPreencheTAb(&posX, &posY) == 0);

	*x = posX;
	*y = posY;

	preencheBlocosServidor(x, y, pos, NaveJogador, LarguraNaveDefault);
	_tprintf(TEXT("\n\n\n"));
	mostraTabuleiro();
	

}

