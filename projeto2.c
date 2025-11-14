#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constantes ---
#define COR_BRANCA 0
#define COR_PRETA  1

#define MAX_ALTURA 768
#define MAX_LARGURA 1024


// --- Estrutura da Imagem (TEM QUE VIR ANTES DOS PROTÓTIPOS) ---
typedef struct {
    int largura;
    int altura;
    int pixels[MAX_ALTURA][MAX_LARGURA];
} Imagem;


// --- Protótipos das Funções ---
void exibir_ajuda();
void processar_entrada_manual(Imagem *img);
void processar_entrada_arquivo(Imagem *img, const char *nome_arquivo);
int verificar_uniformidade(Imagem *img, int lin, int col, int alt, int larg);
void codificar_imagem(Imagem *img, int lin, int col, int alt, int larg);


// --- Função Principal ---
int main(int argc, char *argv[]) {
    Imagem img;
    int modo_manual = 0;
    char *nome_arquivo = NULL;

    if (argc == 1) {
        exibir_ajuda();
        return 0;
    }

    // Interpretar argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0) {
            exibir_ajuda();
            return 0;
        }
        if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--manual") == 0) {
            modo_manual = 1;
        }
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) {
                nome_arquivo = argv[i + 1];
                i++;
            } else {
                printf("Erro: falta o nome do arquivo após -f\n");
                return 1;
            }
        }
    }

    if (!modo_manual && nome_arquivo == NULL) {
        printf("Erro: use -m (manual) ou -f ARQUIVO\n");
        return 1;
    }

    // Carregar imagem
    if (modo_manual)
        processar_entrada_manual(&img);
    else
        processar_entrada_arquivo(&img, nome_arquivo);

    // Codificar
    printf("Código gerado: ");
    codificar_imagem(&img, 0, 0, img.altura, img.largura);
    printf("\n");

    return 0;
}


// --- Implementações das Funções ---

void exibir_ajuda() {
    printf("Uso: projeto2 [-? | -m | -f ARQ]\n");
    printf("Codifica imagens binárias PBM (P1) ou entrada manual.\n");
}

void processar_entrada_manual(Imagem *img) {
    printf("Digite largura e altura: ");
    if (scanf("%d %d", &img->largura, &img->altura) != 2) {
        printf("Erro ao ler dimensões.\n");
        exit(1);
    }

    if (img->largura <= 0 || img->largura > MAX_LARGURA ||
        img->altura <= 0 || img->altura > MAX_ALTURA) {
        printf("Dimensões inválidas.\n");
        exit(1);
    }

    printf("Digite os pixels (0 ou 1):\n");
    for (int i = 0; i < img->altura; i++) {
        for (int j = 0; j < img->largura; j++) {
            if (scanf("%d", &img->pixels[i][j]) != 1 ||
                (img->pixels[i][j] != 0 && img->pixels[i][j] != 1)) {
                printf("Pixel inválido.\n");
                exit(1);
            }
        }
    }
}

void processar_entrada_arquivo(Imagem *img, const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) {
        printf("Erro abrindo arquivo '%s'\n", nome_arquivo);
        exit(1);
    }

    char magic[3];
    if (fscanf(f, "%2s", magic) != 1 || strcmp(magic, "P1") != 0) {
        printf("Arquivo não é PBM P1 válido.\n");
        fclose(f);
        exit(1);
    }

    int c = fgetc(f);
    while (c == '#' ) {   // ignorar comentários
        while (c != '\n' && c != EOF)
            c = fgetc(f);
        c = fgetc(f);
    }
    ungetc(c, f);

    if (fscanf(f, "%d %d", &img->largura, &img->altura) != 2) {
        printf("Erro nas dimensões.\n");
        fclose(f);
        exit(1);
    }

    if (img->largura <= 0 || img->largura > MAX_LARGURA ||
        img->altura <= 0 || img->altura > MAX_ALTURA) {
        printf("Dimensões inválidas.\n");
        fclose(f);
        exit(1);
    }

    for (int i = 0; i < img->altura; i++) {
        for (int j = 0; j < img->largura; j++) {
            if (fscanf(f, "%d", &img->pixels[i][j]) != 1 ||
                (img->pixels[i][j] != 0 && img->pixels[i][j] != 1)) {
                printf("Erro lendo pixel.\n");
                fclose(f);
                exit(1);
            }
        }
    }

    fclose(f);
}

int verificar_uniformidade(Imagem *img, int lin, int col, int alt, int larg) {
    int cor = img->pixels[lin][col];
    for (int i = lin; i < lin + alt; i++) {
        for (int j = col; j < col + larg; j++) {
            if (img->pixels[i][j] != cor)
                return -1;
        }
    }
    return cor;
}

void codificar_imagem(Imagem *img, int lin, int col, int alt, int larg) {
    if (alt <= 0 || larg <= 0)
        return;

    int t = verificar_uniformidade(img, lin, col, alt, larg);

    if (t == COR_BRANCA) {
        printf("B");
        return;
    }
    if (t == COR_PRETA) {
        printf("P");
        return;
    }

    printf("X");

    int alt1 = (alt + 1) / 2;
    int alt2 = alt - alt1;

    int larg1 = (larg + 1) / 2;
    int larg2 = larg - larg1;

    // Ordem: Q1 Q2 Q3 Q4
    codificar_imagem(img, lin, col, alt1, larg1);
    codificar_imagem(img, lin, col + larg1, alt1, larg2);
    codificar_imagem(img, lin + alt1, col, alt2, larg1);
    codificar_imagem(img, lin + alt1, col + larg1, alt2, larg2);
}
