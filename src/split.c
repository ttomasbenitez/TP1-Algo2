#include <stdio.h>
#include <stdlib.h>
#include "split.h"
#include <string.h>

/**
 * Devuelve la cantidad de veces que se repite el separador en el string recibido.
 */
size_t contar_ocurrencias(const char* string, char separador){
    size_t contador = 0;
    int i = 0;

    while(string[i] != '\0'){
        if(string[i] == separador){
            contador++;
        }
        i++;
    }
    return contador;
}

/**
 * Busca en el string recibido el separador y devuelve la cantidad de caracteres que
 * hay entre un separador y otro, o un separador y el final del string.
 */ 
size_t buscar_proximo_separador(const char* string, char separador){
    size_t i = 0;
    while( (string[i] != '\0') && (string[i] != separador) ){
        i++;
    }
    return i;
}

/**
 * Crea un string dinámico a partir de lo contenido entre separadores en el string
 * original y lo devuelve.
 */
char* duplicar_string(const char* string, size_t cantidad){
    char* nuevo_string = malloc((cantidad+1)*sizeof(char));
    for(size_t i = 0; i < cantidad; i++){
        nuevo_string[i] = string[i];
    }
    nuevo_string[cantidad] = 0;
    return nuevo_string;
}

/**
 *  Libera el vector y toda la memoria utilizada por el mismo.
 */
void liberar_todo_el_vector_y_substring(char** vector){
    while(*vector){
        free(*vector);
        vector++;
    }
}

/**
 * Recibe un string y un separador y devuelve un vector dinámico que contenga
 * los substrings delimitados por el separador. Al final del vector debe haber
 * un string NULL para saber que es el final del mismo.
 *
 * Si el string es nulo o no hay memoria disponible, se debe devolver NULL.
 */
char** split(const char* string, char separador){
    if(!string){
        return NULL;
    }
    size_t ocurrencias = contar_ocurrencias(string, separador);
    size_t cantidad_substrings = ocurrencias + 1;

    char** vector = calloc((cantidad_substrings+1), sizeof(void*));
    if(!vector){
        return NULL;
    }

    for(size_t i = 0; i < cantidad_substrings; i++){
        size_t tamanio_substring = buscar_proximo_separador(string, separador);
        char* substring = duplicar_string(string, tamanio_substring);
        if(!substring){
            liberar_todo_el_vector_y_substring(vector);
            return NULL;
        }
        vector[i] = substring;
        string += tamanio_substring + 1;
    }  
    return vector;
}