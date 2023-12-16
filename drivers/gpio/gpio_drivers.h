/*******************************************************************************
* Title                 :   gpio drivers  
* Filename              :   gpio_drivers.h
* Author                :   Javier Perez Macias, Marco Antonio Calderón Macías, Alejandro Morales Holguín
* Origin Date           :   04/12/2023
* Version               :   1.0.0
* Compiler              :   CMAKE
* Target                :   ESP32
*
******************************************************************************/

/** @file gpio_drivers.h
 *  @brief Funciones y declaraciones para el manejo del GPIO
 */
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

/******************************************************************************
* Includes
*******************************************************************************/
#include "gpio_matrix.h"
#include <stdbool.h> //Uso del tipo de dato bool

/******************************************************************************
* Constantes del preprocesador
*******************************************************************************/
#define IN  0
#define OUT 1
#define ON IN
#define OFF OUT
#define PULLUP OFF
#define PULLDOWN ON


/******************************************************************************
* Prototipo de funciones
*******************************************************************************/
extern void     GPIO_OUTPUT_ENABLE(uint8_t port);
extern void     GPIO_OUTPUT_DISABLE(uint8_t port);
extern void     GPIO_OUTPUT_SET(uint8_t port, bool value);
extern void     GPIO_INPUT_ENABLE(uint8_t port, bool resistor);
extern void     GPIO_INPUT_DISABLE(uint8_t port);
extern bool     GPIO_INPUT_READ(uint8_t port);
extern void     GPIO_INTERRUPTION_SET(uint8_t port, uint8_t int_type, uint8_t signal_type);
#endif
/*************** FIN DEL ARCHIVO ***************************************************************************/
