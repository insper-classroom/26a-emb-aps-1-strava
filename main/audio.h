#ifndef AUDIO_H
#define AUDIO_H 

#include <stdint.h>

// GPIO26 ja e usado pelos botoes/touch; audio configurado no GPIO2.
#define AUDIO_PIN 2

void audio_init(void);

void tocar_som(const uint8_t *novo_audio, int tamanho);
void parar_som(void);

#endif