/*******************************************************************************
* Title                 :   gpio matrix  
* Filename              :   gpio_matrix.h
* Author                :   Javier Perez Macias, Marco Antonio Calderón Macías, Alejandro Morales Holguín
* Origin Date           :   04/12/2023
* Version               :   1.0.0
* Compiler              :   CMAKE
* Target                :   ESP32
*
******************************************************************************/
/** @file gpio_matrix.h
 *  @brief Declaraciones de registros del GPIO
 */
#ifndef GPIO_MATRIX_H
#define GPIO_MATRIX_H

#include <stdint.h>

/******************************************************************************
* Constantes del preprocesador
*******************************************************************************/

/**
 * Tipo de registro
 */
#define _I volatile const //Solo lectura (RO)
#define _IO volatile //Escritura y lectura (R/W)
#define _O _IO //Solo escritura (WO)

/**
 * Puertos
 */
#define IO0 0
#define IO1 1
#define IO2 2
#define IO3 3
#define IO4 4
#define IO5 5
#define IO6 6
#define IO7 7
#define IO8 8
#define IO9 9
#define IO10 10
#define IO11 11
#define IO12 12
#define IO13 13
#define IO14 14
#define IO15 15
#define IO16 16
#define IO17 17
#define IO18 18
#define IO19 19
#define IO20 20
#define IO21 21
#define IO22 22
#define IO23 23
#define IO24 24
#define IO25 25
#define IO26 26
#define IO27 27
#define IO28 28
#define IO29 29
#define IO30 30
#define IO31 31
#define IO32 32
#define IO33 33
#define IO34 34
#define IO35 35
#define IO36 36
#define IO37 37
#define IO38 38
#define IO39 39

#define LED4 IO2
#define LED3 IO4
#define LED2 IO16
#define LED1 IO17

/**
 * Interrupciones parametros
 */
#define APP_CPU     0x01
#define APP_CPU_N   0x02
#define PRO_CPU     0x04
#define PRO_CPU_N   0x08

#define DISABLE         0x00
#define RISING_EDGE     0x01
#define FALLING_EDGE    0x02
#define ANY_EDGE        0x03
#define LOW_LEVEL       0x04
#define HIGH_LEVEL      0x05


/**
 * Direcciones de registros
 */
#define PERIFERAL_BASE          ((uint32_t) 0x3FF00000)
#define GPIO_BASE_DIR           ((uint32_t)0x3FF44004)
#define GPIO_OUT_DIR            (GPIO_BASE_DIR)
#define GPIO_OUT_W1TS_DIR       (GPIO_BASE_DIR + 0x0004)
#define GPIO_OUT_W1TC_DIR       (GPIO_BASE_DIR + 0x0008)
#define GPIO_OUT_1_DIR          (GPIO_BASE_DIR + 0x000C)
#define GPIO_OUT_1_W1TS_DIR     (GPIO_BASE_DIR + 0x0010)
#define GPIO_OUT_1_W1TC_DIR     (GPIO_BASE_DIR + 0x0014)
#define GPIO_ENABLE_DIR         (GPIO_BASE_DIR + 0x001C)
#define GPIO_ENABLE_1_DIR       (GPIO_BASE_DIR + 0x0028)
#define GPIO_IN_DIR             (GPIO_BASE_DIR + 0x0038)
#define GPIO_IN_1_DIR           (GPIO_BASE_DIR + 0x003C)
#define GPIO_PIN_DIR            (GPIO_BASE_DIR + 0x0084)
#define GPIO_FUNC_OUT_SEL_DIR   (0x3FF44530)

/**
 * Punteros a los registros
 */
#define GPIO_OUT            ((GPIO_GENERIC_O_0*) GPIO_OUT_DIR)
#define GPIO_OUT_W1TS       ((GPIO_GENERIC_O_0*) GPIO_OUT_W1TS_DIR)
#define GPIO_OUT_W1TC       ((GPIO_GENERIC_O_0*) GPIO_OUT_W1TC_DIR)
#define GPIO_OUT_1          ((GPIO_GENERIC_O_1*) GPIO_OUT_1_DIR)
#define GPIO_OUT_1_W1TS     ((GPIO_GENERIC_O_0*) GPIO_OUT_1_W1TS_DIR)
#define GPIO_OUT_1_W1TC     ((GPIO_GENERIC_O_0*) GPIO_OUT_1_W1TC_DIR)
#define GPIO_IN             ((GPIO_GENERIC_I_0*) GPIO_IN_DIR)
#define GPIO_IN_1           ((GPIO_GENERIC_I_1*) GPIO_IN_1_DIR)
#define GPIO_ENABLE         ((GPIO_GENERIC_O_0*) GPIO_ENABLE_DIR)
#define GPIO_ENABLE_1       ((GPIO_GENERIC_O_1*) GPIO_ENABLE_1_DIR)


/******************************************************************************
* Macros
*******************************************************************************/
#define GPIO_PIN(X)             (GPIO_PIN_DIR + (4*X))
#define GPIO_FUNC_OUT_SEL(X)    ((GPIO_FUNC_OUT_SEL_GENERIC*)(GPIO_FUNC_OUT_SEL_DIR + (4*X)))


/******************************************************************************
* Typedefs
*******************************************************************************/

/**
 * Declaracion generica del registro OUT del puerto 0 hasta el 31
 */
typedef struct 
{   
    _IO uint32_t    REG_IO;
}GPIO_GENERIC_O_0;

/**
 * Declaracion generica del registro OUT del puerto 32 hasta el 39
 */
typedef struct
{
    _IO uint32_t    REG_IO :7;
        uint32_t    RESERVED:24;
} GPIO_GENERIC_O_1;

/**
 * Declaracion generica del registro IN del puerto 0 hasta el 31
 */
typedef struct 
{   
    _I  uint32_t    REG_IO;
} GPIO_GENERIC_I_0;

/**
 * Declaracion generica del registro IN del puerto 32 hasta el 39
 */
typedef struct 
{   
    _I uint32_t    REG_IO      :7;
       uint32_t    RESERVED    :24;
} GPIO_GENERIC_I_1;

/**
 * Declaracion generica del GPIO_PIN
*/
typedef union 
{
    uint32_t reg;
    struct
    {
        uint32_t RESERVED1 :2;
        _IO uint32_t PAD_DRIVER :1;
        uint32_t RESERVED2 :4;
        _IO uint32_t INT_TYPE :3;
        _IO uint32_t WAKEUP_ENABLE :1;
        uint32_t RESERVED3 :2;
        _IO uint32_t INT_ENA :5;
        uint32_t RESERVED4 :13;
    }campo;
} GPIO_PIN_GENERIC;

/**
 * Declaracion generica del GPIO_FUNC_OUT
*/
typedef struct
{
    _IO uint32_t REG;
}GPIO_FUNC_OUT_SEL_GENERIC;



#endif
/*************** FIN DEL ARCHIVO ***************************************************************************/