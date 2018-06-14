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
int VerificaPosicao( int x, int y) {
	if (x < 40 || y < 40)
		if (blocoServ[x][y].tipo == bloco_vazio && blocoServ[x][y + LarguraNaveDefault].tipo == bloco_vazio && blocoServ[x + LarguraNaveDefault][y].tipo == bloco_vazio & blocoServ[x + LarguraNaveDefault][y + LarguraNaveDefault].tipo == bloco_vazio)
			return 1;
		else
			return 0;
	else
		return 0;
}
// preenche Bocos no tabuleiro do servidor e no bufferTabuleiro
void preencheBlocosServidor(int x, int y, int pos, int tipo) {

	for (int i = x; i < x + LarguraNaveDefault; i++) {

		for (int j = y; j < y + LarguraNaveDefault; j++) {

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

	mostraTabuleiro();
	_tprintf(TEXT("\n\n"));
	mostraTabCom();

}
//Funcao que irá iniciar o jogo e as respetivas movimentacoes quando o jogador decidir jogar
void IniciarJogo() {

}

Packet TrataPacoteComandosJogo(Packet *aux, int *x, int *y) {

	Packet resposta;

	switch (aux->dataPacket.comando) {
		
	case cima:
		

	case baixo:

	case esquerda:
		x = x - 1;
		if ( verificaPosicao(x, y) == 1)

	case direita:
	}

}