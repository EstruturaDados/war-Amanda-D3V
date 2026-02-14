// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define NUM_TERRITORIOS 5
#define NUM_CORES_EXERCITO 3
#define NUM_MISSOES 2
#define TAM_MAX_NOME 50
#define TROPAS_INICIAIS 10
#define MISSAO_DESTRUIR_EXERCITO 0
#define MISSAO_CONQUISTAR_TERRITORIOS 1
#define TERRITORIOS_PARA_VENCER 3

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[TAM_MAX_NOME];
    int cor_exercito;  // 0 = Vermelho, 1 = Azul, 2 = Verde
    int num_tropas;
} Territorio;

// Estrutura para armazenar dados da missão
typedef struct {
    int tipo;  // 0 = destruir exército, 1 = conquistar N territórios
    int alvo;  // Se tipo 0: número da cor do exército; Se tipo 1: número de territórios
    char descricao[TAM_MAX_NOME * 2];
} Missao;

// --- Protótipos das Funções ---
// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa(void);
void inicializarTerritorios(Territorio *mapa);
void liberarMemoria(Territorio *mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int num_territorios);
void exibirMissao(const Missao *missao);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, int cor_jogador);
void simularAtaque(Territorio *mapa, int territorio_origem, int territorio_destino, int cor_jogador);
int sortearMissao(Missao *missao);
int verificarVitoria(const Territorio *mapa, int num_territorios, const Missao *missao, int cor_jogador);

// Função utilitária:
void limparBufferEntrada(void);

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main(void) {
    // 1. Configuração Inicial (Setup):
    // - Define o locale para português.
    // - Inicializa a semente para geração de números aleatórios com base no tempo atual.
    // - Aloca a memória para o mapa do mundo e verifica se a alocação foi bem-sucedida.
    // - Preenche os territórios com seus dados iniciais (tropas, donos, etc.).
    // - Define a cor do jogador e sorteia sua missão secreta.
    
    setlocale(LC_ALL, "pt_BR.UTF-8");
    srand((unsigned int)time(NULL));
    
    Territorio *mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro: Falha na alocação de memória para o mapa.\n");
        return 1;
    }
    
    inicializarTerritorios(mapa);
    
    int cor_jogador = rand() % NUM_CORES_EXERCITO;
    Missao missao;
    sortearMissao(&missao);
    
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                   BEM-VINDO AO JOGO WAR!                           ║\n");
    printf("║                                                                    ║\n");
    printf("║  Você é o comandante do exército: ");
    if (cor_jogador == 0) printf("VERMELHO");
    else if (cor_jogador == 1) printf("AZUL");
    else printf("VERDE");
    printf("                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n\n");
    
    // 2. Laço Principal do Jogo (Game Loop):
    // - Roda em um loop 'do-while' que continua até o jogador sair (opção 0) ou vencer.
    // - A cada iteração, exibe o mapa, a missão e o menu de ações.
    // - Lê a escolha do jogador e usa um 'switch' para chamar a função apropriada:
    //   - Opção 1: Inicia a fase de ataque.
    //   - Opção 2: Verifica se a condição de vitória foi alcançada e informa o jogador.
    //   - Opção 0: Encerra o jogo.
    // - Pausa a execução para que o jogador possa ler os resultados antes da próxima rodada.
    
    int opcao;
    int venceu = 0;
    
    do {
        exibirMapa(mapa, NUM_TERRITORIOS);
        exibirMissao(&missao);
        exibirMenuPrincipal();
        
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);
        limparBufferEntrada();
        
        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, cor_jogador);
                break;
            case 2:
                venceu = verificarVitoria(mapa, NUM_TERRITORIOS, &missao, cor_jogador);
                if (venceu) {
                    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
                    printf("║                    PARABÉNS! VOCÊ VENCEU!                           ║\n");
                    printf("║                   Missão cumprida com sucesso!                      ║\n");
                    printf("╚════════════════════════════════════════════════════════════════════╝\n");
                    opcao = 0;
                } else {
                    printf("\nMissão ainda não cumprida. Continue conquistando territórios!\n");
                    printf("Pressione ENTER para continuar...");
                    getchar();
                }
                break;
            case 0:
                printf("\nObrigado por jogar! Até a próxima!\n");
                break;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
                printf("Pressione ENTER para continuar...");
                getchar();
        }
    } while (opcao != 0 && !venceu);
    
    // 3. Limpeza:
    // - Ao final do jogo, libera a memória alocada para o mapa para evitar vazamentos de memória.
    liberarMemoria(mapa);
    
    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
Territorio* alocarMapa(void) {
    Territorio *mapa = (Territorio *)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio *mapa) {
    const char *nomes[] = {"América", "Europa", "Ásia", "África", "Oceania"};
    
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        strcpy(mapa[i].nome, nomes[i]);
        mapa[i].cor_exercito = rand() % NUM_CORES_EXERCITO;
        mapa[i].num_tropas = TROPAS_INICIAIS + (rand() % 6);
    }
}

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.
void exibirMenuPrincipal(void) {
    printf("\n┌────────────────────────────────────┐\n");
    printf("│         MENU PRINCIPAL             │\n");
    printf("├────────────────────────────────────┤\n");
    printf("│ 1 - Iniciar Fase de Ataque         │\n");
    printf("│ 2 - Verificar Vitória              │\n");
    printf("│ 0 - Sair do Jogo                   │\n");
    printf("└────────────────────────────────────┘\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.
void exibirMapa(const Territorio *mapa, int num_territorios) {
    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                         MAPA DO MUNDO                          ║\n");
    printf("╠════════════╦═════════════════╦════════════════╦════════════════╣\n");
    printf("║   ID       ║   Território    ║    Exército    ║    Tropas      ║\n");
    printf("╠════════════╬═════════════════╬════════════════╬════════════════╣\n");
    
    for (int i = 0; i < num_territorios; i++) {
        const char *cor = "";
        if (mapa[i].cor_exercito == 0) cor = "Vermelho";
        else if (mapa[i].cor_exercito == 1) cor = "Azul";
        else cor = "Verde";
        
        printf("║     %d     ║ %-15s ║ %-14s ║ %-14d ║\n", 
               i, mapa[i].nome, cor, mapa[i].num_tropas);
    }
    
    printf("╚════════════╩═════════════════╩════════════════╩════════════════╝\n");
}

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.
void exibirMissao(const Missao *missao) {
    printf("\n┌──────────────────────────────────────────────────────────────────┐\n");
    printf("│ MISSÃO SECRETA DO JOGADOR:                                       │\n");
    printf("├──────────────────────────────────────────────────────────────────┤\n");
    printf("│ %s\n", missao->descricao);
    printf("└──────────────────────────────────────────────────────────────────┘\n");
}

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.
void faseDeAtaque(Territorio *mapa, int cor_jogador) {
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    FASE DE ATAQUE                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n\n");
    
    exibirMapa(mapa, NUM_TERRITORIOS);
    
    int territorio_origem, territorio_destino;
    
    printf("\nDigite o ID do território de ORIGEM (seu exército): ");
    scanf("%d", &territorio_origem);
    limparBufferEntrada();
    
    if (territorio_origem < 0 || territorio_origem >= NUM_TERRITORIOS) {
        printf("\nTerritório inválido!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    if (mapa[territorio_origem].cor_exercito != cor_jogador) {
        printf("\nVocê não controla este território!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    printf("\nDigite o ID do território de DESTINO (inimigo): ");
    scanf("%d", &territorio_destino);
    limparBufferEntrada();
    
    if (territorio_destino < 0 || territorio_destino >= NUM_TERRITORIOS) {
        printf("\nTerritório inválido!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    if (territorio_origem == territorio_destino) {
        printf("\nVocê não pode atacar o seu próprio território!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }
    
    simularAtaque(mapa, territorio_origem, territorio_destino, cor_jogador);
    
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.
void simularAtaque(Territorio *mapa, int territorio_origem, int territorio_destino, int cor_jogador) {
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    SIMULAÇÃO DE BATALHA                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n\n");
    
    int dados_atacante = (rand() % 6) + 1;
    int dados_defensor = (rand() % 6) + 1;
    
    printf("Território atacante: %s (%d tropas)\n", 
           mapa[territorio_origem].nome, mapa[territorio_origem].num_tropas);
    printf("Território defendido: %s (%d tropas)\n", 
           mapa[territorio_destino].nome, mapa[territorio_destino].num_tropas);
    
    printf("\n--- Resultado dos Dados ---\n");
    printf("Dado do Atacante: %d\n", dados_atacante);
    printf("Dado do Defensor: %d\n", dados_defensor);
    
    if (mapa[territorio_origem].num_tropas < 2) {
        printf("\nVocê precisa de pelo menos 2 tropas para atacar!\n");
        printf("(1 fica no território de origem)\n");
        return;
    }
    
    if (dados_atacante > dados_defensor) {
        printf("\n✓ ATACANTE VENCE!\n");
        
        if (mapa[territorio_destino].num_tropas > 1) {
            mapa[territorio_destino].num_tropas--;
        } else {
            // Conquistar o território
            printf("\n*** TERRITÓRIO CONQUISTADO! ***\n");
            mapa[territorio_origem].num_tropas--;
            mapa[territorio_destino].num_tropas = 1;
            mapa[territorio_destino].cor_exercito = cor_jogador;
            printf("%s agora pertence ao seu exército!\n", mapa[territorio_destino].nome);
        }
    } else if (dados_defensor > dados_atacante) {
        printf("\n✗ DEFENSOR VENCE!\n");
        
        if (mapa[territorio_origem].num_tropas > 1) {
            mapa[territorio_origem].num_tropas--;
        } else {
            printf("\nVocê perdeu todas as tropas deste território!\n");
            mapa[territorio_origem].num_tropas = 0;
            mapa[territorio_origem].cor_exercito = -1;  // Neutro
        }
    } else {
        printf("\n= EMPATE!\n");
        printf("Nenhum dano foi causado.\n");
    }
}

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.
int sortearMissao(Missao *missao) {
    int tipo = rand() % 2;
    
    missao->tipo = tipo;
    
    if (tipo == MISSAO_DESTRUIR_EXERCITO) {
        const char *cores[] = {"Vermelho", "Azul", "Verde"};
        missao->alvo = rand() % NUM_CORES_EXERCITO;
        sprintf(missao->descricao, "Destruir o exército %s (dominar todos os seus territórios)", 
                cores[missao->alvo]);
    } else {
        missao->alvo = TERRITORIOS_PARA_VENCER;
        sprintf(missao->descricao, "Conquistar %d territórios diferentes", missao->alvo);
    }
    
    return tipo;
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
int verificarVitoria(const Territorio *mapa, int num_territorios, const Missao *missao, int cor_jogador) {
    if (missao->tipo == MISSAO_DESTRUIR_EXERCITO) {
        // Verificar se destruiu o exército alvo
        for (int i = 0; i < num_territorios; i++) {
            if (mapa[i].cor_exercito == missao->alvo) {
                // O exército alvo ainda existe
                return 0;
            }
        }
        // Exército alvo foi destruído
        return 1;
    } else if (missao->tipo == MISSAO_CONQUISTAR_TERRITORIOS) {
        // Contar territórios do jogador
        int territorios_conquistados = 0;
        for (int i = 0; i < num_territorios; i++) {
            if (mapa[i].cor_exercito == cor_jogador) {
                territorios_conquistados++;
            }
        }
        // Verificar se atingiu o objetivo
        return (territorios_conquistados >= missao->alvo) ? 1 : 0;
    }
    
    return 0;
}

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
