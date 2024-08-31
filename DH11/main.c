/* this is going to be a program that would read temperature value from dh11 */
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

#define DHT_set     ( GPIOB->BSRR = GPIO_BSRR_BS12 )
#define DHT_reset   ( GPIOB->BSRR = GPIO_BSRR_BR12 )

void print_value(uint8_t value);
void initialize_gpios(void);
void configure_clock(void);
void delay(uint16_t ms);
void initialize_debug(void);
void run_debug();

volatile uint8_t debug = 0;
volatile uint32_t ticks = 0;

int main() {
    configure_clock();
    initialize_gpios();
    initialize_debug(); 

    if ( debug ) run_debug();

    // output for initialization
    GPIOB->CRH &= ~GPIO_CRH_MODE12_Msk;
    GPIOB->CRH |= GPIO_CRH_MODE12_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF_Msk;

    /* "Data Single-bus free status is at high voltage level. 
     * When the communication between MCU and
     * DHT11 begins, the programme of MCU will set Data Single-bus voltage level from high to low
     * and this process must take at least 18ms to ensure DHT’s detection of MCU's signal,
     * then MCU will pull up voltage and wait 20-40us for DHT’s response" */

    DHT_reset;
    delay(20000);
    DHT_set;
    delay(40);

    // changing to input mode to receive data
    GPIOB->CRH &= ~GPIO_CRH_MODE12_Msk; 
    // cnf already set to input with pull-up/pull-down
    DHT_reset;

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

void configure_clock(void) {
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;     

    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    SysTick_Config(64);
}

void initialize_debug(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    debug = 1;
}

void run_debug(void) {
    if ( debug ) {
        for ( uint8_t i = 0; i < 4; ++i ) {
            GPIOC->ODR ^= GPIO_ODR_ODR13;
            delay(100);
        }
    }
}

void delay(uint16_t us) {
    uint32_t ticks_after_delay = ticks + us;
    while ( ticks < ticks_after_delay ) __asm__("nop");
}

void SysTick_Handler(void) {
    ticks += 1;
}
