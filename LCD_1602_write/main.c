/* with a help of 
 * https://controllerstech.com/interface-lcd-16x2-with-stm32-without-i2c/
 * https://deepbluembedded.com/stm32-lcd-16x2-tutorial-library-alphanumeric-lcd-16x2-interfacing/  * and some random LCD1602 datasheet */

/* this is a gpio hell... i need to clean it up! */

#include "stm32f103x6.h" 

#define D4_set ( GPIOA->BSRR |= GPIO_BSRR_BS4 )
#define D5_set ( GPIOA->BSRR |= GPIO_BSRR_BS5 )
#define D6_set ( GPIOA->BSRR |= GPIO_BSRR_BS6 )
#define D7_set ( GPIOA->BSRR |= GPIO_BSRR_BS7 )
#define D4_reset ( GPIOA->BSRR |= GPIO_BSRR_BR4 )
#define D5_reset ( GPIOA->BSRR |= GPIO_BSRR_BR5 )
#define D6_reset ( GPIOA->BSRR |= GPIO_BSRR_BR6 )
#define D7_reset ( GPIOA->BSRR |= GPIO_BSRR_BR7 )
#define RS_set ( GPIOA->BSRR |= GPIO_BSRR_BS1 )
#define RS_reset ( GPIOA->BSRR |= GPIO_BSRR_BR1 )
#define EN_set ( GPIOA->BSRR |= GPIO_BSRR_BS3 )
#define EN_reset ( GPIOA->BSRR |= GPIO_BSRR_BR3 )
#define send_data 1
#define send_command 0

void configure_clock(void);
void configure_gpios(void);
void start_timer(uint16_t ms);
void delay(uint16_t ms);
void to_lcd(char data, char type);

int main(void) {

    configure_clock();
    configure_gpios();
}

void delay(uint16_t ms) {
    
    start_timer(ms);

    // so im waiting for interrupt, basically
    while (!(TIM3->SR & TIM_SR_UIF)) {};
    TIM3->SR &= ~TIM_SR_UIF; // need to reset it manualy
}

// nothing new
void start_timer(uint16_t ms) {
    
    // disabling counter 
    TIM3->CR1 &= ~TIM_CR1_CEN;

    // reseting the timer 
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

    // like, there are 100ms in a second
    // prescaler 
    TIM3->PSC = 64000000 / 1000 - 1;

    TIM3->ARR = ms;

    // event generation register, update generation 
    TIM3->EGR |= TIM_EGR_UG;

    // enable interrupt 
    TIM3->DIER |= TIM_DIER_UIE; // update interrupt enable

    // finally, enable timer 
    TIM3->CR1 |= TIM_CR1_CEN; // counter enable 
}

// char looks ok to use
void to_lcd(char data, char type) {

    // command or data?
    type ? RS_set : RS_reset;

    // set/reset pins
    ( data >> 0 & 1 ) ? D4_set : D4_reset;
    ( data >> 1 & 1 ) ? D5_set : D5_reset;
    ( data >> 2 & 1 ) ? D6_set : D6_reset;
    ( data >> 3 & 1 ) ? D7_set : D7_reset;

    // enable tick
    EN_set;
    delay(20);
    EN_reset;
    delay(20);
}

// what a monstrosity 
void configure_gpios(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ( ~GPIO_CRL_MODE1_Msk & ~GPIO_CRL_MODE2_Msk & ~GPIO_CRL_MODE3_Msk & ~GPIO_CRL_MODE4_Msk & ~GPIO_CRL_MODE5_Msk & ~GPIO_CRL_MODE6_Msk & ~GPIO_CRL_MODE7_Msk );
    GPIOA->CRL |= ( GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1 | GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 |  GPIO_CRL_MODE7_1 );
    GPIOA->CRL &= ( ~GPIO_CRL_CNF1_Msk & ~GPIO_CRL_CNF2_Msk & ~GPIO_CRL_CNF3_Msk & ~GPIO_CRL_CNF4_Msk & ~GPIO_CRL_CNF5_Msk & ~GPIO_CRL_CNF6_Msk & ~GPIO_CRL_CNF7_Msk );  
}

// my typical clock configuration, nothing special, comments in other programs
void configure_clock(void) {

    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;     

    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}
