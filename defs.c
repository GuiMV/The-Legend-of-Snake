#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "defs.h"

// ======== Terminal ========

static struct termios estadoOriginal;

void configurarModoRaw() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &estadoOriginal); // Guarda estado original
    t = estadoOriginal;
    t.c_lflag &= ~(ICANON | ECHO); // Desativa modo canônico e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}
void restaurarModoNormal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &estadoOriginal); // Restaura
}
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) > 0;
}

// ======== Telas ========

const char* lob[A_LOB] = {
    "",
    "\t\t\tThe Legend of 🐍\n",
    "\t\t\t     Play\n",
    "",
    "\t\t\t     Sair\n",
    "Comandos: w a s d q*",
};
char telaLob[A_LOB][L_LOB];
char telaLobAnterior[A_LOB][L_LOB];

const char* map1[A_MAP1] = {
    "#####################",
    "#                   #",
    "#                   #",
    "#                   #",
    "                     ",
    "                     ",
    "                     ",
    "#                   #",
    "#                   #",
    "#                   #",
    "#####################"
};
const char* map2[A_MAP23] = {
    "##########################################",
    "#                                        #",
    "#                                        #",
    "                                          ",
    "                     #                    ",
    "                     #                    ",
    "                                          ",
    "#                                        #",
    "#                                        #",
    "                                          ",
    "                     #                    ",
    "                     #                    ",
    "                                          ",
    "#                                        #",
    "#                                        #",
    "##########################################"
};
const char* map3[A_MAP23] = {
    "##########################################",
    "#                   ###                  #",
    "#            ###    ###    ###    ###    #",
    "#   ###                    ###    ###    #",
    "#                                        #",
    "#            #     #####     #           #",
    "#######      #               #    ########",
    "             ###    ###    ###            ",
    "                    ###                   ",
    "#######                           ########",
    "#                                        #",
    "#                  #####                 #",
    "#        #####       #     #####         #",
    "#                    #                   #",
    "#                                        #",
    "##########################################"
};
char telaMap[A_MAP23][L_MAP23];
char telaMapAnterior[A_MAP23][L_MAP23];

void VerTelaLob(int op, Jogo *j){
    CarregarTelaLob(j);                     // Carrega a tela antes de exibir
    
    if (!op) telaLob[2][3] = '>';           /* Adiciona o > na posição */
    else if (op == 1) telaLob[3][3] = '>';
    else telaLob[4][3] = '>';
    
    printf("\033[H\033[J");                 /* Limpa a tela anterior e exibe */
    for (int i = 0; i < A_LOB; i++)
        printf("%s\n", telaLob[i]);
}
void CarregarTelaLob(Jogo *j) {
    for (int i = 0; i < A_LOB; i++) {
        if (i == 3) snprintf(telaLob[i], L_LOB, "\t\t\t  Dificuldade %d\n", j->dific);
        else        strncpy(telaLob[i], lob[i], L_LOB);
        telaLob[i][L_LOB - 1] = '\0';
    }
}

void VerTelaJogo(Jogo* j) {
    for (int i = 0; i < j->A_MAP; i++) {
        for (int k = 0; k < j->L_MAP; k++) {
            char c = telaMap[i][k];
            if (c != telaMapAnterior[i][k]) {
                printf("\033[%d;%dH", i + 1, k + 1);
                switch(c) {
                    case '#':
                        printf("\033[34m#\033[0m");
                        break;
                    case 'O': case 'o':
                        printf("\033[32m%c\033[0m", c);
                        break;
                    case '@':
                        printf("\033[31m@\033[0m");
                        break;
                    default:
                        putchar(c);
                }
                telaMapAnterior[i][k] = c;  // atualiza o buffer antigo
            }   // Ajusta cada cor
        }
    }                   // Exibe a tela
    printf("\033[%d;1HScore: %d  ", j->A_MAP + 1, j->score);   // Atualiza Score
    fflush(stdout);                                            // Limpa o buffer
}
void CarregarTelaJogo(Cobra* cobrinha, Jogo* j) {
    const char **mapa = NULL;
    
    if (j->dific == 1){
        mapa = map1;
        j->A_MAP = 11;
        j->L_MAP = 22;
    } else {
        mapa = (j->dific == 2) ? map2 : map3;
        j->A_MAP = 16;
        j->L_MAP = 43;
    }
        
    for (int i = 0; i < j->A_MAP; i++){
        strncpy(telaMap[i], mapa[i], j->L_MAP);
        telaMap[i][j->L_MAP - 1] = '\0';
    }

    CarregarCobra(cobrinha);
    if (!j->maca) {
        CarregarMaca(j);
    }
    telaMap[j->maca_y][j->maca_x] = '@';
}

void VerTelaOver(Jogo* j, int Dsleep){
    printf("\033[H\033[J");
    printf("\n\t\t\t🗙 Game Over 🗙\n\n");
    
    usleep(1000000);
    int fps = (Dsleep > 0) ? 1000000 / Dsleep : 0;
    
    printf("\tScore: %d\n", j->score);
    printf("\tFPS: %d\n", fps);
    
    usleep(500000);
    printf("\n\tPressione Enter\n");
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    
    j->score = 0; j->maca = 0;
}

void ResetarTelaMapAnterior() {
    for (int i = 0; i < A_MAP23; i++)
        for (int j = 0; j < L_MAP23; j++)
            telaMapAnterior[i][j] = ' ';
}

// ======== Funções de jogo ========

void CarregarMaca(Jogo *j){
    do {
        j->maca_x = 1 + rand() % (j->L_MAP-2);
        j->maca_y = 1 + rand() % (j->A_MAP-2);
    } while (telaMap[j->maca_y][j->maca_x] != ' '); // Posição válida
    
    j->maca = 1;                                           // Declara existência
}

Cobra* CriarCobra(Jogo *j){
    Cobra* cabeca = NULL;                 /* Cria uma cobra com lógica de LSE */
    for (int i = 0; i < 3; i++) {
        Cobra* novo = malloc(sizeof(Cobra));
        novo->x = (j->dific == 1) ? 11-i : 22-i;
        novo->y = (j->dific == 1) ? 5 : (j->dific == 2) ? 7 : 9;
        novo->prox = cabeca;
        cabeca = novo;
    }
    return cabeca;
}
void CarregarCobra(Cobra* cobrinha) {
    int primeira = 1;                              /* Carrega a cobra no mapa */
    while (cobrinha != NULL) {
        telaMap[cobrinha->y][cobrinha->x] = primeira ? 'O' : 'o';
        primeira = 0;
        cobrinha = cobrinha->prox;
    }
}
int MoverCobra(Cobra** cobrinha, Jogo* j){
    int dx = 0, dy = 0;
    
    switch (j->direcao) {
        case 'w': dy = -1; break;
        case 's': dy = 1; break;
        case 'a': dx = -1; break;
        case 'd': dx = 1; break;
    }           // Movimento
    
    int novoX = (*cobrinha)->x + dx;    /* Nova posição */
    int novoY = (*cobrinha)->y + dy;
    
    if (telaMap[novoY][novoX] == '#' || telaMap[novoY][novoX] == 'o')           // Colisão de morte
        return 0; // Game Over
        
    if (novoX <= -1)                                                            /* Colisão com os portais */
        novoX = j->L_MAP - 2;
    else if (novoX >= j->L_MAP - 1)
        novoX = 0;
    
    Cobra* novaCabeca = malloc(sizeof(Cobra));  /* Movimento válido             - nova cabeça */
    novaCabeca->x = novoX;
    novaCabeca->y = novoY;
    novaCabeca->prox = *cobrinha;
    *cobrinha = novaCabeca;

    if (telaMap[novoY][novoX] == '@') {
        j->maca = 0; // Recriar maçã
        j->score++;
        CarregarMaca(j); 
    } else {
        // Remover cauda
        Cobra* atual = *cobrinha;
        while (atual->prox && atual->prox->prox)
            atual = atual->prox;
        free(atual->prox);
        atual->prox = NULL;
    }                                /* Colisão com Maçã             ? Recria a Maçã : Remove a calda */  

    return 1;
}
void LiberarCobra(Cobra* cabeca) {
    Cobra* atual = cabeca;
    while (atual != NULL) {
        Cobra* proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
}