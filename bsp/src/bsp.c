/*
 * bsp.c
 *
 * Created: 11.08.2021 10:57:12
 * Author : user
 */

/*********************************************************************
 * Include section
 * Add all #includes here.
 *********************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#include "qpc.h" // QP-nano framework
#include "bsp.h" // BSP for this application
#include "logger.h"

/*********************************************************************
 * Define section
 *  Add all #defines here.
 *********************************************************************/

#ifndef F_CPU
#define F_CPU 4000000UL
#endif

#define TIMER_PRESCALER 1U
#define TIMER_PERIOD    ((float)F_CPU / TIMER_PRESCALER / BSP_TICKS_PER_SEC) - 1U

/* Normal Mode, Baud register value */
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

/*********************************************************************
 * BSP_initSetup()
 *********************************************************************/
void BSP_initSetup(void)
{
    /* init board pins... */
    // LED_RED pin out with high level (led off)
    LED_PORT.OUTSET = LED_PIN_bm;
    LED_PORT.DIRSET = LED_PIN_bm;

    /* init uart pins... */
    // LOGGER
    LOGGER_RXD_PORT.DIRCLR = LOGGER_RXD_PIN_bm;
    LOGGER_RXD_CTRL &= ~PORT_PULLUPEN_bm;
    LOGGER_TXD_PORT.OUTCLR = LOGGER_TXD_PIN_bm;
    LOGGER_TXD_PORT.DIRSET = LOGGER_TXD_PIN_bm;

    /* alternative position */
    // PORTMUX.USARTROUTEA |= PORTMUX_USART20_bm;

    BSP_LOGGER_init();
};

/*********************************************************************
 * BSP_initTick()
 *********************************************************************/
void BSP_initTick(void)
{
    /* init TCA0 in the Periodic Interrupt Mode */
    Q_TIMER.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
    Q_TIMER.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
    Q_TIMER.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; // no waveform
    Q_TIMER.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTAEI_bm);   // count clock ticks
    Q_TIMER.SINGLE.PER = TIMER_PERIOD;
};

/*********************************************************************
 * BSP_onboardLedOff()
 *********************************************************************/
void BSP_onboardLedOff(void)
{
    LED_PORT.OUTSET = LED_PIN_bm;
};

/*********************************************************************
 * BSP_onboardLedOn()
 *********************************************************************/
void BSP_onboardLedOn(void)
{
    LED_PORT.OUTCLR = LED_PIN_bm;
};

//============================================================================
// QF callbacks...
void QF_onStartup(void)
{
    BSP_initTick();
}
//............................................................................
void QV_onIdle(void)
{ // called with interrupts DISABLED
    // Put the CPU and peripherals to the low-power mode. You might
    // need to customize the clock management for your application,
    // see the datasheet for your particular AVR MCU.
    // SMCR = (0 << SM0) | (1 << SE); // idle mode, adjust to your project
    // QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts

    QF_INT_ENABLE();
}
//............................................................................
// Q_NORETURN Q_onAssert(char const Q_ROM* const module, int location)
Q_NORETURN Q_onAssert(char_t const* const module, int_t const location)
{
    // implement the error-handling policy for your application!!!
    (void)module;
    (void)location;
    QF_INT_DISABLE(); // disable all interrupts
    QF_RESET();       // reset the CPU
    for (;;) {
    }
}
