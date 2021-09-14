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
#include <avr/cpufunc.h>
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

/* Local objects -----------------------------------------------------------*/
static uint32_t l_rnd; /* random seed */

#ifdef Q_SPY
QSTimeCtr QS_tickTime_;
QSTimeCtr QS_tickPeriod_;

/* QSpy source IDs */
static QSpyId const l_SysTick_Handler = {0U};

#endif

void CLOCK_XOSCHF_clock_init(void)
{
    /* Enable crystal oscillator with frequency range 16 MHz and 4K cycles start-up time */
    ccp_write_io((uint8_t*)&CLKCTRL.XOSCHFCTRLA, CLKCTRL_RUNSTDBY_bm | CLKCTRL_CSUTHF_4K_gc |
                                                     CLKCTRL_FRQRANGE_16M_gc |
                                                     CLKCTRL_SELHF_XTAL_gc | CLKCTRL_ENABLE_bm);

    /* Confirm crystal oscillator start-up */
    while (!(CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)) {
        ;
    }

    /* Enable Main Clock Prescaler 16M / 4 = 4M */
    ccp_write_io((uint8_t*)&CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm);

    /* Set the main clock to use XOSCHF as source */
    ccp_write_io((uint8_t*)&CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);

    /* Wait for system oscillator changing to complete */
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm) {
        ;
    }

    /* Clear RUNSTDBY for power save during sleep */
    ccp_write_io((uint8_t*)&CLKCTRL.XOSCHFCTRLA, CLKCTRL.XOSCHFCTRLA & ~CLKCTRL_RUNSTDBY_bm);

    /* Change complete and the main clock is 16 MHz */
}

/*********************************************************************
 * BSP_init()
 *********************************************************************/
void BSP_init(void)
{
    /* XOSCHF with external crystal */
    CLOCK_XOSCHF_clock_init();

    /* init board pins... */
    // LED_RED pin out with high level (led off)
    LED_PORT.OUTSET = LED_PIN_bm;
    LED_PORT.DIRSET = LED_PIN_bm;

    // philo stat pin out
    PHIL_PORT.OUTSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm;
    PHIL_PORT.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm;

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

/*..........................................................................*/
void BSP_terminate(int16_t result)
{
    (void)result;
}
/*..........................................................................*/
void BSP_ledOff(uint8_t n)
{
    PHIL_PORT.OUTSET = 1 << n;
}
/*..........................................................................*/
void BSP_ledOn(uint8_t n)
{
    PHIL_PORT.OUTCLR = 1 << n;
}
/*..........................................................................*/
uint32_t BSP_random(void)
{ /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
     * LCG(2^32, 3*7*11*13*23, 0, seed)
     */
    l_rnd = (uint32_t)(l_rnd * (3L * 7L * 11L * 13L * 23L));
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed)
{
    l_rnd = seed;
}

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
#ifdef Q_SPY
    uint16_t b;
    if (LOGGER.STATUS & USART_DREIF_bm) {
        if ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
            LOGGER.TXDATAL = b;
        }
    }

    QF_INT_ENABLE();
    QS_rxParse(); /* parse all the received bytes */

    // if ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
    // while (!(LOGGER.STATUS & USART_DREIF_bm)) {
    //;
    //}
    ///* Start transmission */
    // LOGGER.TXDATAL = b;
    //}
#else
    QF_INT_ENABLE();
#endif
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

/* QS callbacks ============================================================*/
#ifdef Q_SPY

/*..........................................................................*/
uint8_t QS_onStartup(void const* arg)
{
    static uint8_t qsBuf[2 * 1024]; /* buffer for Quantum Spy */
    static uint8_t qsRxBuf[128];    /* buffer for QS-RX channel */

    (void)arg; /* avoid the "unused parameter" compiler warning */

    QS_initBuf(qsBuf, sizeof(qsBuf));
    QS_rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    /* init LOGGER USART - SPY USART */

    QS_tickPeriod_ = 1000 / BSP_TICKS_PER_SEC; /* in mSec */
    QS_tickTime_ = QS_tickPeriod_;             /* to start the timestamp at zero */

    return 1U; /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void)
{
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void)
{
    return QS_tickTime_;
}
/*..........................................................................*/
void QS_onFlush(void)
{
    /* Enable UDRE interrupt */
    // LOGGER.CTRLA |= USART_DREIE_bm;
}
void BSP_SPY_tick(void)
{
    QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
}
/*..........................................................................*/
void BSP_SPY_dre_isr(void)
{
    uint16_t b;

    /* Check if all data is transmitted */
    if ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
        /* Start transmission */
        LOGGER.TXDATAL = b;
    } else {
        /* Disable UDRE interrupt */
        LOGGER.CTRLA &= ~USART_DREIE_bm;
    }
}
/*..........................................................................*/
/*! callback function to reset the target (to be implemented in the BSP) */
void QS_onReset(void)
{
    QF_RESET();
}
/*..........................................................................*/
/*! callback function to execute a user command (to be implemented in BSP) */
void QS_onCommand(uint8_t cmdId, uint32_t param1, uint32_t param2, uint32_t param3)
{
    (void)cmdId;
    (void)param1;
    (void)param2;
    (void)param3;
}

#endif /* Q_SPY */
/*--------------------------------------------------------------------------*/
