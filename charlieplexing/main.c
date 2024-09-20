/* basic charlieplexing made for learning purposes */

/* for this, i need to be able to set pins to each of the three states: 
* output push-pull, that can drive line to the low and a high state, and output open-drain, that, when set to 1, is in high-impedance state, which is our third state */

#include "stm32f103x6.h"

#define HIGH 1
#define LOW 2
#define OPEN_DRAIN 3
#define NO_OF_DIODES 6

void configure_clock(void);
void configure_gpios(void);
void set_pin(uint8_t pin, uint8_t mode);
void delay(uint16_t s);

volatile uint32_t ticks = 0;

typedef struct diode {
    uint8_t pin_1;
    uint8_t pin_2;
    uint8_t pin_3;
} diode;

volatile diode diodes[NO_OF_DIODES] = {
    { OPEN_DRAIN, HIGH, LOW },
    { OPEN_DRAIN, LOW, HIGH },
    { HIGH, OPEN_DRAIN, LOW },
    { LOW, OPEN_DRAIN, HIGH },
    { LOW, HIGH, OPEN_DRAIN },
    { HIGH, LOW, OPEN_DRAIN }
};

int main() {

    configure_clock();
    configure_gpios();

    uint8_t i = 0;
    while (1) {
        set_pin(5, diodes[i].pin_1);
        set_pin(6, diodes[i].pin_2);
        set_pin(7, diodes[i].pin_3);
        delay(100);
        i = (i + 1) % NO_OF_DIODES;
    }
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

    SysTick_Config(64000);
}

void delay(uint16_t s) {
    uint32_t ticks_after_delay = ticks + s;
    while ( ticks < ticks_after_delay ) __asm__("nop");
}

void SysTick_Handler(void) {
    ticks += 1;
}
