/*******************************************************************************
* Title                 :   AControl 
* Filename              :   AControl.h
* Author                :   Marco Calderón, Javier Perez Macias, Alejandro Morales Holguín
* Origin Date           :   11/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/
/** @file AControl.h
 *  @brief 
 */
#ifndef _AControl_h_
#define _AControl_h_

#pragma once

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/adc.h"
#include "ssd1306.h"
#include "font8x8_basic.h"

#include "drivers/gpio_drivers.h"

/******************************************************************************
* Constantes del preprocesador
*******************************************************************************/
#define Enc_Apg_Btn IO18
#define Modo_Btn    IO19
#define Cool_Btn    IO23
#define S_IN_Btn    IO16
#define S_OUT_Btn   IO17

#define FAN_PIN         IO2
#define DOOR_PIN        IO4
#define LED_PIN         IO5
#define RED_LED_PIN     IO14
#define BLUE_LED_PIN    IO12
#define GREEN_LED_PIN   IO13

#define TEMCOR_PIN      ADC1_CHANNEL_6
#define TEMPAMB_PIN     ADC1_CHANNEL_7

/*Definiciones del sistema y sus estados*/
#define ENCENDIDO   true
#define APAGADO     false

#define AUTO    0
#define OFF     0
#define ON      1
#define COOL    0
#define HEAT    1
#define CLOSED  0
#define OPEN    1

/******************************************************************************
* Variables
*******************************************************************************/
extern bool Enc_Apg_State;
extern SSD1306_t dev;       /*Estructura de la pantalla OLED*/
extern bool Modo_State;
extern bool Cool_Heat_State;

/*Variable de mensaje para la correcta impresión del UART y la OLED*/
extern char mensaje[50];

/******************************************************************************
* Prototipo de funciones
*******************************************************************************/
extern void Initialize_GPIO();
extern void Initialize_UART();
extern void Initialize_ADC();
extern void Initialize_OLED();
extern void Access_Control();
extern void Temperature_Control();
extern void UART_Print();
extern void States_Control();
extern void OLED_Heartbeat();

#endif
/*************** FIN DEL ARCHIVO ***************************************************************************/