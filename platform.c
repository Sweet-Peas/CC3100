/*
 * board.c - msp430fr5739 experiment board configuration
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
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

#include "platform.h"


/****************************************************************************
                  Interrupt Handlers
****************************************************************************/
P_EVENT_HANDLER        pIrqEventHandler = 0;

unsigned long count = 0;

void initClk()
{ 
    CSCTL0_H = 0xA5;
    CSCTL1 = DCORSEL + DCOFSEL0 + DCOFSEL1; /* Set max. DCO setting */
    CSCTL2 = SELA_1 + SELS_3 + SELM_3; /*set ACLK - VLO, the rest = MCLK = DCO*/
    CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0;      /* set all dividers to 0 */

    __bis_SR_register(GIE);
  
}

void stopWDT()
{
    WDTCTL = WDTPW + WDTHOLD;
}

int registerInterruptHandler(P_EVENT_HANDLER InterruptHdl , void* pValue)
{
    pIrqEventHandler = InterruptHdl;

    return 0;
}

void CC3100_disable()
{
    P1OUT &= ~BIT2;
}

void CC3100_enable()
{
    P1OUT |= BIT2;
}

void CC3100_InterruptEnable()
{
    P2IES &= ~BIT3;
    P2IE |= BIT3;
}

void CC3100_InterruptDisable()
{
    P2IE &= ~BIT3;
}

void Delay(unsigned long interval)
{
    while(interval > 0)
    {
        __delay_cycles(24000);
        interval--;
    }
}

void initLEDs()
{
    PJOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3);
    P3OUT &= ~(BIT4 + BIT5 + BIT6 + BIT7);
    
    PJDIR |= (BIT0 + BIT1 + BIT2 + BIT3);
    P3DIR |= (BIT4 + BIT5 + BIT6 + BIT7);
}

void turnLedOn(char ledNum)
{
    switch(ledNum)
    {
      case LED1:
        PJOUT |= (BIT0); 
        break;
      case LED2:
        PJOUT |= (BIT1); 
        break;
      case LED3:
        PJOUT |= (BIT2); 
        break;
      case LED4:
        PJOUT |= (BIT3);
        break;
      case LED5:
        P3OUT |= (BIT4);
        break;
      case LED6:
        P3OUT |= (BIT5);
        break;
      case LED7:
        P3OUT |= (BIT6);
        break;
      case LED8:
        P3OUT |= (BIT7);
        break;
    }
}

void turnLedOff(char ledNum)
{                     
    switch(ledNum)
    {
      case LED1:
        PJOUT &= ~(BIT0); 
        break;
      case LED2:
        PJOUT &= ~(BIT1); 
        break;
      case LED3:
        PJOUT &= ~(BIT2); 
        break;
      case LED4:
        PJOUT &= ~(BIT3);
        break;
      case LED5:
        P3OUT &= ~(BIT4);
        break;
      case LED6:
        P3OUT &= ~(BIT5);
        break;
      case LED7:
        P3OUT &= ~(BIT6);
        break;
      case LED8:
        P3OUT &= ~(BIT7);
        break;
    }
}

void toggleLed(char ledNum)
{
    switch(ledNum)
    {
      case LED1:
        PJOUT ^= (BIT0); 
        break;
      case LED2:
        PJOUT ^= (BIT1); 
        break;
      case LED3:
        PJOUT ^= (BIT2); 
        break;
      case LED4:
        PJOUT ^= (BIT3);
        break;
      case LED5:
        P3OUT ^= (BIT4);
        break;
      case LED6:
        P3OUT ^= (BIT5);
        break;
      case LED7:
        P3OUT ^= (BIT6);
        break;
      case LED8:
        P3OUT ^= (BIT7);
        break;
    }
}

void restartMSP430()
{
   
    PMMCTL0 |= PMMSWPOR;
        
    /*This function will never exit since it forces a complete
    * restart of the MSP430. */
}

/*!
    \brief The IntGPIOHandler interrupt handler

    \param[in]  none

    \return     none

    \note

    \warning
*/
#pragma vector=PORT2_VECTOR
__interrupt void IntGPIOHandler(void)
{
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
}

/* Catch interrupt vectors that are not initialized. */
#ifdef __CCS__
#pragma vector= USCI_A1_VECTOR,PORT4_VECTOR, TIMER1_A0_VECTOR, PORT1_VECTOR, \
	WDT_VECTOR, TIMER1_A1_VECTOR, ADC10_VECTOR, UNMI_VECTOR,COMP_D_VECTOR, \
    DMA_VECTOR, PORT3_VECTOR, RTC_VECTOR, TIMER0_B1_VECTOR, TIMER1_B0_VECTOR, \
	TIMER1_B1_VECTOR, TIMER2_B0_VECTOR, TIMER2_B1_VECTOR,SYSNMI_VECTOR, \
	USCI_B0_VECTOR, TIMER0_A1_VECTOR, TIMER0_A0_VECTOR, TIMER0_B0_VECTOR
__interrupt void Trap_ISR(void)
{
    while(1);
}

#endif
