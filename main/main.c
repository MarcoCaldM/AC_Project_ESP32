/*******************************************************************************
* Title                 :   main 
* Filename              :   main.c
* Author                :   Marco Calderón
* Origin Date           :   09/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

#include "AControl.h"

bool Enc_Apg_State = false;

/*FUNCION PRINCIPAL*/
void app_main() {
    Initialize_GPIO();                /*Inicializa los pines GPIO*/
    Initialize_UART();                /*Inicializa la comuniación UART*/
    uart_write_bytes(UART_NUM_0, "\nSistema: OFF.", 13);    /*Informa que el sistema aún no se enciende*/

    xTaskCreate(LED_Heartbeat, "LED_Hearbeat", 1024 * 2, NULL, 2, NULL);    /*Crea una tarea para el parpadeo del LED*/

    while(true) {
        if (Enc_Apg_State == ENCENDIDO) {   /*Si el sistema se enciende...*/
            /*No tiene nada porque el LED lo moví a una tarea*/
        }
        vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
    }
}