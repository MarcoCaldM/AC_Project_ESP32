/*******************************************************************************
* Title                 :   AControl_IO
* Filename              :   AControl_IO.c
* Author                :   Marco Calderón
* Origin Date           :   11/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

#include "AControl.h"

bool LED_Status = false;

/*Manejador de interrupción para el botón ON/OFF*/
void IRAM_ATTR Enc_Apg_isr_handler(void* arg) {
    Enc_Apg_State = true;                          /*Encender el sistema al presionar*/
}

/*Inicializar GPIO*/
void Initialize_GPIO() {
    gpio_config_t io_conf;

    /*Configura Enc_Apg_Btn*/
    io_conf.pin_bit_mask = (0x01 << Enc_Apg_Btn);
    io_conf.mode = GPIO_MODE_INPUT;             /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;      /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;    /*Resistencia pullup*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;    /*Resistencia pullup*/
    gpio_config(&io_conf);

    /*Configura LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);

    /*Configura la interrupción del botón*/
    gpio_install_isr_service(0);
    gpio_isr_handler_add(Enc_Apg_Btn, Enc_Apg_isr_handler, (void*) Enc_Apg_Btn);
}

/*Inicializar UART*/
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


void Enc_Apg_Sistema(){
    uart_write_bytes(UART_NUM_0, "\nSistema: ON.", 12);
    LED_Status = !LED_Status;
    gpio_set_level(LED_PIN, LED_Status);
}