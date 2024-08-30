/* this is going to be a programm that would read temperature value from dh11 */
/* for now, i will use LEDs to show output */

#include "stm32f103x6.h"

#define bit_set_0   ( GPIOA->BSRR = GPIO_BSRR_BS12 )
#define bit_reset_0 ( GPIOA->BSRR = GPIO_BSRR_BR12 )
#define bit_set_1   ( GPIOA->BSRR = GPIO_BSRR_BS11 )
#define bit_reset_1 ( GPIOA->BSRR = GPIO_BSRR_BR11 )
#define bit_set_2   ( GPIOA->BSRR = GPIO_BSRR_BS10 )
#define bit_reset_2 ( GPIOA->BSRR = GPIO_BSRR_BR10 )
#define bit_set_3   ( GPIOA->BSRR = GPIO_BSRR_BS9 )
#define bit_reset_3 ( GPIOA->BSRR = GPIO_BSRR_BR9 )
#define bit_set_4   ( GPIOA->BSRR = GPIO_BSRR_BS8 )
#define bit_reset_4 ( GPIOA->BSRR = GPIO_BSRR_BR8 )

void print_value(uint8_t value);
void initialize_gpios(void);

int main() {

    initialize_gpios();
    print_value(21);
}


void print_value(uint8_t value) {
    value >> 0 & 0x01 ? bit_set_0 : bit_reset_0;
    value >> 1 & 0x01 ? bit_set_1 : bit_reset_1;
    value >> 2 & 0x01 ? bit_set_2 : bit_reset_2;
    value >> 3 & 0x01 ? bit_set_3 : bit_reset_3;
    value >> 4 & 0x01 ? bit_set_4 : bit_reset_4;
}

void initialize_gpios(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ( ~GPIO_CRH_MODE8_Msk & ~GPIO_CRH_MODE9_Msk & ~GPIO_CRH_MODE10_Msk & ~GPIO_CRH_MODE11_Msk & ~GPIO_CRH_MODE12_Msk );
    GPIOA->CRH |= ( GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_1 | GPIO_CRH_MODE10_1 | GPIO_CRH_MODE11_Msk | GPIO_CRH_MODE12_Msk );
    GPIOA->CRH &= ( ~GPIO_CRH_CNF8_Msk & ~GPIO_CRH_CNF9_Msk & ~GPIO_CRH_CNF10_Msk & ~GPIO_CRH_CNF11_Msk & ~GPIO_CRH_CNF12_Msk );
}
