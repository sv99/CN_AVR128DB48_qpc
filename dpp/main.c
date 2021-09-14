/*****************************************************************************
 * Product: DPP example
 * Last updated for version 6.4.0
 * Last updated on  2019-02-08
 *
 *                    Q u a n t u m  L e a P s
 *                    ------------------------
 *                    Modern Embedded Software
 *
 * Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
 *
 * This program is open source software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, this program may be distributed and modified under the
 * terms of Quantum Leaps commercial licenses, which expressly supersede
 * the GNU General Public License and are specifically designed for
 * licensees interested in retaining the proprietary status of their code.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <www.gnu.org/licenses/>.
 *
 * Contact information:
 * <www.state-machine.com/licensing>
 * <info@state-machine.com>
 *****************************************************************************/
#include "qpc.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/*..........................................................................*/
int main()
{
    static QEvt const* tableQueueSto[N_PHILO];
    static QEvt const* philoQueueSto[N_PHILO][N_PHILO];
    static QSubscrList subscrSto[MAX_PUB_SIG];
    static QF_MPOOL_EL(TableEvt) smlPoolSto[2 * N_PHILO]; /* small pool */
    uint8_t n;

    Philo_ctor(); /* instantiate all Philosopher active objects */
    Table_ctor(); /* instantiate the Table active object */

    BSP_init(); /* initialize the Board Support Package */
    BSP_randomSeed(100U);

    QF_init();   /* initialize the framework and the underlying RT kernel */
    QS_INIT(0U); /* initialize Q_SPY */
    QS_GLB_FILTER(QS_ALL_RECORDS);
    QS_GLB_FILTER(-QS_QF_TICK);
    // QS_GLB_FILTER(QS_UA_RECORDS);

    /* initialize publish-subscribe... */
    QF_psInit(subscrSto, Q_DIM(subscrSto));

    /* initialize event pools... */
    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    /* start the active objects... */
    for (n = 0U; n < N_PHILO; ++n) {
        QACTIVE_START(AO_Philo[n],             /* AO to start */
                      (uint_fast8_t)(n + 1),   /* QP priority of the AO */
                      philoQueueSto[n],        /* event queue storage */
                      Q_DIM(philoQueueSto[n]), /* queue length [events] */
                      (void*)0,                /* stack storage (not used) */
                      0U,                      /* size of the stack [bytes] */
                      (QEvt*)0);               /* initialization event */
    }
    QACTIVE_START(AO_Table,                    /* AO to start */
                  (uint_fast8_t)(N_PHILO + 1), /* QP priority of the AO */
                  tableQueueSto,               /* event queue storage */
                  Q_DIM(tableQueueSto),        /* queue length [events] */
                  (void*)0,                    /* stack storage (not used) */
                  0U,                          /* size of the stack [bytes] */
                  (QEvt*)0);                   /* initialization event */

    return QF_run(); /* run the QF application */
}

/* interrupts */

/*********************************************************************
 * system timer
 *********************************************************************/
ISR(Q_TIMER_vect)
{
    QF_TICK_X(0U, (void*)0); /* process time events for rate 0 */
    // QF_tickXISR(1U); /* process time events for rate 1 */
#ifdef Q_SPY
    BSP_SPY_tick();
#endif
    Q_TIMER.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

///* Logger Data Register Empty */
// ISR(LOGGER_DRE_vect)
//{
//#ifdef Q_SPY
// BSP_SPY_dre_isr();
//#else
// BSP_LOGGER_dre_isr();
//#endif
//}

/*********************************************************************
 * ISR BSP_buttonPressed()
 *********************************************************************/
void ISR_buttonPressed(){
    // QACTIVE_POST_ISR((QActive*)&AO_Blinky, Q_BUTTON_PRESSED_SIG, 0U);
};
