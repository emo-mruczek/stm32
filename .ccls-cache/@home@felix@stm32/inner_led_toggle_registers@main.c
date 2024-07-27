//TODO: more info
#include "main.h"

int main(void) {

    /* enable clock for ports: A and C */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
 
    // zeroing cuz some bits are already set
    // mind its in output mode
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;    // clear both CNF13[1:0] bits -> make it 00 push-pull
     GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;

    GPIOC->CRH |= GPIO_CRH_MODE13_0;  // Set MODE13[0]-bit
    
    /* A0 pin -> mind CRL! */
    //  pull-up  pull-down //
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk; // same as up, zeroing
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // 
    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;
    GPIOA->ODR |= GPIO_ODR_ODR0; 


    while(1) {
        uint32_t gpioa_input = GPIOA->IDR & GPIO_IDR_IDR0;
        
       
        /* is pressed? */
        if (!gpioa_input) {
            uint32_t count = 10000;

            /* silly debouncer */
            while (count--) {
                __asm__("nop");
            }

            if(!(GPIOA->IDR  & GPIO_IDR_IDR0 )) {
                GPIOC->ODR ^= GPIO_ODR_ODR13;
                while (!(GPIOA->IDR  & GPIO_IDR_IDR0));
            }
        }
     }
}
