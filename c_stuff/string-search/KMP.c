#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void read_from_file(const char* filename, char** word);
uint16_t* compute_prefix_function(const char* pattern);
uint8_t kmp_matcher(const char* word, const char* pattern); 

int main(int argc, char** argv) {

    if ( argc != 3 ) { 
        printf("Insufficient number of arguments!\n");
        exit(1);
    }

    printf("pattern: %s file: %s\n", argv[1], argv[2]);

    const char* pattern = argv[1];
    const char* filename = argv[2];
    char* word = NULL;

    read_from_file(filename, &word);


    if ( !(kmp_matcher(word,pattern))) printf("No matches\n");

    free((char*)word);

    return 0;
}

/* KMP-MATCHER(T, P)
*   n = T.length
*   m = P.length
*   pi = COMPUTE-PREFIX-FUNCTION(P)
*   q = 0
*   for i = 1 to n
*      while q > 0 i P[q+1] != T[i]
*         q = pi[q]
*      if P[q+1] == T[i]
*         q = q + 1
*      if q == m
*         wypisz "Wzorzec wystepuje z przesunieciem" i - m
*         q = pi[q]
*/

uint8_t kmp_matcher(const char* word, const char* pattern) {
    uint8_t is_match = 0;

    uint16_t n = strlen(word);
    uint16_t m = strlen(pattern);
    uint16_t* pi = compute_prefix_function(pattern);
    int16_t q = 0;

    for ( uint16_t i = 0; i < n; ++i ) {
        while ( q > 0 && pattern[q] != word[i] ) q = pi[q - 1];

        if ( pattern[q] == word[i] ) q += 1;

        if ( q == m ) {
            printf("Match with a move of %" PRId16 "\n", i - m + 1);
            is_match = 1;
            q = pi[q - 1];
        }
    }

    free(pi);
    return is_match;
}


/* COMPUTE-PREFIX-FUNCTION(P)
*   m = P.length
*   niech pi[1..m] będzie nową tablicą
*   pi[1] = 0
*   k = 0
*   for q = 2 to m
*      while k > 0 i P[k+1] != P[q]
*         k = pi[k]
*      if P[k+1] == P[q]
*         k = k + 1
*      pi[q] = k
*   return pi
*/

uint16_t* compute_prefix_function(const char* pattern) {
    uint16_t m = strlen(pattern);
    uint16_t* pi = (uint16_t*)malloc(m * sizeof(uint16_t));
    if ( !pi ) {
        printf("Couldn't allocate memory! :(");
        exit(1);
    } 

    pi[0] = 0;
    int16_t k = 0;

    for ( uint16_t q = 1; q < m; ++q ) {
        while ( k > 0 && pattern[k] != pattern[q] ) k = pi[k - 1];

        if ( pattern[k] == pattern[q] ) k += 1;

        pi[q] = k;
    }

    return pi;
}


void read_from_file(const char* filename, char** word) {
    FILE *fp = fopen(filename, "r");
    if ( fp != NULL ) {
        fseek(fp, 0, SEEK_END);
        uint16_t filesize = ftell(fp);
        rewind(fp);

        char* filecontent = (char*)malloc((filesize + 1) * sizeof(char));
    
        if ( filecontent == NULL ) {
            printf("Coudn't allocate memory :(\n");
            fclose(fp);
            exit(1);
        }

        fread(filecontent, filesize, 1, fp);
        *(filecontent + filesize)  = '\0';
        fclose(fp);
        printf("Content of a file: %s", filecontent);
        *word = filecontent;
    } else {
        printf("There was an error while reading the file :(\n");
        exit(1);
    }
}



