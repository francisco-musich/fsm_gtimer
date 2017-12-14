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
#define TAMCOLAEVENTO 100
/****************************
 * DECALARACION VAIRABLES GLOBALES*
 * ************************* * * */
char arreglo[50];       //Prueba para ver si responde
char cola_ev[TAMCOLAEVENTO];        //Cola de eventos
int indice_arr =0;         // Indice para arreglo de prueba

char*pevento = cola_ev;

int ticker;
int  counter;
int running = 1;

/*************************************
 * PROTOTIPOS FUNCIONES DE TRANSICION *
 * ***********************************/
void fun_baj(void);     //Dummies
void fun_gir(void);
void fun_mov(void);
void fun_exit(void);
void limp_cola_ev(void);

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
    {girar,estado_juego,limp_cola_ev},
    {mover,estado_juego,fun_mov},
    {FIN_TABLA,estado_juego,}
};
arista_t estado_pausa[]=
{
    {bajar,estado_juego,fun_gir},
    {FIN_TABLA,estado_pausa,}
};
/*arista_t estado_3[]=
{
    {'5',estado_ini,pr_match},
    {FIN_TABLA,estado_ini,pr_pif}
};*/

/****************************
 * HANDLER COLA DE EVENTOS  *
 * **************************/
int get_ev(char modo);

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
    int dificultad = 5;
    while(1)
        {
               
            usleep(100); // NUMEROS MAGICOS PERO FUNCIONAN

            if (ticker)
		ticker--;
            else
            {  
                *(pevento++) = bajar;//uso puntero que agrega eventos para agregar evento de bajar
                ticker= dificultad*100;    //dificultad es inversamente proporcional con el tiempo entre tick   
                *(pevento++) = girar;
            }
        }
}
/* * * * * * * *  * *
 * FUNCIOn juego_matrices     *
 */
void* juego_matrices(void)
{
arista_t *estado_actual = estado_juego; //creo puntero a estado, y lo inicializo en juego(deberia ir en estado_menu)
    
    int evento;
    while(running)  //chequeo que el programa deba seguir corriendo
    {
        while((evento = get_ev(1))&&(running)) //chequeo que haya algo en la cola, y que no se haya elejido salir del juego
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

int get_ev(char modo)       //recibo modo asi evito mas globales(leo evento modo=1, modo = 0 limpio indice)
{
  static int indice = 0;        //indice para lectura de eventos
  int respuesta= 0;
  if(modo)
  {
    if(cola_ev[indice])     //si hay algo en la cola de eventos devuelvo la cola de eventos e incremento el indice
    {
      respuesta = cola_ev[indice++];    //si no hay nada devuelve el evento 0, que coincide con FINTABLA, entonces la fsm no hace nada y espera el proximo evento
    }
  }
  else
  {
      indice =0;
  }
  
  
  return respuesta;
}

/*******************************************************************************
 * FUNCION QUE TERMINA EL PROGRAMA
 * *****************************************************/
void fun_exit(void)
{
    running = 0;        //apago el flag que mantiene los loops
}

void limp_cola_ev(void)
{
   int i;
   for (i=0;i<TAMCOLAEVENTO;i++){ //limpio la cola de eventos
       cola_ev[i] = 0;
   }
   get_ev(0);       //limpio variable estatica dentro de get_ev
   pevento = cola_ev;       // reseteo puntero que guarda eventos al inicio de la cola
}




