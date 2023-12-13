/*******************************************************************************
* Title                 :   AControl 
* Filename              :   AControl.h
* Author                :   Marco Calderón
* Origin Date           :   11/12/2023
* Version               :   1.0.0
* Target                :   ESP32
******************************************************************************/

#ifndef _AControl_h_
#define _AControl_h_

#pragma once

/* Archivos de cabecera del programa*/
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

/*Definiciones para los pines GPIO*/
#define Enc_Apg_Btn GPIO_NUM_18
#define Modo_Btn    GPIO_NUM_26
#define Cool_Btn    GPIO_NUM_25
#define S_IN_Btn    GPIO_NUM_13
#define S_OUT_Btn   GPIO_NUM_12

#define LED_PIN     GPIO_NUM_5

/*Definiciones del sistema*/
#define ENCENDIDO   true
#define APAGADO     false

#define AUTO    0
#define ON      1
#define COOL    0
#define HEAT    1

/*Variables del sistema*/
extern bool Enc_Apg_State;
extern SSD1306_t dev;       /*Estructura de la pantalla OLED*/

/*Funciones externas del sistema*/
extern void Initialize_GPIO();
extern void Initialize_UART();
extern void Initialize_OLED();
extern void LED_Heartbeat();

#endif