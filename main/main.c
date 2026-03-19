#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


const int BTNS[] = {17, 14, 26, 16}; // 0:Red, 1:Blue, 2:Green, 3:Yellow
const int LEDS[] = {15, 20, 13, 18}; 

#define MAX_SEQ 100
int sequencia[MAX_SEQ];
int tamanho_seq = 0;
int indice_jogador = 0;
bool jogo_iniciado = false;

volatile int botao_clicado = -1;

void apaga_leds() {
    for(int i=0; i<4; i++) gpio_put(LEDS[i], 0);
}

// Função para acender o LED com tempo ajustável
void acende_feedback(int idx, int tempo_ms) {
    gpio_put(LEDS[idx], 1);
    sleep_ms(tempo_ms);
    gpio_put(LEDS[idx], 0);
}

void mostra_sequencia() {
    sleep_ms(500); // Pequena pausa antes de começar a mostrar
    for (int i = 0; i < tamanho_seq; i++) {
        acende_feedback(sequencia[i], 600); // 600ms = LED mais visível
        sleep_ms(200); // Intervalo entre cores da sequência
    }
}

void btn_callback(uint gpio, uint32_t event_mask) {
    // Debounce simples: só aceita novo clique se o anterior foi processado
    if (botao_clicado != -1) return; 

    for(int i=0; i<4; i++) {
        if(gpio == BTNS[i]) {
            botao_clicado = i;
            break;
        }
    }
}

void iniciar_jogo() {
    printf("Iniciando...\n");
    // Usa o tempo de boot como semente para o aleatório
    srand(to_ms_since_boot(get_absolute_time()));
    tamanho_seq = 0;
    indice_jogador = 0;
    jogo_iniciado = true;
    
    sequencia[tamanho_seq++] = rand() % 4;
    mostra_sequencia();
}

int main() {
    stdio_init_all();

    for(int i=0; i<4; i++) {
        gpio_init(LEDS[i]);
        gpio_set_dir(LEDS[i], GPIO_OUT);
        
        gpio_init(BTNS[i]);
        gpio_set_dir(BTNS[i], GPIO_IN);
        gpio_pull_up(BTNS[i]);
        
        gpio_set_irq_enabled_with_callback(BTNS[i], GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    }

    while (true) {
        if (botao_clicado != -1) {
            int cor = botao_clicado;
            
            // Lógica de início (Botão Vermelho = Índice 0)
            if (!jogo_iniciado) {
                if (cor == 0) {
                    acende_feedback(0, 300);
                    iniciar_jogo();
                }
                botao_clicado = -1; 
            } 
            else {
                // Jogador apertou uma cor durante o jogo
                acende_feedback(cor, 400); // Feedback rápido do clique

                if (cor == sequencia[indice_jogador]) {
                    indice_jogador++;
                    botao_clicado = -1; // Libera para o próximo clique

                    if (indice_jogador == tamanho_seq) {
                        printf("Proximo nivel!\n");
                        indice_jogador = 0;
                        if(tamanho_seq < MAX_SEQ) {
                            sequencia[tamanho_seq++] = rand() % 4;
                            sleep_ms(400);
                            mostra_sequencia();
                        }
                    }
                } else {
                    printf("Erro!\n");
                    jogo_iniciado = false;
                    // Pisca tudo para avisar erro
                    for(int i=0; i<3; i++) {
                        for(int l=0; l<4; l++) gpio_put(LEDS[l], 1);
                        sleep_ms(150);
                        apaga_leds();
                        sleep_ms(150);
                    }
                    botao_clicado = -1;
                }
            }
        }
        tight_loop_contents();
    }
}