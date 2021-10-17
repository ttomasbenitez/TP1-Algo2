#include "hospital.h"
#include <stdio.h>
#include <string.h>
#include "split.h"


struct _hospital_pkm_t{
    size_t cantidad_pokemon;
    pokemon_t* vector_pokemones;
    size_t cantidad_entrenadores;
};

struct _pkm_t{
    char* nombre;
    size_t nivel;
};


/**
 * Reserva memoria para un hospital y la inicializa.
 *
 * Devuelve NULL en caso de no poder.
 */
hospital_t* hospital_crear(){
    return (calloc(1, sizeof(hospital_t)));
}

/**
 * Recibe linea_leida y devuelve la cantidad de pokemones que se leyeron en esa linea.
 *(sabiendo que hay tantos pokemones como ((elementos en linea_leida - 2)/2).
 */
size_t cantidad_pokemones_linea(hospital_t* hospital, char** linea_leida){
    int i = 2;
    size_t contador = 0;
    while(linea_leida[i] != 0){
        contador++;
        i++;
    }

    return (contador/2);
}

/**
 * Agrega a hospital la cantidad de pokemones que posee el entrenador leído, los suma 
 * a cantidad_pokemon y suma ese entrenador a cantidad_entrenadores.
 */ 
void sumar_cantidad_pokemon_y_entrenador(hospital_t* hospital, size_t pokemones_a_agregar){
    hospital->cantidad_pokemon += pokemones_a_agregar;
    hospital->cantidad_entrenadores += 1;
}

/**
 * Crea un bloque de memoria para agregar a los pokemones que se leyeron, si 
 * vector_pokemones está vacío. Si vector_pokemones ya tiene elementos, agranda el 
 * bloque de memoria para cargar los nuevos pokemones leídos.
 */ 
void reservar_memoria_para_pokemon(hospital_t* hospital, size_t pokemones_a_agregar){
    if( ((hospital->cantidad_pokemon) - pokemones_a_agregar) == 0 ){
        pokemon_t* auxiliar = calloc(((hospital->cantidad_pokemon)+1), sizeof(pokemon_t) );
        if(!auxiliar) return;
        
        hospital->vector_pokemones = auxiliar;
    }
    else{
        pokemon_t* auxiliar = realloc(hospital->vector_pokemones, sizeof(pokemon_t)*((hospital->cantidad_pokemon)+1));
        if(!auxiliar) return;
        
        hospital->vector_pokemones = auxiliar;
    }  
}

/**
 * Crea un bloque de memoria en el vector pokemones, en la posición del pokemon en cuestión,
 * para agregar el nombre del mismo. 
 */ 
void reservar_memoria_nombre_pokemon(hospital_t* hospital, size_t longitud_string, size_t posicion){
    char* auxiliar = calloc((longitud_string+1), sizeof(char));
    if(!auxiliar) return;
        
    hospital->vector_pokemones[posicion].nombre = auxiliar;
}

/**
 * Agrega al hospital en la primera posicion libre del vector pokemones al pokemon leido,
 * (nombre y nivel);
 */ 
void agregar_a_hospital(hospital_t* hospital, char** linea_leida, size_t pokemones_a_agregar){
    size_t pos_a_agregar = ((hospital->cantidad_pokemon) - pokemones_a_agregar);
    int i = 2;
    while(linea_leida[i] != 0){
        if(i % 2 == 0){
            size_t longitud_actual = strlen(linea_leida[i]);
            reservar_memoria_nombre_pokemon(hospital, longitud_actual, pos_a_agregar);
    
            strcpy(hospital->vector_pokemones[pos_a_agregar].nombre, linea_leida[i]);
            hospital->vector_pokemones[pos_a_agregar].nivel = (size_t)atoi(linea_leida[i+1]);;
            
            pos_a_agregar++;
        }
        i++;
    }
}


/**
 * Recibe linea_leida, libera todos sus elementos y la memoria que ocupan, 
 * luego libera linea_leida y la memoria que ocupa.
 */
void liberar_linea_leida(char** linea_leida){
    int i = 0;
    while(linea_leida[i] != 0){
        free(linea_leida[i]);
        i++;
    }
    free(linea_leida);
}

/**
 * Recibe un hospital con todos sus campos cargados y lo ordena alfabéticamente por 
 * nombre.
 */
void ordenar_alfabeticamente_pokemones(hospital_t* hospital){
    if(!hospital) return;

    pokemon_t aux;
    for(int i = 0; i < ((hospital->cantidad_pokemon)-1); i++){
        for(int j = i + 1; j < (hospital->cantidad_pokemon); j++){
            if(strcmp(hospital->vector_pokemones[i].nombre, hospital->vector_pokemones[j].nombre) > 0){
                aux = hospital->vector_pokemones[i];
                hospital->vector_pokemones[i] = hospital->vector_pokemones[j];
                hospital->vector_pokemones[j] = aux;
            }
        }
    }
}

/**
 * Lee un archivo con entrenadores que hacen tratar a sus pokemon en el hospital y los agrega al mismo.
 *
 * En caso de error devuelve false. Caso contrario, true.
 */
bool hospital_leer_archivo(hospital_t* hospital, const char* nombre_archivo){ 
    FILE* archivo_entrenadores = fopen(nombre_archivo, "r");
    if(!archivo_entrenadores){
        perror("No se pudo abrir archivo_entrenadores\n");
        return false;
    }
    
    int bytes_leidos = 0;
    int tamanio = 1024;
    char* buffer = malloc((size_t)tamanio*sizeof(int));
    
    if(!buffer){
        return NULL;
    }

    char** linea_leida;
    
    while( fgets(buffer+bytes_leidos, tamanio-bytes_leidos, archivo_entrenadores) ){
        int leido = (int)strlen(buffer+bytes_leidos);
        if( (leido > 0) && (*(buffer+bytes_leidos+leido-1) == '\n') ){
            *(buffer+bytes_leidos+leido-1) = 0;
            
            linea_leida = split(buffer, ';');
            
            size_t pokemones_a_agregar = cantidad_pokemones_linea(hospital, linea_leida);
            
            sumar_cantidad_pokemon_y_entrenador(hospital, pokemones_a_agregar);
            reservar_memoria_para_pokemon(hospital, pokemones_a_agregar); 
            agregar_a_hospital(hospital, linea_leida, pokemones_a_agregar);
           
            leido = 0;                   
            bytes_leidos = 0;
             
            liberar_linea_leida(linea_leida);
        }
        else{
            char* auxiliar = realloc(buffer, (size_t)tamanio*2);  
            if(!auxiliar){
                free(buffer);
                return NULL;
            }
            buffer = auxiliar;
            tamanio *= 2;
        }
        bytes_leidos += leido;
    }
    
    free(buffer);

    fclose(archivo_entrenadores);
   
    return true;
}


/**
 * Devuelve la cantidad de entrenadores que actualmente hacen atender a sus
 * pokemon en el hospital.
 */
size_t hospital_cantidad_pokemon(hospital_t* hospital){
    if(!hospital) return 0;

    return hospital->cantidad_pokemon;
}

/**
 * Devuelve la cantidad de entrenadores que actualmente hacen atender a sus
 * pokemon en el hospital.
 */
size_t hospital_cantidad_entrenadores(hospital_t* hospital){
    if(!hospital) return 0;

    return hospital->cantidad_entrenadores;
}

/**
 * Aplica una función a cada uno de los pokemon almacenados en el hospital.
 *
 * La función a aplicar recibe el pokemon y devuelve true o false. Si la función
 * devuelve true, se debe seguir aplicando la función a los próximos pokemon si
 * quedan. Si la función devuelve false, no se debe continuar.
 *
 * Devuelve la cantidad de pokemon a los que se les aplicó la función (hayan devuelto true o false).
 */
size_t hospital_a_cada_pokemon(hospital_t* hospital, bool (*funcion)(pokemon_t* p)){
    if((!hospital) || (!funcion) || (!hospital->vector_pokemones)) return 0;
    size_t contador = 0;
    
    ordenar_alfabeticamente_pokemones(hospital);
    
    for(int i = 0; i < (hospital->cantidad_pokemon); i++){
        if(funcion(&(hospital->vector_pokemones[i]))){
            contador++;
        }
        else{
            contador++;
            return contador;
        }
    }
    return contador;
}

/**
 *  Libera el hospital y toda la memoria utilizada por el mismo.
 */
void hospital_destruir(hospital_t* hospital){
    if(!hospital){
        return;
    }
    int i = 0;
    while(i < (hospital->cantidad_pokemon)){
        free(hospital->vector_pokemones[i].nombre);
        i++;
    }
    free(hospital->vector_pokemones);
    free(hospital);
}

/**
 * Devuelve el nivel de un pokemon o 0 si el pokemon es NULL.
 */
size_t pokemon_nivel(pokemon_t* pokemon){
    if(!pokemon) return 0;

    return (pokemon->nivel);
}

/**
 * Devuelve el nombre de un pokemon o NULL si el pokemon es NULL.
 */
const char* pokemon_nombre(pokemon_t* pokemon){
    if(!pokemon) return NULL;

    return (pokemon->nombre);
}
