#ifndef DEFS_H
#define DEFS_H

// ======== Estruturas ========

typedef struct Cobra {
    int x, y;   // posição na matriz
    struct Cobra* prox;
} Cobra;
typedef struct {
    char direcao;   // Direção da cobra
    int dific;      // Dificudade do Jogo

    int maca;       // 0 = não existe, 1 = existe
    int maca_x;     /* Posição da maçã */
    int maca_y;

    int score;      // Pontuação

    int A_MAP;      // altura atual do mapa
    int L_MAP;      // largura atual do mapa
} Jogo;

// ======== Terminal ========

void configurarModoRaw(void);
void restaurarModoNormal(void);
int kbhit(void);

// ======== Telas ========

#define A_LOB 6
#define L_LOB 30

#define A_MAP1 11
#define A_MAP23 16
#define L_MAP23 45

void VerTelaLob(int, Jogo*);
void CarregarTelaLob(Jogo*);

void VerTelaJogo(Jogo*);
void CarregarTelaJogo(Cobra*, Jogo*);

void VerTelaOver(Jogo*, int);

void ResetarTelaMapAnterior(void);

// ======== Funções de jogo ========

void CarregarMaca(Jogo*);

Cobra* CriarCobra(Jogo*);
void CarregarCobra(Cobra*);
int MoverCobra(Cobra**, Jogo*);
void LiberarCobra(Cobra*);

#endif