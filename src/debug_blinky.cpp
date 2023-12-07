
#include <stdio.h>
#include <stdint.h>
#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <pico/platform.h>
#include "hardware/gpio.h"

int count = 0;
const int LED_PIN = 25;
int main()
{
    long stack_size = PICO_STACK_SIZE;  // this is not a bug - it compiles There is something wrong with c_cpp_properties.json
                                        // but I cannot find it.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();
    // stdio_uart_init();
    while(1) {
        count++;
        gpio_put(LED_PIN, 0);
        printf("Led is off count: %d\n", count);
        sleep_ms(1000);
        gpio_put(LED_PIN,1);
        printf("Led is on count:%d\n", count);
        sleep_ms(1000);
    }
}
