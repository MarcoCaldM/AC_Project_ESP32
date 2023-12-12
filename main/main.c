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
    uart_write_bytes(UART_NUM_0, "\nSistema: OFF.", 13);

    while(1) {
        if (Enc_Apg_State == true) {
            Enc_Apg_Sistema();
        }
        vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
    }
}
