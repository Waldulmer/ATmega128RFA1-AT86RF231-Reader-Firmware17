/* Copyright (c) 2009  ATMEL Corporation
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, data, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/
/** \file
 *  \brief Header file for timer_enddevice.c.
 */
#ifndef TIMER_ENDDEVICE_H
#define TIMER_ENDDEVICE_H

#include "mac.h"

#ifndef COORDNODE
//#if (NODETYPE == ENDDEVICE)

#include "config.h"

#if (PLATFORM == RCBRFA1)
/** \brief Definition of macro to check Output Compare Match A Interrupt flag for AT90USB1287.
 */
 #define TIMER2_OCA_TEST  (TIFR2 & (1 << OCF2A))
/** \brief Definition of macro to reset Timer/Counter register for AT90USB1287.
 */
 #define TIMER2_RESET  TCNT2 = 0
#endif // (PLATFORM == RCBRFA1)

void Timer2_set ( uchar ucTimerFlagsA, uchar ucTimerFlagsB, uchar ucTimerCmp );
void Timer2_stop ( void );


//#ifdef CMC

extern volatile uchar GenericTimerFlag;
extern volatile uchar WDTFlag;

void GenericTimer_Init ( void );
void GenericTimer_Stop ( void );
void Timer_delay_us ( u16 uiDelay );
void Timer_delay_ms ( u16 uiDelay );
void WDT_Off ( void );
void WDT_Start ( void );

#if (PLATFORM == RCBRFA1)
#define wdt_intr_enable(value)   \
    __asm__ __volatile__ (  \
        "in __tmp_reg__,__SREG__" "\n\t"    \
        "cli" "\n\t"    \
        "wdr" "\n\t"    \
        "sts %0,%1" "\n\t"  \
        "out __SREG__,__tmp_reg__" "\n\t"   \
        "sts %0,%2" \
        : /* no outputs */  \
        : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
        "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
        "r" ((u8) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | \
            _BV(WDIE) | (value & 0x07)) ) \
        : "r0"  \
    )
#endif

//#endif // CMC

#endif // (NODETYPE == ENDDEVICE)

#endif // TIMER_ENDDEVICE_H
