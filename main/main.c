/*******************************************************************************
* Title                 :   main 
* Filename              :   main.c
* Author                :   Marco Calderón, Javier Perez Macias, Alejandro Morales Holguín
* Origin Date           :   09/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

#include "AControl.h"

/*Variables globales del sistema*/
bool Enc_Apg_State = false;
SSD1306_t dev;

/*FUNCION PRINCIPAL*/
void app_main() {
    Initialize_GPIO();                /*Inicializa los pines GPIO*/
    Initialize_UART();                /*Inicializa la comuniación UART*/
    Initialize_ADC();                /*Inicializa la conversión ADC*/
    Initialize_OLED();                /*Inicializa la pantalla OLED*/
    uart_write_bytes(UART_NUM_0, "\nSistema: OFF.", 13);    /*Informa que el sistema aún no se enciende*/
    ssd1306_display_text(&dev, 0, "Sistema: OFF", 13, false); /*Y también lo imprime por la OLED*/

     /*Crea una tarea para el parpadeo del LED y la impresión constante del los estados por la OLED*/
    xTaskCreate(OLED_Heartbeat, "OLED_Hearbeat", 1024 * 2, NULL, 2, NULL);

    while(true) {
        if (Enc_Apg_State == ENCENDIDO) {   /*Si el sistema se enciende...*/
            ssd1306_display_text(&dev, 0, "Sistema: ON", 12, false);    /*Informa por la OLED*/
            Access_Control();
            Temperature_Control();
            UART_Print();
        }
        vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
    }
}