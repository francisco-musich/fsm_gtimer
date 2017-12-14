/* EL rpgrama tiene 2 threads, uno con un timer que cada medio segundo crea un evento en la cola de eventos
 * el otro thread chequea constantemente en la cola de eventos si recibio alguno. Luego actua acorde
 */



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<stdint.h>
#include<time.h>
#include<unistd.h>

/** * * * * *  * * * * *
 * DEFINICIONES IMPORTANTES*
 * * * *  * * * *   * * * */

#define FIN_TABLA 0
#define bajar 1         //Identificadores de eventos
#define girar 2
#define mover 3
#define baj_pr 'b'
#define mov_pr 'm'
#define gir_pr 'r'

/****************************
 * DECALARACION VAIRABLES GLOBALES*
 * ************************* * * */
char arreglo[50];       //Prueba para ver si responde
int cola_ev[50];        //Cola de eventos
int indice_arr;         // Indice para arreglo de prueba

int TimerTick;
int  counter;

/*************************************
 * PROTOTIPOS FUNCIONES DE TRANSICION *
 * ***********************************/
void fun_baj(void);     //Dummies
void fun_gir(void);
void fun_mov(void);

/*****************************************************
*       DEFINICION DE TIPO ARISTA                   *
 * **************************************************/
typedef struct arista arista_t;
struct arista{
    int evento;
    arista_t *proxestado;
    void (*fun_trans)(void);

} ;

/******************************************************
 * PROTOTIPOS FUNCION EVENT MANAGER                    *
 * *****************************************************/
arista_t* fsm(arista_t* p2estado_actual, int evento1);      //Manager de FSM

/******************************************************
 * PROTOTIPOS FUNCIONES "MAIN" PARA THREADS           *
 * *****************************************************/

void* timer (void);

void* juego_matrices(void);



/************************************
 * DECLARACION DE ESTADOS           *
 * **********************************/
extern arista_t estado_juego[];
extern arista_t estado_pausa[];
extern arista_t estado_menu[];

/*************************
 * INICIALIZACION ESTADOS*
 * ***********************/
arista_t estado_juego[] =
{
    {bajar,estado_juego,fun_baj},
    {girar,estado_juego,fun_gir},
    {mover,estado_juego,fun_mov},
    {FIN_TABLA,estado_juego,}
};
/*arista_t estado_2[]=
{
    {'1',estado_3,pr_suc},
    {FIN_TABLA,estado_ini,pr_pif}
};
arista_t estado_3[]=
{
    {'5',estado_ini,pr_match},
    {FIN_TABLA,estado_ini,pr_pif}
};*/

/****************************
 * HANDLER COLA DE EVENTOS  *
 * **************************/
int get_ev(void);

/***************************
 * MAIN                     *
 * ************************/

int main()
{
       
    pthread_t thread1, thread2;
    if(pthread_create(&thread1,NULL,timer,NULL)){           //Creo ambos threads
        printf("error thread1");
        return 0;
    }
    
    if(pthread_create(&thread2,NULL,juego_matrices,NULL))
    {
        printf("error thread2");
        return 0;
    }
    
    pthread_join(thread1,NULL);     //inicializo y joineo los threads
    pthread_join(thread2,NULL);
    
    return (EXIT_SUCCESS);
}
/* * * * * * * * * *  * * *  
 * TIMER                *
 * * * * * * * * * *  * */

void *timer()           //CADA MEDIO SEGUNDO GUARDA EN LA COLA DE EVENTOS UN 1
{
    while(1)
        {
               
            usleep(100); // 100ms * 

            if (TimerTick)
		TimerTick--;
            else
            {  
                cola_ev[counter++] = 1;        // 
                TimerTick=500;            
            }
        }
}
/* * * * * * * *  * *
 * FUNCIOn juego_matrices     *
 */
void* juego_matrices(void)
{
arista_t *estado_actual = estado_juego;
    
    int evento;
    while(1)
    {
        while(evento = get_ev())
        {
            estado_actual = fsm(estado_actual,evento);
        }
    }
    return 0;
}


/***********************
 * FSM MANAGER        *
 * *********************/
arista_t* fsm(arista_t* p2estado_actual, int evento1){
    while((p2estado_actual->evento != evento1) && (p2estado_actual->evento != FIN_TABLA)){
        p2estado_actual++;
    }
    (*p2estado_actual->fun_trans)();
    return (p2estado_actual->proxestado);
}

/************************
 * FUNCIONES DUMMY      *
 * * * * * * * * *  * * */
void fun_baj(void){
    arreglo[indice_arr++] = baj_pr;
}
void fun_gir(void){
    arreglo[indice_arr++] = gir_pr;
}
void fun_mov(void) {
    arreglo[indice_arr++] = mov_pr;
}

/** * * ** * * * *  * * * * 
 * FUNCION GET_EV      *
 *  * * * *  * * * * * * * */

int get_ev()
{
  static int indice;
  int respuesta;
  if(cola_ev[indice])
  {
    respuesta = cola_ev[indice++];
  }
  else
  {
      respuesta = 0;
  }
  
  
  return respuesta;
}






