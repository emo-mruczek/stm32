#include "../libopencm3/include/libopencm3/stm32/rcc.h"
#include "../libopencm3/include/libopencm3/stm32/gpio.h"

#include <stdint.h>

int main(void) {
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);

    /* LED pin */
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    /* A0 pin */
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0);
    gpio_set(GPIOA, GPIO0);

    /* is the button pressed? */
    uint8_t button_down = 0;

    while (1) {

        /* reading from idr */

        uint32_t gpioa_input = gpio_get(GPIOA, GPIO0);

        /* better toggle!!! */
        /* is pressed? */
        if (!gpioa_input) {
            uint32_t count = 10000;

            /* silly debouncer */
            while (count--) {
                __asm__("nop");
            }

            /* is still pressed? */
            if (!gpio_get(GPIOA, GPIO0)) {

                /* finnaly, toggle! */
                gpio_toggle(GPIOC, GPIO13);

                /* wait until released! */
                while (!gpio_get(GPIOA, GPIO0));
            }
        }


        /********************* OTHER VERSIONS *********************/

        /* toggle! */
        // if (!gpioa_input) {

        /* stupid debouncer :(( */
        //        for (int i = 0; i < 100000; i++) {
        //            __asm__("nop");
        //       }
        //       gpio_toggle(GPIOC, GPIO13);
        //    }



        /* same as below */
        /*   if (gpioa_input) {
                 gpio_set(GPIOC, GPIO13);  // Turn on LED
         } else {
                gpio_clear(GPIOC, GPIO13);  // Turn off LED
               }*/


        /* light off when the button is pushed */
        /*if (gpioa_input) {
            if (button_down) {
                gpio_toggle(GPIOC, GPIO13);
            }
            button_down = 1;
        } else {
            button_down = 0;
        }*/
    }
}
