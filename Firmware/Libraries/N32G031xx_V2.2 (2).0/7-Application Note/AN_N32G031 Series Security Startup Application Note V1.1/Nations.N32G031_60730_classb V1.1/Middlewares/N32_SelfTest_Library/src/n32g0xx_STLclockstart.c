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
#include "n32g0xx_STLlib.h"


/** @addtogroup N32G0xxSelfTestLib_src
  * @{
  */ 


#define STL_SyncSysTick() { while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0uL) {} }
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static ErrorStatus STL_LSIinit(void);
static ErrorStatus STL_HSE_CSSinit(void);
static ErrorStatus STL_SwitchToExtClockSrc(void);
void STL_RTCinit(void);
void STL_SysTickInit(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Start up the internal and external oscillators and verifies
  *   that clock source is within the expected range
  * @param :  None
  * @retval : ClockStatus = (LSI_START_FAIL; HSE_START_FAIL;
  *   HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
  *   CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK)
  */
ClockStatus STL_ClockStartUpTest(void)
{
  uint32_t LSIPeriod, ClockFrequency;
  ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCK_TEST_CALLEE;

  /* Start low speed internal (LSI) oscillator */
  if (STL_LSIinit() != SUCCESS)
  {
    Result = LSI_START_FAIL;
  }
  else  /* LSI started correctly */
  {
    /* Start High-speed external oscillator (HSE) and Clock Security System (CSS) */
    if (STL_HSE_CSSinit() != SUCCESS)
    {
      Result = HSE_START_FAIL;
    }
    else  /* HSE started correctly */
    {
      STL_RTCinit();      /* Start Real Time Clock to measure LSI period */
      STL_SysTickInit();  /* Start SysTick timer to measure HSI then HSE */

      /*-------------- Reference LSI Measurement -----------------------------*/
      STL_SyncSysTick()          // waiting SysTick cnt to 0
      LSIPeriod = RTC_GetSubSecond();       /* Read initial value RTC */
      STL_SyncSysTick()          // waiting SysTick cnt to 0
      LSIPeriod -= RTC_GetSubSecond();       /* read diference */

      /* Switch CPU clock source from internal RC to oscillator and check it */
      if (STL_SwitchToExtClockSrc() != SUCCESS)
      {
        Result = HSI_HSE_SWITCH_FAIL;
      }
      else    /* Clock switched correctly */
      {
        /*-------------------- HSE Measurement -------------------------------*/        
        STL_SyncSysTick()
        RefHSEPeriod= RTC_GetSubSecond();   /* Read initial value RTC */
        STL_SyncSysTick()
        RefHSEPeriod -= RTC_GetSubSecond();   /* read diference */
        RefHSEPeriodInv = ~RefHSEPeriod;   /* Redundant storage */

        ClockFrequency = HSI_Freq * LSIPeriod;

        /* Store first measure as reference for run-time check */
        StartUpClockFreq = ClockFrequency;
        StartUpClockFreqInv = ~ClockFrequency;  /* Redundant storage */

        if (RefHSEPeriod != 0uL)  /* Test for divide by zero */
        {
            uint32_t TmpHSEPEriod = RefHSEPeriod;  /* Added for MISRA compliance */
            ClockFrequency /= TmpHSEPEriod;
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
            /* Clock OK means < 16MHz (external osc max frequency), therefore
            no wait state are necessary */
            FLASH_SetLatency(FLASH_LATENCY_0);
            Result = FREQ_OK;         /* Crystal or Resonator started correctly */
          } /* No harmonics */
        } /* No sub-harmonics */
      } /* Clock switched correctly */
    } /* HSE started correctly */
  } /* LSI started correctly */

  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;

  return(Result);
}

/* ---------------------------------------------------------------------------------- */
/**
  * @brief  Clock the APB domain 1, reset the circuitry of the battery
  *   backuped domain and start-up the low speed internal
  *   RC oscillator (30 < F < 60 kHz)
  * @param :  None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
static ErrorStatus STL_LSIinit(void)
{
    ErrorStatus Result = ERROR;
    uint32_t TimeOut = LSI_START_TIMEOUT;

  CtrlFlowCnt += LSI_INIT_CALLEE;

  /* RTC clock selection -----------------------------------------------------*/
  RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  
  /* Enable LSI */
  RCC_EnableLsi(ENABLE);

  /* Wait till LSI is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSIRD) == RESET) && (TimeOut != 0uL));

  if (TimeOut == 0uL)
  {
    Result = ERROR;     /* Internal low speed oscillator failure */
  }
  else
  {
    Result = SUCCESS;
  }

  CtrlFlowCntInv -= LSI_INIT_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------------- */
/**
  * @brief  Start-up the high speed external oscillator (based on crystal
  *   or resonator), enable the clock security system
  * @param :  None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
static ErrorStatus STL_HSE_CSSinit(void)
{
    ErrorStatus Result = ERROR;
    uint32_t TimeOut = HSE_START_TIMEOUT;

  CtrlFlowCnt += HSE_INIT_CALLEE;

  /* Start-up the oscillator (HSE: High-speed External) */
  RCC_ConfigHse(RCC_HSE_ENABLE);

  /* Wait till HSE is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_GetFlagStatus(RCC_CTRL_FLAG_HSERDF) == RESET) && (TimeOut != 0uL));

  if (TimeOut == 0uL)
  {
    Result = ERROR;     /* Internal low speed oscillator failure */
  }
  else
  {
    /* Enable the Clock Security System (CSS): any failure on HSE will cause
       a Non Maskable Interrupt and switch back to internal oscillator */
    /* At this time the CPU clock source is still the internal oscillator */
    RCC_EnableClockSecuritySystem(ENABLE);
    Result = SUCCESS;
  }
  CtrlFlowCntInv -= HSE_INIT_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------------- */
/**
  * @brief  Start-up the Real Time clock
  * @param :  None
  * @retval : None
  */
void STL_RTCinit(void)
{
  RTC_InitType RTC_InitStruct;

  CtrlFlowCnt += RTC_INIT_CALLEE;

  RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI); /* Select LSI as RTC Clock Source */

  RCC_EnableRtcClk(ENABLE);                  /* Start RTC counter */

 /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Do not prescale to have the highest precision */
  RTC_InitStruct.RTC_HourFormat=  RTC_24HOUR_FORMAT;
  RTC_InitStruct.RTC_AsynchPrediv= 0uL;
  RTC_InitStruct.RTC_SynchPrediv= 0x7FFFuL;
  RTC_Init(&RTC_InitStruct);

  CtrlFlowCntInv -= RTC_INIT_CALLEE;
}

/* ---------------------------------------------------------------------------------- */
/**
  * @brief  Initializes and starts the SysTick timer to have the
  *   Start-up measurement time base
  * @param :  None
  * @retval : None
  */
void STL_SysTickInit(void)
{
  CtrlFlowCnt += SYSTICK_INIT_CALLEE;

  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick->LOAD = SYSTICK_TB_START;          /* Set reload rate (Ref period) */
  SysTick->VAL = SysTick_Counter_Clear;   /* Reset counter */
  SysTick->CTRL |= SysTick_Counter_Enable;  /* Start down-counting */
 
  CtrlFlowCntInv -= SYSTICK_INIT_CALLEE;
}

/* ---------------------------------------------------------------------------------- */
/**
  * @brief  Switch the main clock from internal RC oscillator to external
  *   oscillator.
  * @param :  None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
static ErrorStatus STL_SwitchToExtClockSrc(void)
{
    ErrorStatus Result = ERROR;
    uint32_t TimeOut = CLOCK_SWITCH_TIMEOUT;

  CtrlFlowCnt += CLOCK_SWITCH_CALLEE;

  /* Set 1 wait state for Flash Access, so that core can fetch instruction even
  if  HSE frequency exceeds 24MHz */
  FLASH_SetLatency(FLASH_LATENCY_1);

  /* Switch the main clock to HSE and verify afterwards */
  RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSE);
  do
  {
    TimeOut--;
  }
  while ((RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_HSE) && (TimeOut != 0uL));

  if (TimeOut == 0uL)
  {
    Result = ERROR;     /* Clock switch failure */
  }
  else
  {
    Result = SUCCESS;
  }

  CtrlFlowCntInv -= CLOCK_SWITCH_CALLEE;

  return (Result);
}
/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
