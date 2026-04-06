#ifndef START_H
#define START_H

#include <stdint.h>

// Fallback de audio de inicio: silencio curto para manter a interface do jogo.
#define START_WAV_LENGTH 1
static const uint8_t START_WAV_DATA[START_WAV_LENGTH] = {0};

#endif
