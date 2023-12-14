/*******************************************************************************
* Title                 :   AControl_IO
* Filename              :   AControl_IO.c
* Author                :   Marco Calderón, Javier Perez Macias, Alejandro Morales Holguín
* Origin Date           :   11/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/

#include "AControl.h"

/******************************************************************************
* Variables
*******************************************************************************/

/*Variables de control de estados*/
bool LED_Status = false;
bool RGB_Status = false;
bool Alarma_State = OFF;
bool Modo_State = ON;
bool Cool_Heat_State = HEAT;
bool Fan_State = OFF;
bool Door_State = CLOSED;
bool IN_Pushed = false;
bool OUT_Pushed = false;

/*Variables de control de temperatura*/
double SetPoint = 25.0;
double TEMPAMB;
double TEMCOR;

/*Variables de control de acceso*/
int Cantidad_Actual = 0;
int Cantidad_Max = 5;

/*Variable de mensaje para la correcta impresión del UART y la OLED*/
char mensaje[50];

/******************************************************************************
* Definicion de funciones
*******************************************************************************/

/*! 
* Funcion: Enc_Apg_isr_handler
* Pre-condiciones: 
* Descripcion: Manejador de interrupción para el botón ON/OFF
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void IRAM_ATTR Enc_Apg_isr_handler(void* arg) {
    if(Enc_Apg_State != ENCENDIDO){
        Enc_Apg_State = true;                          /*Encender el sistema al presionar*/
        uart_write_bytes(UART_NUM_0, "\nSistema: ON.", 12);     /*Informa por la UART*/
    }
}

/*! 
* Funcion: IN_OUT_isr_handler
* Pre-condiciones: 
* Descripcion: Manejador de interrupción para los sensores de entrada y salida
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void IRAM_ATTR IN_OUT_isr_handler(void* arg) {
    if(gpio_get_level(S_IN_Btn) == false){      /*Al detectar una entrada*/
        IN_Pushed = true;                       /*Activa la bandera de acceso*/
    }
    if(gpio_get_level(S_OUT_Btn) == false){      /*Al detectar una salida*/
        OUT_Pushed = true;                       /*Activa la bandera de salida*/
    }
}


/*! 
* Funcion: Modo_Cool_isr_handler
* Pre-condiciones: 
* Descripcion: Manejador de interrupción para los botones Modo y Cool
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void IRAM_ATTR Modo_Cool_isr_handler(void* arg) {
    if(Enc_Apg_State == ENCENDIDO){         /*Solo si el sistema está encendido*/
        if(gpio_get_level(Modo_Btn) == false){      /*Si se pulsa el botón de Modo se cambia e imprime*/
        Modo_State = !Modo_State;
        sprintf(mensaje, "Modo: %s\n", Modo_State ? "ON" : "AUTO");
        uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
        }
        if(gpio_get_level(Cool_Btn) == false){      /*Si se pulsa el botón de Modo se cambia e imprime*/
            Cool_Heat_State = !Cool_Heat_State;
            sprintf(mensaje, "Modo: %s\n", Cool_Heat_State ? "HEAT" : "COOL");
            uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
        }
    }   
}

/*! 
* Funcion: Initialize_GPIO
* Pre-condiciones: 
* Descripcion: Inicializar GPIO
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void Initialize_GPIO() {
    gpio_config_t io_conf;

    /*Configura Enc_Apg_Btn*/
    io_conf.pin_bit_mask = (0x01 << Enc_Apg_Btn);
    io_conf.mode = GPIO_MODE_INPUT;                 /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;          /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        /*Resistencia pullup activa*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   /*Resistencia pulldown desactiva*/
    gpio_config(&io_conf);

    /*Configura S_IN_Btn*/
    io_conf.pin_bit_mask = (0x01 << S_IN_Btn);
    io_conf.mode = GPIO_MODE_INPUT;                 /*Como entrada*/
    io_conf.intr_type = GPIO_INTR_NEGEDGE;          /*Interrupción en flanco de bajada*/
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        /*Resistencia pullup activa*/
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   /*Resistencia pulldown desactiva*/
    gpio_config(&io_conf);

    /*Configura S_OUT_Btn*/
    io_conf.pin_bit_mask = (0x01 << S_OUT_Btn);
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

    /*Configura RED_LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << RED_LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);
    gpio_set_level(RED_LED_PIN, true);    /*Apaga el RGB al inicializar*/

    /*Configura GREEN_LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << GREEN_LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);
    gpio_set_level(GREEN_LED_PIN, true);    /*Apaga el RGB al inicializar*/

    /*Configura BLUE_LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << BLUE_LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);
    gpio_set_level(BLUE_LED_PIN, true);    /*Apaga el RGB al inicializar*/

    /*Configura LED_PIN*/
    io_conf.pin_bit_mask = (0x01 << LED_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;            /*Como salida*/
    gpio_config(&io_conf);

    /*Configura las interrupciones de los botones*/
    gpio_install_isr_service(0);
    gpio_isr_handler_add(Enc_Apg_Btn, Enc_Apg_isr_handler, (void*) Enc_Apg_Btn);
    gpio_isr_handler_add(Modo_Btn, Modo_Cool_isr_handler, NULL);
    gpio_isr_handler_add(Cool_Btn, Modo_Cool_isr_handler, NULL);
    gpio_isr_handler_add(S_IN_Btn, IN_OUT_isr_handler, NULL);
    gpio_isr_handler_add(S_OUT_Btn, IN_OUT_isr_handler, NULL);
}

/*! 
* Funcion: Initialize_UART
* Pre-condiciones: 
* Descripcion: Inicializar UART
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void Initialize_UART() {
    uart_config_t uart_config = {
        .baud_rate = 115200,                    /*BaudRate: 115200*/
        .data_bits = UART_DATA_8_BITS,          /*Resolución: 8 bits*/
        .parity    = UART_PARITY_DISABLE,       /*Sin paridad*/
        .stop_bits = UART_STOP_BITS_1,          /*Un bit de paro*/
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE   /*Control de flujo desactivado*/
    };

    uart_param_config(UART_NUM_0, &uart_config);    /*Configurado en el UART0, pines predeterminados*/
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
}

/*! 
* Funcion: Temperature_Control
* Pre-condiciones: 
* Descripcion: Inicializa el ADC
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Initialize_ADC(){
    adc1_config_width(ADC_WIDTH_BIT_12);                    /*Ancho de banda de 12 bits (0 a 4095)*/
    adc1_config_channel_atten(TEMPAMB_PIN, ADC_ATTEN_DB_0);     /*Ch1 para leer el NTC*/
    adc1_config_channel_atten(TEMCOR_PIN, ADC_ATTEN_DB_0);  /*Ch3 para leer el potenciómetrom*/
}


/*! 
* Funcion: Initialize_OLED
* Pre-condiciones: 
* Descripcion: Inicializa la pantalla OLED
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Initialize_OLED(){
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO); /*Se cofiguran los pines de la oled*/
	ssd1306_init(&dev, 128, 64);        /*Se configura la resolución*/
    ssd1306_clear_screen(&dev, false);  /*Se limpia la pantalla para empezar a imprimir*/
    //ssd1306_display_text(&dev, 0, "OLED Iniciado", 14, false);
}

/*! 
* Funcion: Access_Control
* Pre-condiciones: 
* Descripcion: Controla el acceso de las personas en base a 
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Access_Control(){
    if(IN_Pushed == true){      /*Si se pulsa el botón de entrada...*/
        if(Cantidad_Actual < Cantidad_Max && TEMCOR < 37){
            Door_State = OPEN;
            ssd1306_display_text(&dev, 2, "for 5 seconds", 16, false);  /*Informa al usuario el estado*/
            uart_write_bytes(UART_NUM_0, "\nDoor open for 5 seconds\n", 25);
            vTaskDelay(pdMS_TO_TICKS(5000));     /*Esperar 5 segundos*/
            Door_State = CLOSED;
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(50));     /*Espera 50 milisegundos para evitar errores*/
            Cantidad_Actual++;
        }
        else if (Cantidad_Actual >= Cantidad_Max){
            ssd1306_display_text(&dev, 2, "We are full,wait", 16, false);  /*Informa al usuario el estado*/
            uart_write_bytes(UART_NUM_0, "\nWe are full, please wait\n", 18);
            vTaskDelay(pdMS_TO_TICKS(1500));     /*Esperar 1.5 segundos spara facilitar lectura*/
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(50));     /*Espera 50 milisegundos para evitar errores*/
        }
        else{
            Alarma_State = ON;                  /*Activa la alarma RGB*/
            ssd1306_display_text(&dev, 2, "Tmp out of range", 16, false);  /*Informa al usuario el estado*/
            uart_write_bytes(UART_NUM_0, "\nTemperature out of range\n", 26);
            vTaskDelay(pdMS_TO_TICKS(5000));     /*Esperar 5 segundos*/
            Alarma_State = OFF;                 /*Desactiva la alarma RGB*/
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(50));     /*Espera 50 milisegundos para evitar errores*/
        }
        IN_Pushed = false;      /*Desactiva la bandera*/
    }
    if(OUT_Pushed == true){     /*Si se pulsa el botón de salida...*/
        uart_write_bytes(UART_NUM_0, "\nAlguien ha salido\n", 18);
        if(Cantidad_Actual > 0){
            Cantidad_Actual--;
        }
        OUT_Pushed = false;     /*Desactivva la bandera*/
    }
}

/*! 
* Funcion: Temperature_Control
* Pre-condiciones: 
* Descripcion: Controla la lectura e impresión del ADC
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Temperature_Control(){
    TEMPAMB = (adc1_get_raw(TEMPAMB_PIN) * 50.0) / 4095.0;    /*Lee el valor del TEMPAMB (pot 10K; 0 a 50 grados)*/
    sprintf(mensaje, "TEMPAMB: %0.2f C\n", TEMPAMB);
    ssd1306_display_text(&dev, 5, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(50));                          /*Espera 50 milisegundos para evitar errores*/
    TEMCOR = (adc1_get_raw(TEMCOR_PIN) * 50.0) / 4095.0;        /*Lee el valor del TEMCOR (pot 10K; 0 a 50 grados)*/
    sprintf(mensaje, "TEMCOR: %0.2f C\n", TEMCOR);
    ssd1306_display_text(&dev, 6, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(50));                          /*Espera 50 milisegundos para evitar errores*/
}

/*! 
* Funcion: UART_Print
* Pre-condiciones: 
* Descripcion: Imprime los estados del sistema en la UART
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  

void UART_Print(){
    sprintf(mensaje, "\nSistema: %s          ", Enc_Apg_State ? "ON" : "OFF");  /*Estado del sistema*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "Setpoint: %0.2f C\n", SetPoint);                            /*Estado de la puerta*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "Personas: %d          ", Cantidad_Actual);    /*Cantidad de personas dentro*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "Fan: %s\n", Fan_State ? "ON" : "OFF");              /*Estado del ventilador*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "Ventilación: %s       ", Modo_State ? "ON" : "AUTO");   /*Modo de la ventilación*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "Modo de control: %s\n", Modo_State ? "HEAT" : "COOL");   /*Modo de control*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "TEMPAMB: %0.2f C     ", TEMPAMB);            /*Valor de TEMPAMB*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    sprintf(mensaje, "TEMCOR: %0.2f C\n", TEMCOR);                  /*Valor de TEMCOR*/
    uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
    vTaskDelay(pdMS_TO_TICKS(500));                          /*Espera 500 milisegundos para volver a imprimir*/
}

/*! 
* Funcion: States_Control
* Pre-condiciones: 
* Descripcion: Controla el ventilador en base a los modos seleccionados
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void States_Control(){
    if(Modo_State == AUTO){             /*Sistema de temperatura en modo automático*/
        switch(Cool_Heat_State){
           case COOL:                   /*Si el sistema está en COOL*/
            if(TEMPAMB > SetPoint)      /*Y la temperatura es mayor al SetPoint*/
                Fan_State = ON;         /*Enciende el ventilador*/
            else
                Fan_State = OFF;        /*De lo contrario lo apaga*/
           break;
           case HEAT:                   /*Si el sistema está en HEAT*/
            if(TEMPAMB <= SetPoint)     /*Y la temperatura es menor al SetPoint*/
                Fan_State = ON;         /*Enciende el ventilador*/
            else
                Fan_State = OFF;        /*De lo contrario lo apaga*/
           break;
        }
    }
    if(Modo_State == ON){               /*Sistema de temperatura en modo encendido*/
        if(Cool_Heat_State == COOL)     /*Si está en modo COOl, enciende el ventilador*/
            Fan_State = ON;
        else                            /*Si está en modo HEAT lo apaga*/
            Fan_State = OFF;
    }
    sprintf(mensaje, "DOOR: %s\n", Door_State ? "OPEN  " : "CLOSED");   /*Verifica el estado de la puerta*/
    ssd1306_display_text(&dev, 1, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(50));     /*Espera 50 milisegundos para evitar errores*/
    sprintf(mensaje, "FAN: %s\n", Fan_State ? "ON" : "OFF");      /*Verifica el estado del ventilador*/
    ssd1306_display_text(&dev, 4, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(50));     /*Espera 50 milisegundos para evitar errores*/
}


/*! 
* Funcion: OLED_Heartbeat
* Pre-condiciones: Ninguna
* Descripcion: 
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void OLED_Heartbeat(){
    while (true){
        if (Enc_Apg_State == ENCENDIDO){    /*Solo si el sistema está encendido*/
            LED_Status = !LED_Status;       /*Alterna el estado del LED*/
            gpio_set_level(LED_PIN, LED_Status);    /*Cambia el valor en el pin de salida*/

            States_Control();           /*Imprime el estado del ventilador y la puerta*/

            if(Alarma_State == true){       /*Solo si la alarma está activa*/
                RGB_Status = !RGB_Status;       /*Alterna el estado del RGB*/
                gpio_set_level(RED_LED_PIN, !RGB_Status);     /*Cambia el valor de salida de los pines*/
                gpio_set_level(BLUE_LED_PIN, RGB_Status);
            }
            else{
                gpio_set_level(RED_LED_PIN, true);     /*Apaga el RGB*/
                gpio_set_level(BLUE_LED_PIN, true);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));     /*Esperar 200 milisegundos*/
    }
}

/*************** FINAL DE LAS FUNCIONES ***************************************************************************/