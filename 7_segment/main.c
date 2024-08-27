#include "stm32f103x6.h"

#define A_set ( GPIOA->BSRR = GPIO_BSRR_BS0 )
#define B_set ( GPIOA->BSRR = GPIO_BSRR_BS1 )
#define C_set ( GPIOA->BSRR = GPIO_BSRR_BS2 )
#define D_set ( GPIOA->BSRR = GPIO_BSRR_BS3 ) 
#define E_set ( GPIOA->BSRR = GPIO_BSRR_BS4 )
#define F_set ( GPIOA->BSRR = GPIO_BSRR_BS5 ) 
#define G_set ( GPIOA->BSRR = GPIO_BSRR_BS6 ) 
#define DP_set ( GPIOA->BSRR = GPIO_BSRR_BS7 ) 
#define A_reset ( GPIOA->BSRR = GPIO_BSRR_BR0 )
#define B_reset ( GPIOA->BSRR = GPIO_BSRR_BR1 )
#define C_reset ( GPIOA->BSRR = GPIO_BSRR_BR2 )
#define D_reset ( GPIOA->BSRR = GPIO_BSRR_BR3 )
#define E_reset ( GPIOA->BSRR = GPIO_BSRR_BR4 )
#define F_reset ( GPIOA->BSRR = GPIO_BSRR_BR5 ) 
#define G_reset ( GPIOA->BSRR = GPIO_BSRR_BR6 ) 
#define DP_reset ( GPIOA->BSRR = GPIO_BSRR_BR7 )

// i dont really like the fact that i have a dot argument even in dot function???
void configure_gpios(void);
void set_num(const char* num, uint8_t dot);
void command_ZERO(uint8_t dot);
void command_ONE(uint8_t dot);
void command_TWO(uint8_t dot);
void command_THREE(uint8_t dot);
void command_FOUR(uint8_t dot);
void command_FIVE(uint8_t dot);
void command_SIX(uint8_t dot);
void command_SEVEN(uint8_t dot);
void command_EIGHT(uint8_t dot);
void command_NINE(uint8_t dot);
void command_DOT(uint8_t dot);
void command_BLANK(uint8_t dot);
void command_FULL(uint8_t dot);

// maybe i overdid it a little idk i just like function pointers
typedef void(*functionPointerType) (uint8_t dot);

typedef struct structNums {
    char const* type;
    functionPointerType pin_function;
} structNums;

const structNums nums[] = {
    { "ZERO", command_ZERO },
    { "ONE", command_ONE },
    { "TWO", command_TWO },
    { "THREE", command_THREE },
    { "FOUR", command_FOUR },
    { "FIVE", command_FIVE },
    { "SIX", command_SIX },
    { "SEVEN", command_SEVEN },
    { "EIGHT", command_EIGHT },
    { "NINE", command_NINE },
    { "DOT", command_DOT },
    { "BLANK", command_BLANK },
    { "FULL", command_FULL },
    { "", 0 } // end of commands
};


int main() {
    
    configure_gpios();

    //set_num("FULL", 0)
    set_num("SEVEN", 1);

}

void set_num(const char* num, uint8_t dot) {
    for(int i = 0; nums[i].pin_function; ++i) {
            if (nums[i].type == num) {
                nums[i].pin_function(dot);
                return;
        }
    }
   
    // if it doesnt exist just set blank
    command_BLANK(dot);
    return;
}


void configure_gpios(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ( ~GPIO_CRL_MODE0_Msk & ~GPIO_CRL_MODE1_Msk & ~GPIO_CRL_MODE2_Msk & ~GPIO_CRL_MODE3_Msk & ~GPIO_CRL_MODE4_Msk & ~GPIO_CRL_MODE5_Msk & ~GPIO_CRL_MODE6_Msk & ~GPIO_CRL_MODE7_Msk );
    GPIOA->CRL |= ( GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1 | GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1 | GPIO_CRL_MODE6_1 |  GPIO_CRL_MODE7_1 );
    GPIOA->CRL &= ( ~GPIO_CRL_CNF0_Msk & ~GPIO_CRL_CNF1_Msk & ~GPIO_CRL_CNF2_Msk & ~GPIO_CRL_CNF3_Msk & ~GPIO_CRL_CNF4_Msk & ~GPIO_CRL_CNF5_Msk & ~GPIO_CRL_CNF6_Msk & ~GPIO_CRL_CNF7_Msk );
}

void command_ZERO(uint8_t dot) {
    A_set;
    B_set;
    C_set;
    D_set;
    E_set;
    F_set;
    G_reset;
   dot ? DP_set : DP_reset;
}

void command_ONE(uint8_t dot) {
    A_reset;
    B_set;
    C_set;
    D_reset;
    E_reset;
    F_reset;
    G_reset;
    dot ? DP_set : DP_reset;
}

void command_TWO(uint8_t dot) {
    A_set;
    B_set;
    C_reset;
    D_set;
    E_set;
    F_reset;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_THREE(uint8_t dot) {
    A_set;
    B_set;
    C_set;
    D_set;
    E_reset;
    F_reset;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_FOUR(uint8_t dot) {
    A_reset;
    B_set;
    C_set;
    D_reset;
    E_reset;
    F_set;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_FIVE(uint8_t dot) {
    A_set;
    B_reset;
    C_set;
    D_set;
    E_reset;
    F_set;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_SIX(uint8_t dot) {
    A_set;
    B_reset;
    C_set;
    D_set;
    E_set;
    F_set;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_SEVEN(uint8_t dot) {
    A_set;
    B_set;
    C_set;
    D_reset;
    E_reset;
    F_reset;
    G_reset;
    dot ? DP_set : DP_reset;
}

void command_EIGHT(uint8_t dot) {
    A_set;
    B_set;
    C_set;
    D_set;
    E_set;
    F_set;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_NINE(uint8_t dot) {
    A_set;
    B_set;
    C_set;
    D_reset;
    E_reset;
    F_set;
    G_set;
    dot ? DP_set : DP_reset;
}

void command_DOT(uint8_t dot) {
    A_reset;
    B_reset;
    C_reset;
    D_reset;
    E_reset;
    F_reset;
    G_reset;
    // dot ? DP_set : DP_reset;
    DP_set;
}

void command_BLANK(uint8_t dot) {
    A_reset;
    B_reset;
    C_reset;
    D_reset;
    E_reset;
    F_reset;
    G_reset;
    // dot ? DP_set : DP_reset;
    DP_reset;
}

void command_FULL(uint8_t dot){
    A_set;
    B_set;
    C_set;
    D_set;
    E_set;
    F_set;
    G_set;
    // dot ? DP_set : DP_reset;
    DP_set;
}




