/*******************************************************************************
* Title                 :   gpio drivers  
* Filename              :   gpio_drivers.c
* Author                :   Javier Perez Macias, Marco Antonio Calderón Macías, Alejandro Morales Holguín
* Origin Date           :   04/12/2023
* Version               :   1.0.0
* Compiler              :   CMAKE
* Target                :   ESP32
*
******************************************************************************/
/******************************************************************************
* Includes
*******************************************************************************/
#include "gpio_drivers.h"
#include "gpio_iomatrix.h"


/******************************************************************************
* Definicion de funciones
*******************************************************************************/

/*! 
* Funcion: GPIO_OUTPUT_SET
* Pre-condiciones: El puerto debe estar habilitado
* Descripcion: Asigna un valor de salida (0 o 1) al puerto seleccionado 
* Valores de entrada: Puerto, Valor de salida 
* Valores de salida: Ninguno
 */  
void GPIO_OUTPUT_SET(uint8_t port, bool value){
    //Del puerto 34 al 39 no pueden ser salidas
    switch (value)
    {
        case ON: 
            GPIO_OUT_W1TS   -> REG_IO = (1 << port) * !(port > IO31 && port <= IO33);
            GPIO_OUT_1_W1TS -> REG_IO = (1 << (port - IO32)) * (port > IO31 && port <= IO33);
            break;
        case OFF:
            GPIO_OUT_W1TC   -> REG_IO = (1 << port) * !(port > IO31 && port <= IO33);
            GPIO_OUT_1_W1TC -> REG_IO = (1 << (port - IO32)) * (port > IO31 && port <= IO33);
            break;
    }
    return;
}

/*! 
* Funcion: GPIO_OUTPUT_ENABLE
* Pre-condiciones: Ninguna
* Descripcion: Habilita la salida del puerto seleccionado
* Valores de entrada: Puerto
* Valores de salida: Ninguno
*/  
void GPIO_OUTPUT_ENABLE(uint8_t port){
    IO_MUX port_selected;
    port_selected.reg = ioMuxDirections[port]; //Seleccion del puerto
    port_selected.confirguration.MCU_SEL = 0x02;
    GPIO_ENABLE -> REG_IO   = (1 << port) * !(port > IO31 && port <= IO39);
    GPIO_ENABLE_1 -> REG_IO = (1 << (port - IO32)) * (port > IO31 && port <= IO39);
    return;
}

/*! 
* Funcion: GPIO_OUTPUT_DISABLE
* Pre-condiciones: Ninguna
* Descripcion: Deshabilita la salida del puerto seleccionado
* Valores de entrada: Puerto
* Valores de salida: Ninguno
*/  
void GPIO_OUTPUT_DISABLE(uint8_t port){
    GPIO_ENABLE -> REG_IO   ^= (1 << port) * !(port > IO31 && port <= IO39);
    GPIO_ENABLE_1 -> REG_IO ^= (1 << (port - IO32)) * (port > IO31 && port <= IO39);
    return;
}

/*! 
* Funcion: GPIO_INPUT_ENABLE
* Pre-condiciones: Ninguna
* Descripcion: Habilita la entrada del puerto seleccionado
* Valores de entrada: Puerto, tipo de resitencia
* Valores de salida: Ninguno
*/  
void GPIO_INPUT_ENABLE(uint8_t port, bool resistor){
    IO_MUX port_selected;
    port_selected.reg = ioMuxDirections[port]; //Seleccion del puerto
    port_selected.confirguration.FUN_IE = 1; //Habilitacion de la entrada
    port_selected.confirguration.FUN_WPU = !resistor; //Habilitacion del pullup
    port_selected.confirguration.FUN_WPD = resistor; //Habilitacion del pulldown
    return;
}

/*! 
* Funcion: GPIO_INPUT_DISABLE
* Pre-condiciones: Ninguna
* Descripcion: Deshabilita el puerto seleccionado
* Valores de entrada: Puerto
* Valores de salida: Ninguno
*/  
void GPIO_INPUT_DISABLE(uint8_t port){
    IO_MUX port_selected;
    port_selected.reg = ioMuxDirections[port]; //Seleccion del puerto
    port_selected.confirguration.FUN_IE = 0; //Deshabilitacion de la entrada
    port_selected.confirguration.FUN_WPU = 0; //Deshabilitacion del pullup
    port_selected.confirguration.FUN_WPD = 0; //Deshabilitacion del pulldown
    return;
}

/*! 
* Funcion: GPIO_INPUT_READ
* Pre-condiciones: Ninguna
* Descripcion: Lee el puerto seleccionado
* Valores de entrada: Puerto
* Valores de salida: Valor de la lectura
*/  
bool GPIO_INPUT_READ(uint8_t port){
    bool lecture = 0;
    switch (port > IO31 && port <= IO39)
    {
        case true:
            lecture = (((GPIO_IN_1 -> REG_IO) >> (port - IO32))  & 0x01);
            break;
        case false:
            lecture = (((GPIO_IN -> REG_IO) >> port)  & 0x01);
            break;
    }
    return lecture;
}

/*! 
* Funcion: GPIO_INTERRUPTION_SET
* Pre-condiciones: Que el puerto seleccionado sea una entrada
* Descripcion: Habilita la interrupcion de la entrada (Sin terminar)
* Valores de entrada: Puerto, tipo de habilitacion, tipo de interrupcion
* Valores de salida: Ninguno
*/  
void GPIO_INTERRUPTION_SET(uint8_t port, uint8_t ena_type, uint8_t int_type){
    GPIO_PIN_GENERIC port_selected;
    port_selected.reg = GPIO_PIN(port);
    port_selected.campo.INT_ENA = ena_type;
    port_selected.campo.INT_TYPE = int_type;
    return;
}
/*************** FINAL DE LAS FUNCIONES ***************************************************************************/
