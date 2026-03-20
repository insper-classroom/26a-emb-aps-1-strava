#ifndef AUDIO_H
#define AUDIO_H 

#define AUDIO_PIN = 26;
void audio_init(void);

void tocar_som(const uint8_t *novo_audio, int tamanho);

#endif