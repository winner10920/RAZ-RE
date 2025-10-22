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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "n32g0xx_STLparam.h"
#include "n32g0xx_STLlib.h"

/** @addtogroup N32G0xxSelfTestLib_src
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CTRL_TICKINT_Set      ((uint32_t)0x00000002)
#define CTRL_TICKINT_Reset    ((uint32_t)0xFFFFFFFD)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the SysTick for clock frequency measurement and
  *   main time base. The RTC is reset and synchronized with the
  *   SysTick to have a direct comparison with reference period
  *   (RTC counter is read during each SysTick interrupt).
  * @param :  None
  *   None
  * @retval : None
  */
void STL_SysTickConfig(void)
{
  /* Initializes the Systick for run-time clock measurement */
  SysTick->CTRL &= SysTick_Counter_Disable; /* Stop SysTick timer */
  SysTick->VAL = SysTick_Counter_Clear;    /* Reset counter */
  /* Set reload rate. Corresponds to reference period for clock measurement */
  SysTick->LOAD = SYSTICK_TB_RUN;
  /* Enable the SysTick Interrupt */
  SysTick->CTRL |= CTRL_TICKINT_Set;
  /* Start down-counting */ 
  SysTick->CTRL |= SysTick_Counter_Enable;
  
    /*Configure the SysTick IRQ priority */
    NVIC_SetPriority(SysTick_IRQn, TICK_INT_PRIORITY);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies the frequency from the last clock
  *   period measurement
  * @param Period (uint32_t), with the same unit as CurrentHSEPeriod (could
  *   be an average for instance)
  * @retval : ClockStatus = (LSI_START_FAIL, HSE_START_FAIL,
  *   HSI_HSE_SWITCH_FAIL, TEST_ONGOING, EXT_SOURCE_FAIL,
  *   CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK)
  */
ClockStatus STL_CheckPeriod(uint32_t Period)
{
    uint32_t ClockFrequency;
    ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCKPERIOD_TEST_CALLEE;
  /* First measure as reference for run-time check */
  if ((StartUpClockFreq ^ StartUpClockFreqInv) == 0xFFFFFFFFuL)
  {
    ClockFrequency = StartUpClockFreq;

    if (Period != 0u)  /* Test for divide by zero */
    {
      ClockFrequency /= Period;
    }
    else  /* Set frequency to an unrealistic high value */
    {
      ClockFrequency = U32_MAX;
    }

    if (ClockFrequency < HSE_LimitLow)
    {
      /* Switch back to internal clock */
      RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);
      Result = EXT_SOURCE_FAIL;	/* Sub-harmonics: HSE -25% below expected */
    }
    else
    {
      if (ClockFrequency > HSE_LimitHigh)
      {
        /* Switch back to internal clock */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);
        Result = EXT_SOURCE_FAIL;	/* Harmonics: HSE +25% above expected */
      }
      else
      {
        Result = FREQ_OK;         /* Crystal or Resonator started correctly */
      } /* No harmonics */
    } /* No sub-harmonics */
  } /* Control Flow error */
  else
  {
    Result = CLASS_B_VAR_FAIL;
  }

  CtrlFlowCntInv -= CLOCKPERIOD_TEST_CALLEE;

  return (Result);
}


/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
