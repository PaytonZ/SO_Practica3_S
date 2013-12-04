#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "sem.h"
#include "varcond.h"
#include "mailbox.h"
#include "cbuffer.h"


#define UMAX(a, b)      ((a) > (b) ? (a) : (b))





struct sys_mbox {
  cbuffer_t* cbuffer;
  
  struct  mi_cond_var *hayElem; // no empty
  struct  mi_cond_var *hayHueco; //no full
  pthread_mutex_t *mutex;
    
};






/*-----------------------------------------------------------------------------------*/

struct sys_mbox* mbox_new(unsigned int max_size) {

  struct sys_mbox *mbox;

  mbox = (struct sys_mbox *)malloc(sizeof(struct sys_mbox));
  if (mbox == NULL) {
    return NULL;
  }
    /* Create the data structure */
	if ((mbox->cbuffer = create_cbuffer_t(max_size)) == NULL ) {
		free(mbox);
		return NULL ;
	}
	
  mbox->hayElem = var_cond_new();
  mbox->hayHueco = var_cond_new();
    
  mbox->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mbox->mutex , NULL);

  
  return mbox;
}

/*-----------------------------------------------------------------------------------*/
void
mbox_free(struct sys_mbox *mbox)
{
  if ((mbox != NULL) ) {
    
    var_cond_free(mbox->hayElem);
    var_cond_free(mbox->hayHueco);
	  
    pthread_mutex_destroy(mbox->mutex);
	free(mbox->mutex);
	 
      destroy_cbuffer_t(mbox->cbuffer);

	  
	  free(mbox);
  }
}
/*-----------------------------------------------------------------------------------*/
// Trata de introducir un mensaje en el mailbox
// Si el mailbox esta lleno, el hilo se bloqueara hasta que quede un hueco
// Si hay al menos un hueco, se insertara el mensaje en el mailbox y se saldra

void
mbox_post( struct sys_mbox *mbox, void *msg)
{

    pthread_mutex_lock(mbox->mutex);


        while(is_full_cbuffer_t(mbox->cbuffer)) {



                //pthread_cond_wait(&(mbox->not_full),mbox->mutex);
		var_cond_wait(mbox->hayHueco,mbox->mutex);



        }

        //int unico_elemento = size_cbuffer_t(mbox->cbuffer);
        insert_cbuffer_t(mbox->cbuffer, msg);

        //if (!unico_elemento) {
                
               var_cond_signal(mbox->hayElem);
        //}

        pthread_mutex_unlock(mbox->mutex);
}





/*-----------------------------------------------------------------------------------*/

// Trata de extraer un mensaje del mailbox
// Si el mailbox esta vacio, el hilo debe bloquearse hasta que llegue un mensaje
// Si hay al menos un mensaje, lo extraera del mailbox y lo devolvera
void*
mbox_fetch(struct sys_mbox *mbox)
{
printf("ATENCION funcion mbox_fetch NO implementada\n");
return NULL;	
}




