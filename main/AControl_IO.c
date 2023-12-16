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
bool Fan_State = OFF;
bool Door_State = CLOSED;

/*Variables de control de temperatura*/
double SetPoint = 25.0;
double TEMPAMB;
double TEMCOR;

/*Variables de control de acceso*/
int Cantidad_Actual = 0;
int Cantidad_Max = 5;


/******************************************************************************
* Definicion de funciones
*******************************************************************************/
/*! 
* Funcion: Initialize_GPIO
* Pre-condiciones: Ninguna
* Descripcion: Inicializar GPIO
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/ 
void Initialize_GPIO() {
    gpio_config_t io_conf;

    /*Configura Enc_Apg_Btn*/
    GPIO_INPUT_ENABLE(Enc_Apg_Btn, PULLUP);
    
    /*Configura S_IN_Btn*/
    GPIO_INPUT_ENABLE(S_IN_Btn, PULLUP);

    /*Configura S_OUT_Btn*/
    GPIO_INPUT_ENABLE(S_OUT_Btn, PULLUP);

    /*Configura Modo_Btn*/
    GPIO_INPUT_ENABLE(Modo_Btn, PULLUP);

    /*Configura Cool_Btn*/
    GPIO_INPUT_ENABLE(Cool_Btn, PULLUP);

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
    GPIO_OUTPUT_ENABLE(LED_PIN);

    /*Configura FAN_PIN*/
    GPIO_OUTPUT_ENABLE(FAN_PIN);

    /*Configura DOOR_PIN*/
    GPIO_OUTPUT_ENABLE(DOOR_PIN);
}

/*! 
* Funcion: Initialize_UART
* Pre-condiciones: Ninguna
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
* Funcion: Initialize_ADC
* Pre-condiciones: Ninguna
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
* Pre-condiciones: Ninguna
* Descripcion: Inicializa la pantalla OLED
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Initialize_OLED(){
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO); /*Se cofiguran los pines de la oled*/
	ssd1306_init(&dev, 128, 64);        /*Se configura la resolución*/
    ssd1306_clear_screen(&dev, false);  /*Se limpia la pantalla para empezar a imprimir*/
}

/*! 
* Funcion: Access_Control
* Pre-condiciones: Encendido del sistema y activación de las banderas IN_Pushed u OUT_Pushed
* Descripcion: Controla el acceso de las personas en base a las banderas de pulsación de IN y OUT
               modifica los estados de la puerta y la alarma.
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void Access_Control(){
    if(!GPIO_INPUT_READ(S_IN_Btn)){      /*Si se pulsa el botón de entrada...*/
        if(Cantidad_Actual < Cantidad_Max && TEMCOR < 37){
            Door_State = OPEN;
            GPIO_OUTPUT_SET(DOOR_PIN, !Door_State);
            ssd1306_display_text(&dev, 2, "for 5 seconds", 16, false);  /*Informa al usuario el estado*/
            uart_write_bytes(UART_NUM_0, "\nDoor open for 5 seconds\n", 25);
            vTaskDelay(pdMS_TO_TICKS(5000));     /*Esperar 5 segundos*/
            Door_State = CLOSED;
            GPIO_OUTPUT_SET(DOOR_PIN, !Door_State);
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
    }
    if(!GPIO_INPUT_READ(S_OUT_Btn)){     /*Si se pulsa el botón de salida...*/
        uart_write_bytes(UART_NUM_0, "\nAlguien ha salido\n", 18);
        if(Cantidad_Actual > 0){
            Cantidad_Actual--;
        }
    }
}

/*! 
* Funcion: Temperature_Control
* Pre-condiciones: Activación de la bandera de encendido del sistema (Enc_Apg_State)
* Descripcion: Controla la lectura e impresión del ADC por la OLED
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
* Funcion: States_Control
* Pre-condiciones: Activación de la bandera de encendido del sistema (Enc_Apg_State)
* Descripcion: Controla el estado ventilador en base a los modos seleccionados e imprime en la OLED
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

    GPIO_OUTPUT_SET(FAN_PIN, !Fan_State);
}


/*! 
* Funcion: OLED_Heartbeat
* Pre-condiciones: Ninguna
* Descripcion: Controla el encendido del LED indicador del sistema y de la alarma RGB, invoca constantemente
               la función States_Control
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  
void LED_Heartbeat(){
    while (true){
        if (Enc_Apg_State == ENCENDIDO){    /*Solo si el sistema está encendido*/
            LED_Status = !LED_Status;       /*Alterna el estado del LED*/
            GPIO_OUTPUT_SET(LED_PIN,LED_Status);
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
        vTaskDelay(pdMS_TO_TICKS(500));     /*Esperar 500 milisegundos*/
    }
}

/*! 
* Funcion: UART_Print
* Pre-condiciones: Activación de la bandera de encendido del sistema (Enc_Apg_State)
* Descripcion: Imprime los estados del sistema en la UART
* Valores de entrada: Ninguno
* Valores de salida: Ninguno
*/  

void UART_Print(){
    while(true){
        if (Enc_Apg_State){
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
            sprintf(mensaje, "Modo de control: %s\n", Cool_Heat_State ? "HEAT" : "COOL");   /*Modo de control*/
            uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
            sprintf(mensaje, "TEMPAMB: %0.2f C     ", TEMPAMB);            /*Valor de TEMPAMB*/
            uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
            sprintf(mensaje, "TEMCOR: %0.2f C\n", TEMCOR);                  /*Valor de TEMCOR*/
            uart_write_bytes(UART_NUM_0, mensaje, strlen(mensaje));
            vTaskDelay(pdMS_TO_TICKS(500));                          /*Espera 500 milisegundos para volver a imprimir*/
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/*************** FINAL DE LAS FUNCIONES ***************************************************************************/