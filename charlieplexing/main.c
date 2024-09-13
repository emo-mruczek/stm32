/* basic charlieplexing made for learning purposes */

/* for this, i need to be able to set pins to each of the three states: 
* output push-pull, that can drive line to the low and a high state, and output open-drain, that, when set to 1, is in high-impedance state, which is our third state */

#include "stm32f103x6.h"

#define HIGH 1
#define LOW 2
#define OPEN_DRAIN 3

void configure_clock(void);
void configure_gpios(void);
void set_pin(uint8_t pin, uint8_t mode);

int main() {

    configure_clock();
    configure_gpios();


    // to set pin to open-drain, i need TO 1 <<  4 * PIN + 2
    // to set a pin high, i just need to 1 << PIN
    // to reset, 1 << ( 16 + PIN )

    set_pin(5, OPEN_DRAIN);
    set_pin(6, HIGH);
    set_pin(7, LOW);

    while(1) {};
}

/* change configuration of a pin
*  modes:
*  HIGH: push-pull high
*  LOW: push-pull low
*  OPEN_DRAIN: open-drain high
*  default: low
*/
void set_pin(uint8_t pin, uint8_t mode) {
    switch ( mode ) {
        case HIGH: {
            GPIOB->CRL &= ~( 1 << (4 * pin + 2) );
            GPIOB->BSRR |= ( 1 << pin );
            break;
        }
        case LOW: {
            GPIOB->CRL &= ~( 1 << (4 * pin + 2) );
            GPIOB->BSRR |= ( 1 << (16 + pin) );
            break;
        }
        case OPEN_DRAIN: {
            GPIOB->CRL |= ( 1 << (4 * pin + 2) );
            GPIOB->BSRR |= ( 1 << pin );
            break;
        }
        default: { 
            GPIOB->BSRR |= ( 1 << (16 + pin) );
        }
    }
}

void configure_gpios(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // i think that i wasn't doing it correclty in previous programs xD
    // GPIOB->CRL &= ~( GPIO_CRL_CNF5_Msk | GPIO_CRL_CNF4_Msk | GPIO_CRL_CNF3_Msk );
    GPIOB->CRL &= ~GPIO_CRL_CNF5_Msk;
    GPIOB->CRL &= ~GPIO_CRL_CNF6_Msk;
    GPIOB->CRL &= ~GPIO_CRL_CNF7_Msk;


    GPIOB->CRL |= ( GPIO_CRL_MODE7_0 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE5_0 );
}

void configure_clock(void) {
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;  

    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};
}
