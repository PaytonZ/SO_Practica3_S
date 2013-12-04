
#include "varcond.h"
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

struct mi_cond_var* var_cond_new( ) {
    struct mi_cond_var* vc =NULL;
    
    vc =(struct mi_cond_var*) malloc(sizeof(struct mi_cond_var));
    if (vc != NULL) {
        if ( sem_init(&(vc->sem),0,0) != 0 ) {
            printf("Error creando el semáforo de la variable condicional\n");
        }
        pthread_mutex_init(&(vc->mutex), NULL);
        vc->c=0;
        
    }
    return vc;
    
}


void var_cond_free(struct  mi_cond_var *vc) {
    
    sem_destroy(&(vc->sem));
	pthread_mutex_destroy(&(vc->mutex));
    free(vc);
}


// Solo hacemos sem_post si hay alguien esperando (porque no debemos modificar el estaod del
// semaforo en caso contrario.
// Asi garantizamos que el valor del semaforo NUCA es positivo
void var_cond_signal(struct  mi_cond_var *vc) {
    
	pthread_mutex_lock(&(vc->mutex));
    if ( vc->c > 0)
        sem_post(&(vc->sem));
	pthread_mutex_unlock(&(vc->mutex));
}


// Funcion que "emula" la llamada wait sobre un variable condicional
// Se asume que el cerrojo "l" recibiido como argumento esta cerrado
// y pertenece al hilo que esta invocando var_cond_wait
// Asimismo, por construccion (ver codigo de var_cond_signal), se asume que 
// el semaforo (vc->sem) nunca tendra un valor mayor que 0.
int var_cond_wait(struct  mi_cond_var *vc, pthread_mutex_t* l) {
   
// 1. Incrementar el contador de hilos bloqueados en vc.
vc->c++;

// (hay un campo para ese contador en el tipo de vc)
// 2. Liberar el cerrojo m
pthread_mutex_unlock(l);

// 3. Bloquearse (usando el semáforo de vc)
sem_wait(&(vc->sem));

// 4. Volver a obtener el cerrojo m
pthread_mutex_lock(l);

// 5. Decrementar el contador de hilos
vc->c--;

return 0;	

   }
