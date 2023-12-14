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
bool Fan_State = OFF;
bool Door_State = CLOSED;
bool IN_Pushed = false;
bool OUT_Pushed = false;

double SetPoint = 25.0;
double TEMPAMB;
double TEMCOR;

uint32_t Cantidad_Actual = 0;
uint32_t Cantidad_Max = 5;

char mensaje[50];

/*Manejador de interrupción para el botón ON/OFF*/
void IRAM_ATTR Enc_Apg_isr_handler(void* arg) {
    if(Enc_Apg_State != ENCENDIDO){
        Enc_Apg_State = true;                          /*Encender el sistema al presionar*/
        uart_write_bytes(UART_NUM_0, "\nSistema: ON.", 12);     /*Informa por la UART*/
    }
}

/*Manejador de interrupción para los sensores de entrada y salida*/
void IRAM_ATTR IN_OUT_isr_handler(void* arg) {
    if(gpio_get_level(S_IN_Btn) == false){      /*Al detectar una entrada*/
        IN_Pushed = true;                       /*Activa la bandera de acceso*/
    }
    if(gpio_get_level(S_OUT_Btn) == false){      /*Al detectar una salida*/
        OUT_Pushed = true;                       /*Activa la bandera de salida*/
    }
}

/*Manejador de interrupción para los botones Modo y Cool*/
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

/*Inicializa el ADC*/
void Initialize_ADC(){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(NTC_PIN, ADC_ATTEN_DB_0);     /*Ch1 para leer el NTC*/
    adc1_config_channel_atten(TEMCOR_PIN, ADC_ATTEN_DB_0);  /*Ch3 para leer el potenciómetrom*/
}

/*Inicializa la pantalla OLED*/
void Initialize_OLED(){
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO); /*Se cofiguran los pines de la oled*/
	ssd1306_init(&dev, 128, 64);        /*Se configura la resolución*/
    ssd1306_clear_screen(&dev, false);  /*Se limpia la pantalla para empezar a imprimir*/
    //ssd1306_display_text(&dev, 0, "OLED Iniciado", 14, false);
}

/*Controla el acceso de las personas en base a */
void Access_Control(){
    if(IN_Pushed == true){      /*Si se pulsa el botón de entrada...*/
        uart_write_bytes(UART_NUM_0, "\nEntrada", 8);
        if(Cantidad_Actual < Cantidad_Max && TEMCOR < 37){
            Door_State = OPEN;
            ssd1306_display_text(&dev, 2, "for 5 seconds", 16, false);  /*Informa al usuario el estado*/
            vTaskDelay(pdMS_TO_TICKS(5000));     /*Esperar 5 segundos*/
            Door_State = CLOSED;
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
            Cantidad_Actual++;
        }
        else if (Cantidad_Actual >= Cantidad_Max){
            ssd1306_display_text(&dev, 2, "We are full,wait", 16, false);  /*Informa al usuario el estado*/
            vTaskDelay(pdMS_TO_TICKS(1000));     /*Esperar 1 segundo para facilitar lectura*/
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
        }
        else{
            ssd1306_display_text(&dev, 2, "Tmp out of range", 16, false);  /*Informa al usuario el estado*/
            vTaskDelay(pdMS_TO_TICKS(5000));     /*Esperar 5 segundos*/
            Door_State = CLOSED;
            ssd1306_clear_line(&dev, 2, false);  /*Borra el mensaje*/
            vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
        }
        IN_Pushed = false;      /*Desactiva la bandera*/
    }
    if(OUT_Pushed == true){     /*Si se pulsa el botón de salida...*/
        uart_write_bytes(UART_NUM_0, "\nSalida", 8);
        if(Cantidad_Actual > 0){
            Cantidad_Actual--;
        }
        OUT_Pushed = false;     /*Desactivva la bandera*/
    }
}

/*Controla la lectura e impresión del ADC*/
void Temperature_Control(){
    TEMPAMB = adc1_get_raw(NTC_PIN);                            /*Lee el valor del TEMPAMB*/
    sprintf(mensaje, "TEMPAMB: %f\n", TEMPAMB);
    ssd1306_display_text(&dev, 5, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(10));                          /*Espera 10 milisegundos para evitar errores*/
    TEMCOR = (adc1_get_raw(TEMCOR_PIN) * 50.0) / 4095.0;        /*Lee el valor del TEMCOR (pot 10K; 0 a 50 grados)*/
    sprintf(mensaje, "TEMCOR: %0.2f\n C", TEMCOR);
    ssd1306_display_text(&dev, 6, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
}

/*Controla el ventilador en base a los modos seleccionados*/
void States_Control(){
    if(Modo_State == AUTO){
        if(Cool_Heat_State == COOL){

        }
        else{

        }
    }
    if(Modo_State == ON){
        if(Cool_Heat_State == COOL)
            Fan_State = ON;
        else
            Fan_State = OFF;
    }
    sprintf(mensaje, "DOOR: %s\n", Door_State ? "OPEN  " : "CLOSED");   /*Verifica el estado de la puerta*/
    ssd1306_display_text(&dev, 1, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
    sprintf(mensaje, "FAN: %s\n", Fan_State ? "ON" : "OFF");      /*Verifica el estado del ventilador*/
    ssd1306_display_text(&dev, 4, mensaje, strlen(mensaje), false);     /*Y lo informa por la OLED*/
    vTaskDelay(pdMS_TO_TICKS(10));     /*Espera 10 milisegundos para evitar errores*/
}

void OLED_Heartbeat(){
    while (true){
        if (Enc_Apg_State == ENCENDIDO){    /*Solo si el sistema está encendido*/
            LED_Status = !LED_Status;       /*Cambia el estado del LED*/
            gpio_set_level(LED_PIN, LED_Status);    /*Y coloca el estado en el pin de salida*/
            States_Control();           /*Imprime el estado del ventilador y la puerta*/
        }
        vTaskDelay(pdMS_TO_TICKS(100));     /*Esperar 200 milisegundos*/
    }
}