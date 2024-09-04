#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void read_from_file(const char* filename, char** word);
int8_t finite_automaton_matcher(const char* word, uint16_t** tra, uint16_t pattern_len, const char* sigma);
uint16_t** compute_transition_function(const char* pattern, const char* sigma); 
uint8_t is_sufix(const char* pattern, uint16_t k, uint16_t q, uint16_t a, const char* sigma);
char* generate_sigma(const char* word, const char* pattern);

// for gdb
void print_matrix(uint16_t** matrix);

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

    const char* sigma = generate_sigma(word, pattern);

    uint16_t** tr = compute_transition_function(pattern, sigma);

    if ( !(finite_automaton_matcher(word, tr, strlen(pattern), sigma))) printf("No matches\n");

    for (uint16_t i = 0; i <= strlen(pattern); ++i) {
        free(tr[i]);
    }
    free(tr);
    free((char*)sigma);
    free((char*)word);
   
    return 0;
}


uint8_t is_sufix(const char* pattern, uint16_t k, uint16_t q, uint16_t a, const char* sigma) {
    char* new_pattern = (char*)malloc((q + 2) * sizeof(char));
    if ( !new_pattern ) {
        printf("Couldn't allocate memory! :(");
        exit(1);
    }

    strncpy(new_pattern, pattern, q);
    new_pattern[q] = sigma[a];
    new_pattern[q+1] = '\0';

    for ( int16_t i = 0; i < k; ++i ) {
        if ( pattern[k - i - 1] !=  new_pattern[q - i] ) {
        free(new_pattern);
        return 0;
        }
    }

    free(new_pattern);
    return 1;
}

/* COMPUTE-TRANSITION-FUNCTION(P, SIG)
 *    m = P.length
 *    for q = 0 to m
 *       for kazdy symbol a nal SIG
 *          k = min( m + 1, q + 2 )
 *          repeat
 *             k = k - 1
 *          until Pk sufix Pqa
 *          tr(q, a) = k
 *  return tr
 */

uint16_t** compute_transition_function(const char* pattern, const char* sigma) {
    uint16_t m = strlen(pattern);
    uint16_t n = strlen(sigma);
    uint16_t k;

    // tr is a transition function of size (m+1)*n
    uint16_t** tr = (uint16_t**)malloc((m + 1) * sizeof(uint16_t*));
    for ( uint16_t i = 0; i <= m; ++i ) {
        tr[i] = (uint16_t*)malloc(n * sizeof(uint16_t));
    }
    if ( !tr ) {
        printf("Couldn't allocate memory! :(");
        exit(1);  
    }

    // there are m + 1 states
    for ( uint16_t q = 0; q <= m; ++q) {
        for ( uint16_t a = 0; a < n; ++a) { 
            k = ( (m + 1) < (q + 2) ? m + 1 : q + 2 );
            do {
                k -= 1;
            } while ( !(is_sufix(pattern, k, q, a, sigma)) );
            tr[q][a] = k;
        }
    }

    return tr;
}

/* FINITE-AUTOMATON-MATCHER(T, tr, m)
 *  n = T.length
 *  q = 0
 *  for i = 1 to n
 *     q = tr(q, T[i])
 *     if q == m
 *        wypisz "Wzorzec wystepuje z przesunieciem " i - m
 */

int8_t finite_automaton_matcher(const char* word, uint16_t** tr, uint16_t pattern_len, const char* sigma) {
    uint16_t n = strlen(word);
    uint16_t q = 0;
    uint16_t sigma_index;
    uint16_t sigma_len = strlen(sigma);

    for ( uint16_t i = 0; i < n; ++i ) {
         for (uint16_t j = 0; j < sigma_len; ++j) {
            if (sigma[j] == word[i]) {
                sigma_index = j;
                break;
            }
        }   

        q = tr[q][sigma_index]; 
        if ( q == pattern_len ) printf("Match with a move of %" PRId16 "\n", i - pattern_len + 1);
    }

    return -1;
}

// generating alphabet assuming everything is ascii
char* generate_sigma(const char* word, const char* pattern) {
    uint16_t ascii[256] = { 0 };
    uint16_t size = 0;
    const char* word_copy = word;
    const char* pattern_copy = pattern;

    while ( *word_copy ) {
        if ( !ascii[*word_copy] ) {
            ascii[*word_copy] = 1;
            ++size;
        }
        ++word_copy;
    }

    while ( *pattern_copy ) {
        if ( !ascii[*pattern_copy] ) {
            ascii[*pattern_copy] = 1;
            ++size;
        }
        ++pattern_copy;
    }

    char* sigma = (char*)malloc((size + 1) * sizeof(char));
    if ( !sigma ) {
        printf("Couldn't allocate memory! :(");
        exit(1);
    }
    sigma[size] = '\0';


    uint16_t index = 0;
    for ( uint16_t i = 0; i < 256; ++i ) {
        if ( ascii[i] ) {
            sigma[index++] = (char)i;
        }
    }

    return sigma;
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


// for gdb
void print_matrix(uint16_t** matrix) {
  int i, j;
  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 3; ++j)
      printf("% " PRId16, matrix[i][j]);
    printf("\n");
  }
}
