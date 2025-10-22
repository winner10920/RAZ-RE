/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32g031_it.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include "n32g031.h"
#include "n32g0xx_it.h"
#include "n32g0xx_STLparam.h"
#include "n32g0xx_STLclassBvar.h"
#include "n32g0xx_STLlib.h"
#include <stdio.h>



/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSV_Handler exception.
 */
void PendSV_Handler(void)
{
}

/**
* @brief This function handles System tick timer.
 *1.get 2ms SysTick to test main check
 *2.test 
*/
//2ms
void SysTick_Handler(void)
{
    /* Verify TickCounter integrity */
    if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
    {
        TickCounter++;
        TickCounterInv = ~TickCounter;

        if (TickCounter >= SYSTICK_20ms_TB)
        {
          uint32_t RamTestResult;

          /* Reset timebase counter */
          TickCounter = 0u;
          TickCounterInv = 0xFFFFFFFF;

          /* Set Flag read in main loop */
          TimeBaseFlag = 0xAAAAAAAAuL;
          TimeBaseFlagInv = 0x55555555uL;

          if ((CurrentHSEPeriod ^ CurrentHSEPeriodInv) == 0xFFFFFFFFuL)
          {
            //ISRCtrlFlowCnt += MEASPERIOD_ISR_CALLER;
            LastHSEPeriod = CurrentHSEPeriod;
            CurrentHSEPeriod = RTC_GetSubSecond();
            LastHSEPeriodInv = ~LastHSEPeriod;
            CurrentHSEPeriodInv = ~CurrentHSEPeriod;
            //ISRCtrlFlowCntInv -= MEASPERIOD_ISR_CALLER;
          }
          else  /* Class B Error on CurrentHSEPeriod */
          {
              #ifdef STL_VERBOSE
                printf("\n\r Class B Error on CurrentHSEPeriod \n\r");
              #endif  /* STL_VERBOSE */
          }
          
      /*----------------------------------------------------------------------*/
      /*------------------     RAM test(in interrupt)  -----------------------*/
      /*----------------------------------------------------------------------*/
            ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
            __disable_irq();
            RamTestResult = STL_TranspMarch();
            __enable_irq();
            ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER; 
          switch ( RamTestResult )
          {
            case TEST_RUNNING:
              break;
            case TEST_OK:
              #ifdef STL_VERBOSE
               /* avoid any long string output here in the interrupt, '#' marks ram test completed ok */
                putchar((int)'#');      /* RAM OK mark */
              #endif  /* STL_VERBOSE */
              break;
            case TEST_FAILURE:
            case CLASS_B_DATA_FAIL:
            default:
              #ifdef STL_VERBOSE
                printf("\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
              #endif  /* STL_VERBOSE */
              FailSafePOR();
              break;
          } /* End of the switch */

          /* Do we reached the end of RAM test? */
          /* Verify 1st ISRCtrlFlowCnt integrity */
          if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
          {
            if (RamTestResult == TEST_OK)
            {
              if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
              {
                  #ifdef STL_VERBOSE
                    printf("\n\r Control Flow error (RAM test) \n\r");
                  #endif  /* STL_VERBOSE */
                  FailSafePOR();
              }
              else  /* Full RAM was scanned */
              {
                 ISRCtrlFlowCnt = 0u;
                 ISRCtrlFlowCntInv = 0xFFFFFFFFuL;
              }
            } /* End of RAM completed if*/
          } /* End of control flow monitoring */
          else
          {
              #ifdef STL_VERBOSE
                printf("\n\r Control Flow error in ISR \n\r");
              #endif  /* STL_VERBOSE */
              FailSafePOR();
          }         
       
        } /* End of the 20 ms timebase interrupt */
    }
    else  /* Class B variable error on TickCounter */
    {
        #ifdef STL_VERBOSE
        printf("\n\r Class B Error on TickCounter\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
    }
}


/**
* @brief This function handles DMA1 channel 1 interrupt.
*/
void DMA_Channel1_2_IRQnHandler(void)
{
  
}

/**
* @brief This function handles TIM1 break, update, trigger and commutation interrupts.
*/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  
}



/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
 
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C)  *****END OF FILE****/
