/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"


const int BTN_PIN_RED = 17; 
const int BTN_PIN_BLUE = 14; 
const int BTN_PIN_GREEN = 26; //preto
const int BTN_PIN_YELLOW = 16;

const int LED_PIN_RED = 15;
const int LED_PIN_BLUE = 20;
const int LED_PIN_GREEN = 13;
const int LED_PIN_YELLOW = 18;

volatile bool seed_timer_fired = false;

volatile int flag_red = 0;
volatile int flag_blue = 0;
volatile int flag_green = 0;
volatile int flag_yellow = 0;

int64_t seed_alarm_callback(alarm_id_t id, void *user_data){
    seed_timer_fired = true;
    return 0;
}

void btn_callback(uint gpio, uint32_t event_mask) {
    
    if (gpio == BTN_PIN_BLUE){
        flag_blue = 1;
    }
    else if (gpio == BTN_PIN_GREEN){
        flag_green = 1;
    }
    else if (gpio == BTN_PIN_YELLOW){
        flag_yellow = 1;
    }
    else if (gpio == BTN_PIN_RED){
        flag_red = 1;
    }
        
    
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);

    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);

    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    gpio_init(LED_PIN_YELLOW);
    gpio_set_dir(LED_PIN_YELLOW, GPIO_OUT);
    

    gpio_init(BTN_PIN_RED);
    gpio_set_dir(BTN_PIN_RED, GPIO_IN);
    gpio_pull_up(BTN_PIN_RED);

    gpio_init(BTN_PIN_BLUE);
    gpio_set_dir(BTN_PIN_BLUE, GPIO_IN);
    gpio_pull_up(BTN_PIN_BLUE);

    gpio_init(BTN_PIN_GREEN);
    gpio_set_dir(BTN_PIN_GREEN, GPIO_IN);
    gpio_pull_up(BTN_PIN_GREEN);

    gpio_init(BTN_PIN_YELLOW);
    gpio_set_dir(BTN_PIN_YELLOW, GPIO_IN);
    gpio_pull_up(BTN_PIN_YELLOW);

    gpio_set_irq_enabled_with_callback(BTN_PIN_RED, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_set_irq_enabled(BTN_PIN_BLUE, GPIO_IRQ_EDGE_FALL, true);

    gpio_set_irq_enabled(BTN_PIN_GREEN, GPIO_IRQ_EDGE_FALL, true);

    gpio_set_irq_enabled(BTN_PIN_YELLOW, GPIO_IRQ_EDGE_FALL, true);

    // uint32_t start_ms = to_ms_since_boot(get_absolute_time());

    while (true) {
        

        if (flag_red){
            printf("Red button pressed!\n");
            flag_red = 0;
            gpio_put(LED_PIN_RED, 1);
            // uint32_t end_ms = to_ms_since_boot(get_absolute_time());
            // uint32_t seed_time = end_ms - start_ms;
            // printf("Seed time: %d ms\n", seed_time);
        }
        else if (flag_blue){
            printf("Blue button pressed!\n");
            flag_blue = 0;
            gpio_put(LED_PIN_BLUE, 1);
        }
        else if (flag_green){
            printf("Green button pressed!\n");
            flag_green = 0;
            gpio_put(LED_PIN_GREEN, 1);
        }
        else if (flag_yellow){
            printf("Yellow button pressed!\n");
            flag_yellow = 0;
            gpio_put(LED_PIN_YELLOW, 1);
        }
    

        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
