/* comments via reference manual and cortex-M3 programming manual */

/* blinking PC13 LED using hardware interrupts */

#include "main.h"

#define BUTTON_PIN 0 // should use this probably, writting 1 << BUTTON_PIN is more readable

volatile unsigned char led_on = 0;

int main() {

    /* "To configure the AFIO_EXTICRx for the mapping of external interrupt/event lines 
     * onto GPIOs, the AFIO clock should first be enabled" */
    
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; 
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // for LED on PC13
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // for button on A0

    /* setting up stuff like in previous programs */
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; // clear mode bits
    GPIOC->CRH |= GPIO_CRH_MODE13_0;  // Set MODE13[0]-bit -> now is 01, in output mode
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; // clear both CNF13[1:0] bits -> make it 00 push-pull
    
    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;// zeroing (unnecesary), to make it input 00
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk; // same as up, zeroing
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // as the pin is in input mode, 10 means "input with pull-up/pull-down"
    GPIOA->ODR |= GPIO_ODR_ODR0; // just resetting output data register
   
    /* "To configure the 20 lines as interrupt sources, use the following procedure:
    • Configure the mask bits of the 20 Interrupt lines (EXTI_IMR)
    • Configure the Trigger Selection bits of the Interrupt lines (EXTI_RTSR and
      EXTI_FTSR)
    • Configure the enable and mask bits that control the NVIC IRQ channel mapped to the
      External Interrupt Controller (EXTI) so that an interrupt coming from one of the 20 lines
      can be correctly acknowledged." */
    
    /* for the button configured with a pull-up resistor, listen for a falling edge as a press */
    EXTI->IMR |= EXTI_IMR_MR0; // interrupt mask register for 0 pin
    EXTI->RTSR &= ~EXTI_RTSR_TR0; // rising trigger selection register (buttor released) DISABLE
    EXTI->FTSR |= EXTI_FTSR_TR0; // falling edge selection register (button pressed)
    
    /* void NVIC_SetPriority (IRQn_t IRQn, uint32_t priority) 
     * setting the irq piority */
    NVIC_SetPriority(EXTI0_IRQn, 3);

    /* void NVIC_EnableIRQ(IRQn_t IRQn)
     * enabling interrupt request handler */
    NVIC_EnableIRQ(EXTI0_IRQn);

    /* main loop, finally */
    while (1) {
        if ( led_on ) {
            GPIOC->ODR |= ( GPIO_ODR_ODR13 );
        } else {
            GPIOC->ODR &= ~( GPIO_ODR_ODR13 );
        }
    }
}

/* interrupt request handler, from vector table */
void EXTI0_IRQHandler(void) {
    if ( EXTI->PR & ( 1 << BUTTON_PIN ) ) {
        EXTI->PR |= ( 1 << BUTTON_PIN ); // clearing (yes, to 1)
        led_on = !led_on;
    }
}

/* i did not used software debouncer this time - ive managed to debounce the circuit using a 100nF capacitor and a 470Ohm resistor, same as here: https://vivonomicon.com/2018/04/22/bare-metal-stm32-programming-part-3-leds-and-buttons/ */
