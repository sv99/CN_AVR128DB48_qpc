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
//#define BSP_TICKS_PER_SEC 200U
// period 10ms
#define BSP_TICKS_PER_SEC 100U

/* after declare F_CPU and BSP_TICKS_PER_SEC */
#include "bsp_pins.h"

/* Normal Mode, Baud register value */
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void BSP_init(void);
void BSP_initTick(void);
void BSP_terminate(int16_t result);

void BSP_displayPaused(uint8_t paused);
void BSP_displayPhilStat(uint8_t n, char_t const* stat);

void BSP_randomSeed(uint32_t seed); /* random seed */
uint32_t BSP_random(void);          /* pseudo-random generator */

/* LOGGER UART */
void BSP_LOGGER_init(void);
void BSP_LOGGER_dre_isr(void);
#ifdef Q_SPY
void BSP_SPY_dre_isr(void);
void BSP_SPY_tick(void);
#endif

void BSP_onboardLedOff(void);
void BSP_onboardLedOn(void);

void BSP_ledOff(uint8_t n);
void BSP_ledOn(uint8_t n);

#endif // BSP_H
