/**
 * @file
 * @brief QF/C port to AVR128, cooperative QV kernel
 * @cond
 ******************************************************************************
 * Last updated for version 6.9.3
 * Last updated on  2021-08-26
 *
 *                    Q u a n t u m  L e a P s
 *                    ------------------------
 *                    Modern Embedded Software
 *
 * Copyright (C) 2002-2020 Quantum Leaps, LLC. All rights reserved.
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
 ******************************************************************************
 * @endcond
 */
#ifndef QF_PORT_H
#define QF_PORT_H

/* The maximum number of active objects in the application, see NOTE01 */
#define QF_MAX_ACTIVE 8U

/* The maximum number of system clock tick rates */
#define QF_MAX_TICK_RATE 2U

#define QF_EVENT_SIZ_SIZE   1U
#define QF_EQUEUE_CTR_SIZE  1U
#define QF_MPOOL_SIZ_SIZE   1U
#define QF_MPOOL_CTR_SIZE   1U
#define QF_TIMEEVT_CTR_SIZE 2U

/* GNU-AVR function attribute for "no-return" function */
#define Q_NORETURN __attribute__((noreturn)) void

/* GNU-AVR PROGMEM utilities are not compatible with GNU-AVR-C++! */
#define Q_ROM                __flash // PROGMEM
#define Q_ROM_BYTE(rom_var_) pgm_read_byte_near(&(rom_var_))
#define Q_ROM_PTR(rom_var_)  pgm_read_word_near(&(rom_var_))

/* GNU-AVR do not define extended keyword for allocates const objects to EEPROM */
#define Q_EEPROM

/* QF interrupt disable/enable... */
#define QF_INT_DISABLE() __asm__ __volatile__("cli" ::)
#define QF_INT_ENABLE()  __asm__ __volatile__("sei" ::)

/* QF critical section entry/exit... */
// clang-format off
#define QF_CRIT_STAT_TYPE uint8_t
#define QF_CRIT_ENTRY(stat_)    do { \
    (stat_) = SREG; \
    cli(); \
} while (0)
#define QF_CRIT_EXIT(stat_)     (SREG = (stat_))

/*
#define QF_CRIT_ENTRY(UNUSED) __asm__ __volatile__ (     \
    "in __tmp_reg__, __SREG__"                    "\n\t" \
    "cli"                                         "\n\t" \
    "push __tmp_reg__"                            "\n\t" \
    ::: "memory"                                         \
    )

#define QF_CRIT_EXIT(UNUSED)  __asm__ __volatile__ (     \
    "pop __tmp_reg__"                             "\n\t" \
    "out __SREG__, __tmp_reg__"                   "\n\t" \
    ::: "memory"                                         \
    )
 */
// clang-format on

/* QF CPU reset for AVR */
#define QF_RESET() __asm__ __volatile__("jmp 0x0000" ::)

#include <stdint.h>  /* Exact-width types. WG14/N843 C99 Standard */
#include <stdbool.h> /* Boolean type.      WG14/N843 C99 Standard */
#include <string.h>  /* memset and string functions               */
#include <ctype.h>   /* tolower functions                         */

#include <avr/pgmspace.h>  /* accessing data in program memory (PROGMEM) */
#include <avr/io.h>        /* SREG/SMCR definitions */
#include <avr/interrupt.h> /* cli()/sei() */
#include <avr/eeprom.h>    /* AVR EEPROM support */

#include "qep_port.h" /* QEP port */
#include "qv_port.h"  /* QV cooperative kernel port */
#include "qf.h"       /* QF platform-independent public interface */

/*****************************************************************************
 * NOTE01:
 * The maximum number of active objects QF_MAX_ACTIVE can be increased
 * up to 64, if necessary. Here it is set to a lower level to save some RAM.
 */

#endif /* QF_PORT_H */
