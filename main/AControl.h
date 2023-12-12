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
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "driver/uart.h"

/*Definiciones para los pines GPIO*/
#define Enc_Apg_Btn GPIO_NUM_18
#define LED_PIN GPIO_NUM_5

/*Variables del sistema*/
extern bool Enc_Apg_State;

/*Funciones externas del sistema*/
extern void Initialize_GPIO();
extern void Initialize_UART();
extern void Enc_Apg_Sistema();

#endif