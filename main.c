#include <stdio.h>      // printf...
#include <stdlib.h>     // exit, srand...
#include <time.h>       // time...
#include <sys/select.h> // select...
#include <unistd.h>     // usleep...
#include <termios.h>    // terminal
#include "defs.h"       // cabeçalho das funções

Jogo jogo = {
    .direcao = 'a',
    .dific = 1,         // 1 - Mapa pequeno | 2 - Mapa Grande | 3 - Mapa PacMan
    .maca = 0,
    .score = 0,         // 100FPS -> 38p (suporta até 39 ~200FPS)
    .A_MAP = 11,
    .L_MAP = 22
};      // Variávei globais do jogo

int main() {
    configurarModoRaw();    // Entra no modo de leitura contínua
    printf("\e[?25l");      // Esconde o cursor
    
    srand(time(NULL));      // gera a seed aleatória
    while(1){
        int loop = 1, op = 0;
        while(loop) {
            VerTelaLob(op, &jogo);      // Exibe a tela
            
            switch (getchar()) {
                case 'w':
                    if (op > 0) op--;
                    break;
                case 's':
                    if (op < 2) op++;
                    break;
                case '\n':
                    if (op == 0) loop = 0;
                    else if (op == 1) 
                        jogo.dific = (jogo.dific == 3) ? 1 : jogo.dific+1;
                    else exit(1);
                    break;
            }    // Lê as entradas
        }           // Tela Lob
        
        Cobra* cobrinha = CriarCobra(&jogo);
        loop = 1;
        
        int Dsleep = 200000;        // ~5 FPS
        
        printf("\033[H\033[J");     // Limpa a tela
        fflush(stdout);             // Limpa o buffer
        
        while(loop){
            CarregarTelaJogo(cobrinha, &jogo);              // Carrega a tela
            VerTelaJogo(&jogo);                             // Exibe a tela
            
            int fps = (Dsleep > 0) ? 1000000 / Dsleep : 0;
            printf("\tFPS: %d\n", fps);                     /* Calcula do FPS */
            
            if (kbhit()) {
                switch(getchar()) {
                    case 'q': loop = 0; break;
                    case 'w': if (jogo.direcao != 's') jogo.direcao = 'w'; break;
                    case 's': if (jogo.direcao != 'w') jogo.direcao = 's'; break;
                    case 'a': if (jogo.direcao != 'd') jogo.direcao = 'a'; break;
                    case 'd': if (jogo.direcao != 'a') jogo.direcao = 'd'; break;
                }
            }                              // Lê a movimentação
            
            if (!MoverCobra(&cobrinha, &jogo)) loop = 0;    // Caso de morte
            
            /* Nas Dific 1 e 2, temos variação no FPS */
            if (jogo.dific != 3 && jogo.score <= 38) Dsleep = 200000 - (jogo.score * 5000); 
            usleep(Dsleep);
        }            // Tela Jogo
        
        VerTelaOver(&jogo, Dsleep); // Exibe a tela
        
        /* Reseta o jogo */
        jogo.direcao = 'a';
        LiberarCobra(cobrinha);
        ResetarTelaMapAnterior();
    }           // Loop principal
    
    restaurarModoNormal();  // Sai do modo de leitura contínua
    return 0;
}