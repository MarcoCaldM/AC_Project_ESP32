#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define Enc_Apg_Btn 4
#define LED_PIN 14

volatile bool Enc_Apg_State = false;
volatile bool LED_Status = false;

void Initialize_GPIO();
void Initialize_UART();

/*Manejador de interrupción para el botón ON/OFF*/
void IRAM_ATTR Enc_Apg_isr_handler(void* arg) {
    Enc_Apg_State = 1;                          /*Encender el sistema al presionar*/
}

/*Inicializar GPIO*/
void Initialize_GPIO() {
    gpio_config_t io_conf;

    /*Configura Enc_Apg_Btn*/
    io_conf.pin_bit_mask = (1ULL << Enc_Apg_Btn);
    io_conf.mode = GPIO_MODE_INPUT;             /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_POSEDGE;      /*Interrupción en flanco de subida y bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;    /*Resistencia pullup*/
    gpio_config(&io_conf);

    /*Configura LED_PIN*/
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);

    /*Configura la interrupción del botón*/
    gpio_install_isr_service(0);
    gpio_isr_handler_add(Enc_Apg_Btn, Enc_Apg_isr_handler, (void*) Enc_Apg_Btn);
}

void Initialize_UART() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
}

void app_main() {
    Initialize_GPIO();      /*Inicializa los pines GPIO*/
    Initialize_UART();      /*Inicializa la comuniación UART*/

    while(1) {
        if (Enc_Apg_State == 1) {
            printf("Sistema: ON\n");
            LED_Status = !LED_Status;
            gpio_set_level(LED_PIN, LED_Status);
            vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
            char mensaje[50];
            sprintf(mensaje, "Estado del sistema: %s\n", Enc_Apg_State ? "Encendido" : "Apagado");
            uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
        }
    }
}
