#include "audio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include <stddef.h>

#define AUDIO_PIN 26;

volatile const uint8_t *audio_atual = NULL;

volatile int tamanho_audio = 0;

volatile int wav_position = 0;

void pwm_interrupt_handler(void){
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));

    if (audio_atual != NULL){
        int pos = wav_position >> 3;

        if (pos < tamanho_audio){
            pwm_set_gpio_level(AUDIO_PIN,audio_atual[pos]);
            wav_position++;
        }
    } else {
        audio_atual = NULL;
        wav_position = 0;
        pwm_set_gpio_level(AUDIO_PIN,0);

    }
}

void audio_init(void){
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int slice_num = pwm_gpio_to_slice_num(AUDIO_PIN);
    
    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, 8.0f);

    pwm_config_set_wrap(&config, 250);

    pwm_init(slice_num, &config, true);
}

void tocar_som(const uint8_t *novo_audio, int tamanho){
    audio_atual = novo_audio;
    tamanho_audio = tamanho;
    wav_position = 0;
}