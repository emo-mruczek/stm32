/* with a help of 
 * https://controllerstech.com/interface-lcd-16x2-with-stm32-without-i2c/
 * https://deepbluembedded.com/stm32-lcd-16x2-tutorial-library-alphanumeric-lcd-16x2-interfacing/ 
 * and some random LCD1602 datasheet for hitachi controller, idk i bought lcd on aliexpress */

#include "stm32f103x6.h" 

#define D4_set ( GPIOA->BSRR = GPIO_BSRR_BS4 )
#define D5_set ( GPIOA->BSRR = GPIO_BSRR_BS5 )
#define D6_set ( GPIOA->BSRR = GPIO_BSRR_BS6 )
#define D7_set ( GPIOA->BSRR = GPIO_BSRR_BS7 )
#define D4_reset ( GPIOA->BSRR = GPIO_BSRR_BR4 )
#define D5_reset ( GPIOA->BSRR = GPIO_BSRR_BR5 )
#define D6_reset ( GPIOA->BSRR = GPIO_BSRR_BR6 )
#define D7_reset ( GPIOA->BSRR = GPIO_BSRR_BR7 )
#define RS_set ( GPIOA->BSRR = GPIO_BSRR_BS1 )
#define RS_reset ( GPIOA->BSRR = GPIO_BSRR_BR1 )
#define EN_set ( GPIOA->BSRR = GPIO_BSRR_BS3 )
#define EN_reset ( GPIOA->BSRR = GPIO_BSRR_BR3 )
#define send_data 1
#define send_command 0

// ngl im debugging this using leds...
#define delay_time 500

void configure_clock(void);
void configure_gpios(void);
void start_timer(uint16_t ms);
void stop_timer();
void delay(uint16_t ms);
void send_to_lcd(uint8_t data, uint8_t type);
void send_nibble(uint8_t data, uint8_t type);
void initialize_lcd(void);
void initialize_debug();
void put_cursor(uint8_t row, uint8_t col);
void write(char* message);
void run_debug();

volatile uint8_t debug = 0;
volatile uint32_t ticks = 0;

int main(void) {
    configure_clock();
    initialize_debug();

    run_debug();

    configure_gpios();
    initialize_lcd();

    // write something
    put_cursor(0, 0);
    send_to_lcd(0x08, send_command);
}

void write(char* message) {
    while (*message) send_to_lcd(*message++, send_data);

    run_debug();
}

void initialize_lcd(void) {

    // taken from datasheet 
    // delays just-in-case, with my clock speed they technically shouldnt be necessary
    send_to_lcd(0x00, send_command);
    delay(delay_time);
    send_to_lcd(0x30, send_command);
    delay(delay_time);
    send_to_lcd(0x30, send_command);
    delay(delay_time);
    send_to_lcd(0x30, send_command);
    delay(delay_time);

    // 4-bit mode
    send_to_lcd(0x20, send_command);
    delay(delay_time);

    // writing it like that makes sense, because im sending it as nibbles

    // N = 1: 2 lines, N = 0: 1 line
    // F = 1: 5 × 10 dots, F = 0: 5 × 8 dots
    send_to_lcd(0x28, send_command);
    delay(delay_time);
    send_to_lcd(0x08, send_command);
    delay(delay_time);
    send_to_lcd(0x01, send_command);
    delay(delay_time);

    // I/D = 1: Increment
    // I/D = 0: Decrement
    // S = 1: Accompanies display shift
    send_to_lcd(0x06, send_command);
    delay(delay_time);
    
    // initialization finished

    // turn on the display
    send_to_lcd(0x0C, send_command);
    delay(delay_time);
    
    run_debug();
}

void put_cursor(uint8_t row, uint8_t col) {
    // "set DDRAM address or cursor position on display 0x80 + add"
    // "80 Force cursor to the beginning (1st line)"
    // "C0 Force cursor to the beginning (2nd line)"
    col |= ( row ?  0xC0 : 0x80 );

    send_to_lcd(col, send_command);
    
    run_debug();
}

// okay so in 4-bit mode i need to send data in nibbles, thus this function
void send_to_lcd(uint8_t data, uint8_t type) {
    // 0000xxxx & 1111
    send_nibble( (data >> 4) & 0x0f, type );
    delay(delay_time);
    send_nibble( data & 0x0f, type);
}

// char looks ok to use
void send_nibble(uint8_t data, uint8_t type) {
    // command or data?
    type ? RS_set : RS_reset;

    // set/reset pins
    ( ( data >> 3 ) & 0x01 ) ? D7_set : D7_reset;
    ( ( data >> 2 ) & 0x01 ) ? D6_set : D6_reset;
    ( ( data >> 1 ) & 0x01 ) ? D5_set : D5_reset;
    ( ( data >> 0 ) & 0x01 ) ? D4_set : D4_reset;

    // enable tick
    // delay should be ok, my clock speed is not very high
    EN_set;
    delay(delay_time);
    EN_reset;
    delay(delay_time);
}

void delay(uint16_t ms) {
    uint32_t ticks_after_delay = ticks + ms;

    while ( ticks < ticks_after_delay ) __asm__("nop");
}

void SysTick_Handler(void) {
    ticks += 1;
}

void configure_clock(void) {
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;     

    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    RCC->CR |= RCC_CR_PLLON;
    while ( !(RCC->CR & RCC_CR_PLLRDY) ) {};

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ) {};

    // ive spent ungodly amount of time to try to setup timer... systick is perfect
    SysTick_Config(64000);
}

void initialize_debug(void) {
    // yes, its just pc13...
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
    GPIOC->CRH |= GPIO_CRH_MODE13_0;
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    debug = 1;
}

void configure_gpios(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ( ~GPIO_CRL_MODE1_Msk & ~GPIO_CRL_MODE2_Msk & ~GPIO_CRL_MODE3_Msk & ~GPIO_CRL_MODE4_Msk & ~GPIO_CRL_MODE5_Msk & ~GPIO_CRL_MODE6_Msk & ~GPIO_CRL_MODE7_Msk );
    GPIOA->CRL |= ( GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1 | GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 |  GPIO_CRL_MODE7_1 );
    GPIOA->CRL &= ( ~GPIO_CRL_CNF1_Msk & ~GPIO_CRL_CNF2_Msk & ~GPIO_CRL_CNF3_Msk & ~GPIO_CRL_CNF4_Msk & ~GPIO_CRL_CNF5_Msk & ~GPIO_CRL_CNF6_Msk & ~GPIO_CRL_CNF7_Msk );

    run_debug();
}

void run_debug(void) {
    if ( debug ) {
        for ( uint8_t i = 0; i < 4; ++i ) {
            GPIOC->ODR ^= GPIO_ODR_ODR13;
            delay(100);
        }
    }
}
