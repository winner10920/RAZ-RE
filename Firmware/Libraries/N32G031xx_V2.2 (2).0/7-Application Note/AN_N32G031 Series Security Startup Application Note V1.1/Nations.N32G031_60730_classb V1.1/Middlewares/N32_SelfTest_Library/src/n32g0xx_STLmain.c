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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ErrorStatus STL_CheckStack(void);

/**
  * @brief  Initializes the Class B variables and their inverted
  *   redundant counterparts. Init also the Systick and RTC timer
  *   for clock frequency monitoring.
  * @param :  None
  * @retval : None
  */
void STL_InitRunTimeChecks(void)
{
  /* Init Class B variables required in main routine and SysTick interrupt
  service routine for timing purposes */
    
  TickCounter = 0uL;
  TickCounterInv = 0xFFFFFFFFuL;

  TimeBaseFlag = 0uL;
  TimeBaseFlagInv = 0xFFFFFFFFuL;

  LastCtrlFlowCnt = 0uL;
  LastCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize variables for SysTick interrupt routine control flow monitoring */
  ISRCtrlFlowCnt = 0uL;
  ISRCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize variable for clock memory check */
  CurrentHSEPeriod = 0uL;
  CurrentHSEPeriodInv = 0xFFFFFFFFuL;
  LastHSEPeriod = RTC_GetSubSecond();
  LastHSEPeriodInv = ~LastHSEPeriod;

  /* Initialize SysTick for clock frequency measurement and main time base */
  STL_SysTickConfig();

  /* Initialize variables for run time invariable memory check */  
  STL_FlashCrc32Init();
  
  STL_TranspMarchInit();

  #if defined(USE_INDEPENDENT_WDOG) || defined(USE_WINDOW_WDOG)
    initialize_system_wdogs();
  #endif  /* USE_INDEPENDENT_WDOG | USE_WINDOW_WDOG */ 
       
  /* Initialize variables for main routine control flow monitoring */
  CtrlFlowCnt = 0uL;
  CtrlFlowCntInv = 0xFFFFFFFFuL;
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  Provide a short description of the function
  * @param :  None
  * @retval : None
  */
uint32_t debugtemp = 0;
void STL_DoRunTimeChecks(void)
{
  /* Is the time base duration elapsed? */
  if (TimeBaseFlag == 0xAAAAAAAAuL)
  {
        uint32_t TmpFlag = TimeBaseFlagInv;

    /* Verify its integrity (class B variable) */
    if ((TimeBaseFlag ^ TmpFlag) == 0xFFFFFFFFuL)
    {
      uint32_t RomTest;

      /* Reset Flag (no need to reset the redundant: it is not tested if
      TimeBaseFlag != 0xAAAAAAAA, it means that 100ms elapsed */
      TimeBaseFlag = 0uL;

      /*----------------------------------------------------------------------*/
      /*---------------------------- CPU registers ----------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += CPU_TEST_CALLER;
      if (STL_RunTimeCPUTest() != CPUTEST_SUCCESS)
      {
        #ifdef STL_VERBOSE
          printf("Run-time CPU Test Failure\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CtrlFlowCntInv -= CPU_TEST_CALLER;
      }

      /*----------------------------------------------------------------------*/
      /*------------------------- Stack overflow -----------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += STACK_OVERFLOW_TEST;
      if (STL_CheckStack() != SUCCESS)
      {
        #ifdef STL_VERBOSE
          printf("Stack overflow\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CtrlFlowCntInv -= STACK_OVERFLOW_TEST;
      }

      /*----------------------------------------------------------------------*/
      /*------------------------- Clock monitoring ---------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += CLOCK_TEST_CALLER;
      switch ( STL_MainClockTest() )
      {
        case FREQ_OK:
          CtrlFlowCntInv -= CLOCK_TEST_CALLER;
          break;

        case EXT_SOURCE_FAIL:
          #ifdef STL_VERBOSE
          /* Loop until the end of current transmission */
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXC) == RESET)
            {
            }
            /* Re-config USART baudrate FOR 115200 bds with HSI clock (8MHz) */
            USART_ReConfigurationClk();
            printf("\n\r Clock Source failure (Run-time)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;

        case CLASS_B_VAR_FAIL:
          #ifdef STL_VERBOSE
            printf("\n\r Class B variable error (clock test)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;

        case LSI_START_FAIL:
        case HSE_START_FAIL:
        case HSI_HSE_SWITCH_FAIL:
        case TEST_ONGOING:
        case CTRL_FLOW_ERROR:
        default:
          #ifdef STL_VERBOSE
            printf("Abnormal Clock Test routine termination \n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }

      /*----------------------------------------------------------------------*/
      /*------------------ Invariable memory CRC check -----------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += FLASH_TEST_CALLER;

      RomTest = STL_crc32Run();
      
      switch ( RomTest )
      {
        case TEST_RUNNING:
            CtrlFlowCntInv -= FLASH_TEST_CALLER;
          break;

        case TEST_OK:
          #ifdef STL_VERBOSE
            putchar((int)'*');      /* FLASH test OK mark */
          #endif  /* STL_VERBOSE */
          CtrlFlowCntInv -= FLASH_TEST_CALLER;
          break;

        case TEST_FAILURE:
        case CLASS_B_DATA_FAIL:
        default:
          #ifdef STL_VERBOSE
            printf("\n\r Run-time FLASH CRC Error\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }

      /*----------------------------------------------------------------------*/
      /*---------------- Check Safety routines Control flow  -----------------*/
      /*------------- Refresh Window and independent watchdogs ---------------*/
      /*----------------------------------------------------------------------*/
      /* Update WWDG counter */
      #ifdef USE_WINDOW_WDOG
        WWDG_SetCnt(0x7Fu);
      #endif  /* USE_WWDOG */
      
      /* Reload IWDG counter */
      #ifdef USE_INDEPENDENT_WDOG
        IWDG_ReloadKey();
      #endif  /* USE_INDEPENDENT_WDOG */
           
      if (((CtrlFlowCnt ^ CtrlFlowCntInv) == 0xFFFFFFFFuL)
        &&((LastCtrlFlowCnt ^ LastCtrlFlowCntInv) == 0xFFFFFFFFuL))
      {
        if (RomTest == TEST_OK)
        {
          debugtemp = ROM_SIZE;
	  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  /* ==============================================================================*/
  /* MISRA violation of rule 17.4 - pointer arithmetic is used for Control flow calculation */
	    #pragma diag_suppress=Pm088
	  #endif   /* IAR Compiler */
          if ((CtrlFlowCnt == FULL_FLASH_CHECKED)\
          && ((CtrlFlowCnt - LastCtrlFlowCnt) == (LAST_DELTA_MAIN)))
	  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
	    #pragma diag_default=Pm088
  /* ==============================================================================*/
	  #endif   /* IAR Compiler */
          {
            CtrlFlowCnt = 0uL;
            CtrlFlowCntInv = 0xFFFFFFFFuL;
          }
          else  /* Return value form crc check was corrupted */
          {
            #ifdef STL_VERBOSE
              printf("Control Flow Error (main loop, Flash CRC)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }
        else  /* Flash test not completed yet */
        {
          if ((CtrlFlowCnt - LastCtrlFlowCnt) != DELTA_MAIN)
          {
            #ifdef STL_VERBOSE
              printf("Control Flow Error (main loop, Flash CRC on-going)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }

        LastCtrlFlowCnt = CtrlFlowCnt;
        LastCtrlFlowCntInv = CtrlFlowCntInv;
      }
      else
      {
        #ifdef STL_VERBOSE
          printf("Control Flow Error (main loop)\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
      }
          
    } /* End of periodic Self-test routine */
    else  /* Class B variable error (can be Systick interrupt lost) */
    {
      #ifdef STL_VERBOSE
        printf("\n\r Class B variable error (clock test)\n\r");
      #endif  /* STL_VERBOSE */
      FailSafePOR();
    }
  } /* End of periodic Self-test routine */
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies the frequency using the measurement
  *   done in Systick interrupt.
  * @param :  None
  * @retval : ClockStatus = (LSI_START_FAIL, HSE_START_FAIL,
  *   HSI_HSE_SWITCH_FAIL, TEST_ONGOING, EXT_SOURCE_FAIL,
  *   CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK)
  */
ClockStatus STL_MainClockTest(void)
{
  uint32_t  DifPeriod;
  ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCK_TEST_CALLEE;

  if (((CurrentHSEPeriod ^ CurrentHSEPeriodInv) == 0xFFFFFFFFuL)\
     &&  ((LastHSEPeriod ^ LastHSEPeriodInv) == 0xFFFFFFFFuL))
  {
    if(LastHSEPeriod == 0uL)
    {
      /* first measurement of HSE period must be ignored as there is nothing to compare */
      Result = FREQ_OK;
      /* to keep consistency in control flow as STL_CheckPeriod() is not called at this step */
      CtrlFlowCnt += CLOCKPERIOD_TEST_CALLEE;
      CtrlFlowCntInv -= CLOCKPERIOD_TEST_CALLEE;
    }
    else
    {
      /* find difference between last two RTC subsecond measurements */
      if(LastHSEPeriod > CurrentHSEPeriod)
      {
        DifPeriod= LastHSEPeriod - CurrentHSEPeriod;
      }
      else
      {
      /* case the RTC subsecond register was just reloaded by value from PRER reg (set to a maximum 0x7FFF) */
        DifPeriod= LastHSEPeriod + 0x7FFFu - CurrentHSEPeriod;
      }
      if (STL_CheckPeriod(DifPeriod / SYSTICK_20ms_TB) == FREQ_OK)
      {
        Result = FREQ_OK;
      }
      else
      {
        Result = EXT_SOURCE_FAIL;
      }
    }
  }
  else  /* Inverse redundant variables failure */
  {
    Result = CLASS_B_VAR_FAIL;
  }

  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies that Stack didn't overflow
  * @param :  None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
ErrorStatus STL_CheckStack(void)
{
    ErrorStatus Result = ERROR;

  CtrlFlowCnt += STACK_OVERFLOW_CALLEE;

  if (aStackOverFlowPtrn[0] != 0xAAAAAAAAuL)
  {
    Result = ERROR;
  }
  else /* aStackOverFlowPtrn[0] == 0xAAAAAAAA */
  {
    if (aStackOverFlowPtrn[1] != 0xBBBBBBBBuL)
    {
      Result = ERROR;
    }
    else /* aStackOverFlowPtrn[1] == 0xBBBBBBBB */
    {
      if (aStackOverFlowPtrn[2] != 0xCCCCCCCCuL)
      {
        Result = ERROR;
      }
      else /* aStackOverFlowPtrn[2] == 0xCCCCCCCC */
      {
        if (aStackOverFlowPtrn[3] != 0xDDDDDDDDuL)
        {
          Result = ERROR;
        }
        else
        {
          Result = SUCCESS;
        }
      }
    }
  }

  CtrlFlowCntInv -= STACK_OVERFLOW_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function initialize both independent & window system watch dogs
  * @param :  None
  * @retval None
  */
void initialize_system_wdogs(void)
{
  #ifdef USE_INDEPENDENT_WDOG
    /* Ebnable LSI clcok to feed IWDG & wait until clock are ready */
    RCC_EnableLsi(ENABLE);
    while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSIRD)!= SET)
    { }
       
      /* Enable write access to IWDG_PR and IWDG_RLR registers */
      IWDG_WriteConfig(IWDG_WRITE_ENABLE);
      /* IWDG clock: 30-60KHz(LSI) / 32 = ~1.8 - 0,94KHz */
      IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32);
      /* Set counter reload value to 39 (~21-42ms) */
      IWDG_CntReload(39u);
      /* Reload IWDG counter */
      IWDG_ReloadKey();
      /* Enable IWDG */
      IWDG_Enable();
  #endif  /* USE_INDEPENDENT_WDOG */
      
  #ifdef USE_WINDOW_WDOG
    /* enable clock for WWDG */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_WWDG, ENABLE);
    /* set prescaler - at PCLK 48MHz it will count down from 0x7f to ~0x44 at 20 ms refresh */
    WWDG_SetPrescalerDiv(WWDG_PRESCALER_DIV4);
    /* set window value above 0x44 with some reserve */
    WWDG_SetWValue(0x48u);
    WWDG_Enable(0x7Fu);
  #endif  /* USE_WINDOW_WDOG */
}

/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
