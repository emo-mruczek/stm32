/* NAIVE-STRING-MATCHER(T, P)
*   n = T.length
*   m = P.length
*   for s = 0 to n - m
*       if P[1..m] == T[s+1..s+m]
*           wypisz "Wzorzec wystepuje z przesunieciem" s
*/

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint16_t is_match(const char* word_a, const char* word_b);
uint8_t read_from_file(const char* filename, char** word);

int main(int argc, char** argv) {

    if ( argc != 3 ) { 
        printf("Insufficient number of arguments!\n");
        return 1;
    }

    printf("pattern: %s file: %s\n", *(argv + 1), *(argv + 2));

    const char* filename = *(argv + 2);
    const char* word_b = *(argv + 1);
    char* word_a = NULL;

    if ( read_from_file(filename, &word_a) ) return 1;

   
    uint16_t n = strlen(word_a);
    uint16_t m = strlen(word_b);

    for ( uint16_t s = 0; s <= n - m; ++s ) {
        if ( is_match(word_a + s, word_b) ) {
            printf("Match with a move of %" PRIu16 "\n", s);
            return 0;
        }
    }

    printf("No matches\n");
    return 0;
}

uint16_t is_match(const char* word_a, const char* word_b) {
    
    while ( *word_b ) {
        if ( *word_a != *word_b ) return 0;

        ++word_a;
        ++word_b;
    }

    return 1;
}

uint8_t read_from_file(const char* filename, char** word) {
    FILE *fp = fopen(filename, "r");
    if ( fp != NULL ) {
        fseek(fp, 0, SEEK_END);
        uint16_t filesize = ftell(fp);
        rewind(fp);

        char* filecontent = (char*)malloc((filesize + 1) * sizeof(char));
    
        if ( filecontent == NULL ) {
            printf("Coudn't allocate memory :(\n");
            fclose(fp);
            return 1;
        }

        fread(filecontent, filesize, 1, fp);
        *(filecontent + filesize)  = '\0';
        fclose(fp);
        printf("Content of a file: %s", filecontent);
        *word = filecontent;
        return 0;
    } else {
        printf("There was an error while reading the file :(\n");
        return 1;
    }
}
