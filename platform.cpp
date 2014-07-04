/*
 * platform.c - Arduino based configuration
 *
 * Copyright (C) 2014 Electronic Sweet Peas
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <Arduino.h>
#include "platform.hpp"
#include "utility/simplelink.h"

UINT8 gcs_pin = 0xff;
UINT8 gen_pin = 0xff;
UINT8 girq_pin = 0xff;
UINT8 girq_num = 0xff;

void CC3000InterruptHandler(void);

/****************************************************************************
                  Interrupt Handlers
****************************************************************************/

void CC3100_disable()
{
    digitalWrite(gen_pin, HIGH);
}

void CC3100_enable()
{
    digitalWrite(gen_pin, LOW);
}

void CC3100_InterruptEnable()
{
  if (girq_num != 0xff)
    attachInterrupt(girq_num, CC3000InterruptHandler, RISING);
}

void CC3100_InterruptDisable()
{
  if (girq_num != 0xff)
    detachInterrupt(girq_num);
}

/*
 * Initialize system pins
 */
void CC3100_init_hw(char cs_pin, char en_pin, char irq_pin, char irq_num)
{
  gcs_pin = cs_pin;
  gen_pin = en_pin;
  girq_pin = irq_pin;
  girq_num = irq_num;
}


/*!
    \brief The IntGPIOHandler interrupt handler

    \param[in]  none

    \return     none

    \note

    \warning
*/
void CC3000InterruptHandler(void)
{
#if 0
    switch(__even_in_range(P2IV,P2IV_P2IFG3))
    {
      case P2IV_P2IFG3:
        if (pIrqEventHandler)
        {
            pIrqEventHandler(0);
        }
        break;
    default:
        break;
    }
#endif
}
