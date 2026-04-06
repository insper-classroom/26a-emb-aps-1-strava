#ifndef STOP_H
#define STOP_H

#include <stdint.h>

// Fallback de audio de game over: silencio curto para manter a interface do jogo.
#define STOP_WAV_LENGTH 1
static const uint8_t STOP_WAV_DATA[STOP_WAV_LENGTH] = {0};

#endif
