/*******************************************************************************
* Title                 :   main 
* Filename              :   main.c
* Author                :   Marco Calderón, Javier Perez Macias, Alejandro Morales Holguín
* Origin Date           :   09/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

#include "AControl.h"
#include "drivers/gpio_drivers.h"

/*Variables globales del sistema*/
bool Enc_Apg_State = false;
SSD1306_t dev;
bool Modo_State = ON;
bool Cool_Heat_State = HEAT;
char mensaje[50];

/*FUNCION PRINCIPAL*/
void app_main() {
    Initialize_GPIO();                /*Inicializa los pines GPIO*/
    Initialize_UART();                /*Inicializa la comuniación UART*/
    Initialize_ADC();                /*Inicializa la conversión ADC*/
    Initialize_OLED();                /*Inicializa la pantalla OLED*/
    uart_write_bytes(UART_NUM_0, "\nSistema: OFF.", 13);    /*Informa que el sistema aún no se enciende*/
    ssd1306_display_text(&dev, 0, "Sistema: OFF", 13, false); /*Y también lo imprime por la OLED*/

     /*Crea una tarea para el parpadeo del LED, la impresión constante del los estados por la OLED y la impresion de la UART*/
    xTaskCreate(OLED_Heartbeat, "OLED_Hearbeat", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(UART_Print, "UART_Print", 1024 * 2, NULL, 2, NULL);

    while(true) {
        if (Enc_Apg_State == ENCENDIDO) {   /*Si el sistema se enciende...*/
            ssd1306_display_text(&dev, 0, "Sistema: ON", 12, false);    /*Informa por la OLED*/
            Access_Control();
            Temperature_Control();

            if(!GPIO_INPUT_READ(Modo_Btn)){      /*Si se pulsa el botón de Modo se cambia e imprime*/
                Modo_State = !Modo_State;
                sprintf(mensaje, "Modo: %s\n", Modo_State ? "ON" : "AUTO");
                uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
                while(!GPIO_INPUT_READ(Modo_Btn));
            }
            else if(!GPIO_INPUT_READ(Cool_Btn)){      /*Si se pulsa el botón de Modo se cambia e imprime*/
                Cool_Heat_State = !Cool_Heat_State;
                sprintf(mensaje, "Modo: %s\n", Cool_Heat_State ? "HEAT" : "COOL");
                uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
                while(!GPIO_INPUT_READ(Cool_Btn));
            }
            if (!GPIO_INPUT_READ(Enc_Apg_Btn)){
                Enc_Apg_State = false;                          /*Encender el sistema al presionar*/
                ssd1306_clear_screen(&dev, false);  /*Se limpia la pantalla para empezar a imprimir*/
                ssd1306_display_text(&dev, 0, "Sistema: OFF", 13, false); /*Y también lo imprime por la OLED*/
                uart_write_bytes(UART_NUM_0, "\nSistema: OFF.", 13);     /*Informa por la UART*/
                while (!GPIO_INPUT_READ(Enc_Apg_Btn));
            }    
        }
        if (!GPIO_INPUT_READ(Enc_Apg_Btn) && Enc_Apg_State != ENCENDIDO){
            Enc_Apg_State = true;                          /*Encender el sistema al presionar*/
            uart_write_bytes(UART_NUM_0, "\nSistema: ON.", 12);     /*Informa por la UART*/
            while(!GPIO_INPUT_READ(Enc_Apg_Btn));
        }
        vTaskDelay(pdMS_TO_TICKS(10));     /*Esperar 10 milisegundos*/
    }
}