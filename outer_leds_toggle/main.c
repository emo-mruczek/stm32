
#include "main.h"

typedef enum LED {
    NONE,
    GREEN,
    RED,
    BLUE,
} LED;

int main(void) {

    // enable clock for ports: A and C 
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
 
    // mind its in output mode
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; // clear mode bits
    GPIOC->CRH |= GPIO_CRH_MODE13_0;  // Set MODE13[0]-bit -> now is 01, in output mode
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; // clear both CNF13[1:0] bits -> make it 00 push-pull
    
    // A0 pin -> mind CRL! //
    //  pull-up  pull-down //
    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;// zeroing (unnecesary), to make it input 00
    GPIOA->CRL &= ~GPIO_CRL_CNF0_Msk; // same as up, zeroing
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // as the pin is in input mode, 10 means "input with pull-up/pull-down"
    GPIOA->ODR |= GPIO_ODR_ODR0; // just resetting output data register
    
    // adding LED on pins: B12, B13, B14 //
    GPIOB->CRH &= ~GPIO_CRH_MODE12_Msk;
    GPIOB->CRH &= ~GPIO_CRH_MODE13_Msk;
    GPIOB->CRH &= ~GPIO_CRH_MODE14_Msk;
    GPIOB->CRH |= GPIO_CRH_MODE12_0;
    GPIOB->CRH |= GPIO_CRH_MODE12_1;
    GPIOB->CRH |= GPIO_CRH_MODE13_0;
    GPIOB->CRH |= GPIO_CRH_MODE13_1;
    GPIOB->CRH |= GPIO_CRH_MODE14_0;
    GPIOB->CRH |= GPIO_CRH_MODE14_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF12_Msk;
    GPIOB->CRH &= ~GPIO_CRH_CNF13_Msk;
    GPIOB->CRH &= ~GPIO_CRH_CNF14_Msk;

    LED what_led = NONE;

    while(1) {
        uint32_t gpioa_input = GPIOA->IDR & GPIO_IDR_IDR0; // idr is read-only
        // checking the bit with AND (something AND 1)
       
       
        // is pressed? 
        if (!gpioa_input) {
            uint32_t count = 10000;

            // silly debouncer 
            while (count--) {
                __asm__("nop");
            }

            if(!(GPIOA->IDR  & GPIO_IDR_IDR0 )) {
                GPIOC->ODR ^= GPIO_ODR_ODR13; // finally, toggle
                what_led = (what_led + 1) % 4;

                switch (what_led) {
                    case GREEN: {
                        GPIOB->BSRR = GPIO_BSRR_BS12; // i think set/reset is more suitable here
                        GPIOB->BSRR = GPIO_BSRR_BR13;
                        GPIOB->BSRR = GPIO_BSRR_BR14;
                        break;
                    }
                    case RED: {
                        GPIOB->BSRR = GPIO_BSRR_BR12;
                        GPIOB->BSRR = GPIO_BSRR_BS13;
                        GPIOB->BSRR = GPIO_BSRR_BR14;
                        break;
                    }
                    case BLUE: {
                        GPIOB->BSRR = GPIO_BSRR_BR12;
                        GPIOB->BSRR = GPIO_BSRR_BR13;
                        GPIOB->BSRR = GPIO_BSRR_BS14;
                        break;
                    }
                    default: {
                        GPIOB->BSRR = GPIO_BSRR_BR12;
                        GPIOB->BSRR = GPIO_BSRR_BR13;
                        GPIOB->BSRR = GPIO_BSRR_BR14;
                       // what_led = NONE;
                        break;
                    }
                }
               /* GPIOB->ODR ^= GPIO_ODR_ODR12;
                GPIOB->ODR ^= GPIO_ODR_ODR13;
                GPIOB->ODR ^= GPIO_ODR_ODR14;*/
                while (!(GPIOA->IDR  & GPIO_IDR_IDR0));
            }
        }
     }
}


/*int main(void)
{
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;

     RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk; // clear mode bits
    GPIOC->CRH |= GPIO_CRH_MODE13_0;  // Set MODE13[0]-bit -> now is 01, in output mode
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk; // clear both CNF13[1:0] bits -> make it 00 push-pull


	GPIOB->CRH|= GPIO_CRH_MODE12_Msk;
    GPIOA->CRL |= GPIO_CRL_MODE0_0;
    GPIOA->CRL |= GPIO_CRL_MODE0_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF12_Msk;

	while(1)
	{
		GPIOB->ODR ^= GPIO_ODR_ODR12;
        GPIOC->ODR ^= GPIO_ODR_ODR13;
		for(int i=0;i<100000;i++) __asm__("nop");
		GPIOB->ODR ^= GPIO_ODR_ODR12;
        GPIOC->ODR ^= GPIO_ODR_ODR13;
		for(int i=0;i<100000;i++) __asm__("nop");

	}
}*/
