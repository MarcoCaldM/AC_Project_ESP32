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
bool Modo_State = ON;
bool Cool_Heat_State = HEAT;
double SetPoint = 25.0;

char mensaje[50];

/*Manejador de interrupción para el botón ON/OFF*/
void IRAM_ATTR Enc_Apg_isr_handler(void* arg) {
    Enc_Apg_State = true;                          /*Encender el sistema al presionar*/
    uart_write_bytes(UART_NUM_0, "\nSistema: ON.", 12);     /*Informa por la UART*/
    //while(!gpio_get_level(Enc_Apg_Btn));        /*Evita entrar varias veces mientras se pulse*/
}

/*Manejador de interrupción para los botones Modo y Cool*/
void IRAM_ATTR Modo_Cool_isr_handler(void* arg) {
    if(gpio_get_level(Modo_Btn) == false){      /*Si se pulsa el botón de Modo se cambia e imprime*/
        Modo_State = !Modo_State;
        sprintf(mensaje, "Modo: %s\n", Modo_State ? "AUTO" : "ON");
        uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    }
    if(gpio_get_level(Cool_Btn) == false){      /*Si se pulsa el botón de Modo se cambia e imprime*/
        Cool_Heat_State = !Cool_Heat_State;
        sprintf(mensaje, "Modo: %s\n", Cool_Heat_State ? "COOL" : "HEAT");
        uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    }
}

/*Inicializar GPIO*/
void Initialize_GPIO() {
    gpio_config_t io_conf;

    /*Configura Enc_Apg_Btn*/
    io_conf.pin_bit_mask = (0x01 << Enc_Apg_Btn);
    io_conf.mode = GPIO_MODE_INPUT;                 /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;          /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        /*Resistencia pullup activa*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   /*Resistencia pulldown desactiva*/
    gpio_config(&io_conf);

    /*Configura Modo_Btn*/
    io_conf.pin_bit_mask = (0x01 << Modo_Btn);
    io_conf.mode = GPIO_MODE_INPUT;                 /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;          /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        /*Resistencia pullup activa*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   /*Resistencia pulldown desactiva*/
    gpio_config(&io_conf);

    /*Configura Cool_Btn*/
    io_conf.pin_bit_mask = (0x01 << Cool_Btn);
    io_conf.mode = GPIO_MODE_INPUT;                 /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;          /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        /*Resistencia pullup activa*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   /*Resistencia pulldown desactiva*/
    gpio_config(&io_conf);

    /*Configura LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);

    /*Configura la interrupción de los botones*/
    gpio_install_isr_service(0);
    gpio_isr_handler_add(Enc_Apg_Btn, Enc_Apg_isr_handler, (void*) Enc_Apg_Btn);
    gpio_isr_handler_add(Modo_Btn, Modo_Cool_isr_handler, NULL);
    gpio_isr_handler_add(Cool_Btn, Modo_Cool_isr_handler, NULL);
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


void LED_Heartbeat(){
    while (true)
    {
        if (Enc_Apg_State == ENCENDIDO){
            LED_Status = !LED_Status;
            gpio_set_level(LED_PIN, LED_Status);
        }
        vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
    }
}