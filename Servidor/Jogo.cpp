#include "../AcessoMemDLL/stdafx.h"
#include "Servidor.h"
#pragma comment(lib, "../x64/Debug/AcessoMemDLL.lib")

BlocoServ blocoServ[dimMapa_x][dimMapa_y];

// funcao que vai limpar os tabuleiros do servidor e da memoria partilhada
void limpaTabuleiro() {

	for (int x = 0; x < dimMapa_x; x++) {
		for (int y = 0; y < dimMapa_y; y++) {

			blocoServ[x][y].id = 0; //não existe
			blocoServ[x][y].tipo = bloco_vazio; //vazio
			blocoServ[x][y].posArray = 0; //vazio
			escreveBufferTabuleiro(x, y, bloco_vazio);
		}
	}
}
// mostra tabuleiro do Servidor
void mostraTabuleiro() {

	for (int x = 0; x < dimMapa_x; x++) {
		for (int y = 0; y < dimMapa_y; y++) {

			_tprintf(TEXT("%d"), blocoServ[x][y].tipo); //tirei o espaco

		}
		_tprintf(TEXT("\n"));
	}

}
//vai verificar posiçoes vazias para preencher com naves Enimigas antes de começar o jogo
int VerificaPosicaoPreencheTAb(int *x, int *y) {

	if (blocoServ[*x][*y].tipo == bloco_vazio && blocoServ[*x + 1][*y].tipo == bloco_vazio && blocoServ[*x][*y + 1].tipo == bloco_vazio && blocoServ[*x + 1][*y + 1].tipo == bloco_vazio)
		return 1;
	else
		return 0;

}
// funcao que verifica a posicacao para onde o jogador quer-se movimentar
int VerificaPosicaoJogo( int *x, int *y, int tipo, int orientacao) {

	switch (tipo)
	{
		case NaveJogador:
		
				switch (orientacao)
				{
					case cima:
						if (*x + 1 < dimMapa_x && *y - 1 >= 0) {

							if (blocoServ[*x][*y - 1].tipo == bloco_vazio && blocoServ[*x + 1][*y - 1].tipo == bloco_vazio){
							
								return 1;
							
							} else {
							
								return 0;
							}
						}
						break;
					
					case baixo: 
						if ( *x + 1 < dimMapa_x && *y + 1 < dimMapa_y){
							
							if (blocoServ[*x][*y + 1].tipo == bloco_vazio && blocoServ[*x + 1][*y + 1].tipo == bloco_vazio){
							
								return 1;
							
							} else {
								
								return 0;
							}
						}
						break;
					
					case esquerda: 
						if ( *x - 1 >= 0 && *y + 1 < dimMapa_y) {

							if (blocoServ[*x - 1][*y].tipo == bloco_vazio && blocoServ[*x - 1][*y + 1].tipo == bloco_vazio) {
							
								return 1;
							
							} else {
							
								return 0;
							
							}
						}
						break;

					case direita:
						if (*x + 1 < dimMapa_x && *y + 1 < dimMapa_y) {
							
							if (blocoServ[*x + 1][*y].tipo == bloco_vazio && blocoServ[*x + 1][*y + 1].tipo == bloco_vazio) {
								
								return 1;
							
							} else {
							
								return 0;
							}
						}
						break;
				}
	
		
			break;
	}
	return 0;
}
// funcao vai limpar as posicoes do tabuleiro
void LimpaPosTabuleiro(int x, int y, int tipo, int Largura) {

	for (int i = x; i < x + Largura; i++) {

		for (int j = y; j < y + Largura; j++) {

			blocoServ[i][j].tipo = tipo;

			escreveBufferTabuleiro(i, j, blocoServ[i][j].tipo);
		}
	}
}
// preenche Bocos no tabuleiro do servidor e no bufferTabuleiro
void preencheBlocosServidor(int x, int y, int pos, int tipo, int Largura) {
	
	for (int i = x; i < x + Largura; i++) {

		for (int j = y; j < y + Largura; j++) {

			blocoServ[i][j].tipo = tipo;

			blocoServ[i][j].posArray = pos;

			escreveBufferTabuleiro(i, j, blocoServ[i][j].tipo);
		}
	}
}
// funcao que vai colocar as naves antes do jogo começar no tabuleiro
void ColocaNavesTab() {
	
	colocaNavesBasicas();

	colocaNavesEsquiva();
	_tprintf(TEXT("\ncheguei aqui3"));
	mostraTabuleiro();
	_tprintf(TEXT("\n\n"));
	mostraTabCom();

}
//Funcao que irá iniciar o jogo e as respetivas movimentacoes quando o jogador decidir jogar
void IniciarJogo() {

}

