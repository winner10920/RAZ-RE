  .syntax unified
  .cpu cortex-m0
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call the clock system initialization function.*/
  bl  SystemInit
/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl  main

LoopForever:
  b LoopForever
  .size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M0.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  0
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler
  .word  WWDG_IRQHandler
  .word  PVD_IRQHandler
  .word  RTC_IRQHandler
  .word  MMU_IRQHandler
  .word  FLASH_IRQHandler
  .word  RCC_IRQHandler
  .word  EXTI0_1_IRQHandler
  .word  EXTI2_3_IRQHandler
  .word  EXTI4_15_IRQHandler
  .word  RESERVE9_IRQHandler
  .word  DMA_Channel1_2_IRQHandler
  .word  DMA_Channel1_2_3_4_IRQHandler
  .word  DMA_Channel5_IRQHandler
  .word  TIM1_BRK_UP_TRG_COM_IRQHandler
  .word  TIM1_CC_IRQHandler
  .word  DMA_Channel3_4_IRQHandler
  .word  TIM3_IRQHandler
  .word  USART2_IRQHandler
  .word  TIM8_BRK_UP_TRG_COM_IRQHandler
  .word  TIM8_CC_IRQHandler
  .word  LPTIM_TIM6_IRQHandler
  .word  ADC_IRQHandler
  .word  SPI2_IRQHandler
  .word  I2C1_IRQHandler
  .word  I2C2_IRQHandler
  .word  SPI1_IRQHandler
  .word  HDIV_SQRT_IRQHandler
  .word  RAMC_PERR_IRQHandler
  .word  USART1_2_IRQHandler
  .word  LPUART_IRQHandler
  .word  USART1_IRQHandler
  .word  COMP_IRQHandler

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak  NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak  HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak  SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak  PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak  SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak  WWDG_IRQHandler
  .thumb_set WWDG_IRQHandler,Default_Handler

  .weak  PVD_IRQHandler
  .thumb_set PVD_IRQHandler,Default_Handler

  .weak  RTC_IRQHandler
  .thumb_set RTC_IRQHandler,Default_Handler

  .weak  MMU_IRQHandler
  .thumb_set MMU_IRQHandler,Default_Handler

  .weak  FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler

  .weak  RCC_IRQHandler
  .thumb_set RCC_IRQHandler,Default_Handler

  .weak  EXTI0_1_IRQHandler
  .thumb_set EXTI0_1_IRQHandler,Default_Handler

  .weak  EXTI2_3_IRQHandler
  .thumb_set EXTI2_3_IRQHandler,Default_Handler

  .weak  EXTI4_15_IRQHandler
  .thumb_set EXTI4_15_IRQHandler,Default_Handler

  .weak  RESERVE9_IRQHandler
  .thumb_set RESERVE9_IRQHandler,Default_Handler

  .weak  DMA_Channel1_2_IRQHandler
  .thumb_set DMA_Channel1_2_IRQHandler,Default_Handler

  .weak  DMA_Channel1_2_3_4_IRQHandler
  .thumb_set DMA_Channel1_2_3_4_IRQHandler,Default_Handler

  .weak  DMA_Channel5_IRQHandler
  .thumb_set DMA_Channel5_IRQHandler,Default_Handler

  .weak  TIM1_BRK_UP_TRG_COM_IRQHandler
  .thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler

  .weak  TIM1_CC_IRQHandler
  .thumb_set TIM1_CC_IRQHandler,Default_Handler

  .weak  DMA_Channel3_4_IRQHandler
  .thumb_set DMA_Channel3_4_IRQHandler,Default_Handler

  .weak  TIM3_IRQHandler
  .thumb_set TIM3_IRQHandler,Default_Handler

  .weak  USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak  TIM8_BRK_UP_TRG_COM_IRQHandler
  .thumb_set TIM8_BRK_UP_TRG_COM_IRQHandler,Default_Handler

  .weak  TIM8_CC_IRQHandler
  .thumb_set TIM8_CC_IRQHandler,Default_Handler

  .weak  LPTIM_TIM6_IRQHandler
  .thumb_set LPTIM_TIM6_IRQHandler,Default_Handler

  .weak  ADC_IRQHandler
  .thumb_set ADC_IRQHandler,Default_Handler

  .weak  SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak  I2C1_IRQHandler
  .thumb_set I2C1_IRQHandler,Default_Handler

  .weak  I2C2_IRQHandler
  .thumb_set I2C2_IRQHandler,Default_Handler

  .weak  SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak  HDIV_SQRT_IRQHandler
  .thumb_set HDIV_SQRT_IRQHandler,Default_Handler

  .weak  RAMC_PERR_IRQHandler
  .thumb_set RAMC_PERR_IRQHandler,Default_Handler

  .weak  USART1_2_IRQHandler
  .thumb_set USART1_2_IRQHandler,Default_Handler

  .weak  LPUART_IRQHandler
  .thumb_set LPUART_IRQHandler,Default_Handler

  .weak  USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak  COMP_IRQHandler
  .thumb_set COMP_IRQHandler,Default_Handler
