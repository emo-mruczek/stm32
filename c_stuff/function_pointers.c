/* fuction pointers for a commands on a hypotethical embedded system from "Making Embedded Systems" book */
#include <stdio.h>
#include <inttypes.h>

void commandVersion(void) {
    printf("v21.37\n");
}

void commandFlashTest(void) {
    printf("Everything' fine!\n");
}

void commandBlinkLED() {
    printf("Blinking the LED!\n");
    
   // printf("Blinking with a %" PRIu16 " Hz rate!", rate);
}

void commandHelp() {
    printf("**Something seemingly helpful**\n");
}

typedef void(*functionPointerType)(void);

typedef struct commandStruct {
    char const* name; // name of a command
    functionPointerType execute; // function to call
    char const* help; // just a helpy string
} commandStruct;

// list of commands
// each command is a commandStruct type
const commandStruct commands[] = {
    { "ver", commandVersion, "Displays firmware version" },
    { "flashTest", commandFlashTest, "Runs flash test" },
    { "blinkLED", commandBlinkLED, "Blinks led with a desired rate, required frequency parameter (Hz)" },
    { "help", commandHelp, "Shows help" },
    { "",0,"" } //end of table indicator
};

void runCommand(const char* command) {
    for(int i = 0; commands[i].execute; ++i) {
            if (commands[i].name == command) {
                commands[i].execute();
                return;
        }
    }
    
    printf("This command does not exist\n");
    return;
}

int main(void) {

    runCommand("ver");
    runCommand("fashTest");
    runCommand("blinkLED");
    runCommand("help");
    runCommand("tetestste");

    return 0;
}

