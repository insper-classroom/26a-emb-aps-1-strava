#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "image_bitmap.h"

// Propriedades do LCD
#define SCREEN_ROTATION 1           
const int width = 320;             
const int height = 240;     

// Posição da imagem na tela
const int  rotImgPosX = (width - 32) / 2;
const int  rotImgPosY = (height - 24) / 2;
const int  startImgPosX = (width - 48) / 2;
const int  startImgPosY = (height - 48) / 2;

const int BTNS[] = {11, 27, 12, 26}; // 0:Red, 1:Blue, 2:Green, 3:Yellow
const int LEDS[] = {21, 10, 9, 13}; 

#define MAX_SEQ 100
int sequencia[MAX_SEQ];
int tamanho_seq = 0;
int indice_jogador = 0;
bool jogo_iniciado = false;

volatile int botao_clicado = -1;

int direcao = 1; 


void drawImagem(int estado) {
    gfx_fillRect(startImgPosX, startImgPosY, 48, 48, 0x0000);
    if (direcao == 1){ //horario
        if (estado == 0)
        gfx_drawBitmap(startImgPosX, startImgPosY, start, 48, 48, 0xFFFF); // start > 48,48
    else if(estado == 1)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, horario_2, 32, 24, 0xFFFF); //2 > 32,24
    else if (estado == 2)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, horario_3, 24, 32, 0xFFFF); //3 > 24,32
    else if (estado == 3)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, horario_4, 32, 24, 0xFFFF); //4 > 32,24
        
    }
    else if (direcao == -1){ //æntihorario
        if (estado == 0)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, anti_horario_4, 32, 24, 0xFFFF); //1 > 24,32
    else if(estado == 1)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, anti_horario_3, 24, 32, 0xFFFF); //2 > 32,24
    else if (estado == 2)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, anti_horario_2, 32, 24, 0xFFFF); //3 > 24,32
    else if (estado == 3)
        gfx_drawBitmap(rotImgPosX, rotImgPosY, anti_horario_1, 24, 32, 0xFFFF); //4 > 32,24
    }
    
    
}

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
        // Atualiza a imagem na tela pra cada cor
        
        // Acende o LED correspondente
        acende_feedback(sequencia[i], 600); // 600ms = LED mais visível
        sleep_ms(200); // Intervalo entre cores da sequência
    }
    // Volta pra posição inicial apos mostrar sequencia
    
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
    printf("Sequencia: %d\n", sequencia);
    mostra_sequencia();
}

int main() {
    stdio_init_all();

    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);  

    //### GFX
    gfx_init();                         
    gfx_clear();                        

    gfx_setTextSize(2);                                 
    gfx_setTextColor(0x07E0);                          

        const char *start_text = "Aperte o botao vermelho";
        const int startTextPosX = (width - gfx_getTextWidth(start_text)) / 2;
        const int startTextPosY = rotImgPosY - 40;
    
        gfx_drawText(
            startTextPosX, // Posição horizontal do texto
            startTextPosY, // Posição vertical do texto
            start_text // Texto a ser exibido
    );
    int img =0;
    
    drawImagem(img);

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
                // Mostra a cor na tela
                
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
                            
                            sleep_ms(200);
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
                    // Volta pra tela inicial
                    
                    botao_clicado = -1;
                }
            }
        }
        tight_loop_contents();
    }
}