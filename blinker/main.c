/*
 * qpn_blinker.c
 *
 * Created: 11.08.2021 10:57:12
 * Author : user
 */

#include "qpc.h"    /* QP-nano API           */
#include "bsp.h"    /* Board Support Package */
#include "blinky.h" /* Application interface */

Q_DEFINE_THIS_FILE

int main(void)
{
    static QEvt const* blinky_queueSto[10]; /* event queue buffer for Blinky */

    QF_init();  /* initialize the framework */
    BSP_init(); /* initialize the BSP */

    /* instantiate and start the Blinky active object */
    Blinky_ctor();                        /* in C you must explicitly call the Blinky constructor */
    QACTIVE_START(AO_Blinky,              /* active object to start */
                  1U,                     /* priority of the active object */
                  blinky_queueSto,        /* event queue buffer */
                  Q_DIM(blinky_queueSto), /* the length of the buffer */
                  (void*)0, 0U,           /* private stack (not used) */
                  (QEvt*)0);              /* initialization event (not used) */

    return QF_run(); /* let the framework run the application */
}

/* interrupts */

/*********************************************************************
 * system timer
 *********************************************************************/
ISR(Q_TIMER_vect)
{
    QF_TICK_X(0U, (void*)0); /* process time events for rate 0 */
    // QF_tickXISR(1U); /* process time events for rate 1 */
    Q_TIMER.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

/* Logger Data Register Empty */
ISR(LOGGER_DRE_vect)
{
    BSP_LOGGER_dre_isr();
}

/*********************************************************************
 * ISR BSP_buttonPressed()
 *********************************************************************/
void ISR_buttonPressed(){
    // QACTIVE_POST_ISR((QActive*)&AO_Blinky, Q_BUTTON_PRESSED_SIG, 0U);
};
