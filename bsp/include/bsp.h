/*
 * bsp.h
 *
 * Created: 11.08.2021 13:47:34
 *  Author: user
 */
#ifndef BSP_H
#define BSP_H

#ifndef F_CPU
#define F_CPU 4000000UL
#endif

// period 5ms
#define BSP_TICKS_PER_SEC 200U
// period 10ms
// #define BSP_TICKS_PER_SEC 100U

/* after declare F_CPU and BSP_TICKS_PER_SEC */
#include "bsp_pins.h"

/* Normal Mode, Baud register value */
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void BSP_initSetup(void);
void BSP_initTick(void);

/* LOGGER UART */
void BSP_LOGGER_init(void);
void BSP_LOGGER_dre_isr(void);

void BSP_onboardLedOff(void);
void BSP_onboardLedOn(void);

#endif // BSP_H
