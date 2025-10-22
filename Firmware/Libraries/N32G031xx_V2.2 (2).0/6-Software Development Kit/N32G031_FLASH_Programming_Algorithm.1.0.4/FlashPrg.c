/* -----------------------------------------------------------------------------
 * Copyright (c) 2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty. 
 * In no event will the authors be held liable for any damages arising from 
 * the use of this software. Permission is granted to anyone to use this 
 * software for any purpose, including commercial applications, and to alter 
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not 
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be 
 *    appreciated but is not required. 
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be 
 *    misrepresented as being the original software. 
 * 
 * 3. This notice may not be removed or altered from any source distribution.
 *   
 *
 * $Date:        15. April 2014
 * $Revision:    V1.00
 *  
 * Project:      Flash Programming Functions
 * --------------------------------------------------------------------------- */

/* History:
 *  Version 1.00
 *    Initial release
 */ 

#include "FlashOS.H"        // FlashOS Structures

typedef volatile unsigned char  vu8;
typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;

#define M8(adr)  (*((vu8  *) (adr)))
#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))

// Peripheral Memory Map
#define IWDG_BASE       0x40003000
#define FLASH_BASE      0x40022000
#define AFEC_BASE       0x40001800

#define IWDG            ((IWDG_TypeDef *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)
#define AFEC            ((AFEC_TypeDef*) AFEC_BASE)

// FLASH BANK size
#define BANK1_SIZE      0x00010000      // Bank1 Size = 64kB *

// Independent WATCHDOG
typedef struct {
  vu32 KR;
  vu32 PR;
  vu32 RLR;
  vu32 SR;
} IWDG_TypeDef;

// Flash Registers
typedef struct {
  vu32 ACR;                                     // offset  0x000
  vu32 KEYR;                                    // offset  0x004
  vu32 OPTKEYR;                                 // offset  0x008
  vu32 SR;                                      // offset  0x00C
  vu32 CR;                                      // offset  0x010
  vu32 AR;                                      // offset  0x014
  vu32 RESERVED0[1];
  vu32 OBR;                                     // offset  0x01C
  vu32 WRPR;                                    // offset  0x020
  vu32 RESERVED1[4];                            // offset  0x024~0x030
  vu32 FLAG;                                    // offset  0x034
} FLASH_TypeDef;

// AFEC Registers
typedef struct {
  vu32 TRIMR0;                                  // offset  0x000
  vu32 TRIMR1;                                  // offset  0x004
} AFEC_TypeDef;


// Flash Keys
#define RDPRT_KEY       0x5AA5
#define FLASH_KEY1      0x45670123
#define FLASH_KEY2      0xCDEF89AB

// Flash Control Register definitions
#define FLASH_PG        0x00000001
#define FLASH_PER       0x00000002

#define CR_PER_Reset    0x00001FFD

#define FLASH_MER       0x00000004
#define FLASH_OPTPG     0x00000010
#define FLASH_OPTER     0x00000020
#define FLASH_STRT      0x00000040
#define FLASH_LOCK      0x00000080
#define FLASH_OPTWRE    0x00000100

// Flash Status Register definitions
#define FLASH_BSY       0x00000001
#define FLASH_PGERR     0x00000004
#define FLASH_WRPRTERR  0x00000010
#define FLASH_EOP       0x00000020

#define FLASH_SR_CLR    0x000000B4

#define FLASH_FLAG      0xF7FFF7FF

unsigned long base_adr;
unsigned long flash_flag = 0;

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase ²Á³ý, 2 - Program  ³ÌÐò, 3 - Verify Ð£Ñé)
 *    Return Value:   0 - OK,  1 - Failed
 */

#ifdef FLASH_MEM
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  base_adr = adr & ~(BANK1_SIZE - 1);          // Align to Size Boundary

  // Zero Wait State
  FLASH->ACR  = 0x00000000;

  // Unlock Flash    
  if((FLASH->CR & FLASH_LOCK) != 0) {
    FLASH->KEYR  = FLASH_KEY1;
    FLASH->KEYR  = FLASH_KEY2;
  }

  // Test if IWDG is running (IWDG in HW mode)
  if ((FLASH->OBR & 0x04) == 0x00) {
    // Set IWDG time out to ~32.768 second
    IWDG->KR  = 0x5555;                         // Enable write access to IWDG_PR and IWDG_RLR     
    IWDG->PR  = 0x06;                           // Set prescaler to 256  
    IWDG->RLR = 4095;                           // Set reload value to 4095
  }
  
  if(FLASH->FLAG == FLASH_FLAG)
  {
    flash_flag = 1;
  }

  return (0);
}
#endif

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

#ifdef FLASH_MEM
int UnInit (unsigned long fnc) {

  // Lock Flash
  FLASH->CR  |=  FLASH_LOCK;

  return (0);
}
#endif

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

#ifdef FLASH_MEM
int EraseChip (void) {

  if((FLASH->CR & FLASH_PG) != 0)
  {
    FLASH->CR &= ~FLASH_PG;
  }
  FLASH->SR  |=  FLASH_SR_CLR;

  FLASH->CR  |=  FLASH_MER;                     // Mass Erase Enabled
  FLASH->CR  |=  FLASH_STRT;                    // Start Erase

  while (FLASH->SR  & FLASH_BSY) {
    IWDG->KR = 0xAAAA;                          // Reload IWDG
  }

  FLASH->CR  &= ~FLASH_MER;                     // Mass Erase Disabled
  
  if((FLASH->SR & FLASH_EOP) == 0)
  {
    return (1);
  }

  return (0);                                   // Done
}
#endif

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

#ifdef FLASH_MEM
int EraseSector (unsigned long adr) {

  vu32 flash_vol = 0, hsi_trim = 0, tmp_reg1 = 0, tmp_reg2 = 0, tmp_reg3 = 0;
  
  if(flash_flag == 1)
  {
      if((adr == 0x08000000) || (adr == 0x00000000))
      {
        tmp_reg1 = AFEC->TRIMR0;
        tmp_reg2 = (AFEC->TRIMR0 & (~0x00000700));
        flash_vol = (tmp_reg1 & 0x00000700);
        if(flash_vol < 0x00000300)
        {
          flash_vol = 0x00000000;
        }
        else
        {
          flash_vol -= 0x00000200;
        }
        AFEC->TRIMR0 = (tmp_reg2 | flash_vol);
        if(AFEC->TRIMR0 != (tmp_reg2 | flash_vol))
        {
          AFEC->TRIMR0 = tmp_reg1;
          return (1);
        }
        
        tmp_reg3 = AFEC->TRIMR1;
        hsi_trim = 0x0000FFC0;
        AFEC->TRIMR1 = (tmp_reg3 | hsi_trim);
        if(AFEC->TRIMR1 != (tmp_reg3 | hsi_trim))
        {
          AFEC->TRIMR0 = tmp_reg1;
          AFEC->TRIMR1 = tmp_reg3;
          return (1);
        }
        
        tmp_reg2 = 0x0000000F;
        while(tmp_reg2--);
      }
  }
  
  if((FLASH->CR & FLASH_PG) != 0)
  {
    FLASH->CR &= ~FLASH_PG;
  }
  FLASH->SR  |=  FLASH_SR_CLR;

  FLASH->CR  |=  FLASH_PER;                   // Page Erase Enabled 
  FLASH->AR   =  adr;                         // Page Address
  FLASH->CR  |=  FLASH_STRT;                  // Start Erase

  while (FLASH->SR  & FLASH_BSY) {
    IWDG->KR = 0xAAAA;                        // Reload IWDG
  }

  FLASH->CR  &= ~FLASH_PER;                   // Page Erase Disabled 
  
  if((FLASH->SR & FLASH_EOP) == 0)
  {
    if(flash_flag == 1)
    {
        if((adr == 0x08000000) || (adr == 0x00000000))
        {
          AFEC->TRIMR0 = tmp_reg1;
          AFEC->TRIMR1 = tmp_reg3;
          if((AFEC->TRIMR0 != tmp_reg1)||(AFEC->TRIMR1 != tmp_reg3))
          {
            AFEC->TRIMR0 = tmp_reg1;
            AFEC->TRIMR1 = tmp_reg3;
            return (1);
          }
        }
    }
    return (1);
  }
  
  if(flash_flag == 1)
  {
      if((adr == 0x08000000) || (adr == 0x00000000))
      {
        AFEC->TRIMR0 = tmp_reg1;
        AFEC->TRIMR1 = tmp_reg3;
        if((AFEC->TRIMR0 != tmp_reg1)||(AFEC->TRIMR1 != tmp_reg3))
        {
          AFEC->TRIMR0 = tmp_reg1;
          AFEC->TRIMR1 = tmp_reg3;
          return (1);
        }
      }
  }
  
  return (0);                                   // Done
}
#endif

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
#ifdef FLASH_MEM
int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

  vu32 flash_vol = 0, hsi_trim = 0, tmp_reg1 = 0, tmp_reg2 = 0, tmp_reg3 = 0;
  if(flash_flag == 1)
  {
      if(((adr >= 0x08000000)&&(adr < 0x08000200))
       || (adr < 0x00000200))
      {
        tmp_reg1 = AFEC->TRIMR0;
        tmp_reg2 = (AFEC->TRIMR0 & (~0x00000700));
        flash_vol = (tmp_reg1 & 0x00000700);
        if(flash_vol > 0x00000400)
        {
          flash_vol = 0x00000700;
        }
        else
        {
          flash_vol += 0x00000200;
        }
        AFEC->TRIMR0 = (tmp_reg2 | flash_vol);
        if(AFEC->TRIMR0 != (tmp_reg2 | flash_vol))
        {
          AFEC->TRIMR0 = tmp_reg1;
          return (1);
        }
        
        tmp_reg3 = AFEC->TRIMR1;
        hsi_trim = (~0x0000FFC0);
        AFEC->TRIMR1 = (tmp_reg3 & hsi_trim);
        if(AFEC->TRIMR1 != (tmp_reg3 & hsi_trim))
        {
          AFEC->TRIMR0 = tmp_reg1;
          AFEC->TRIMR1 = tmp_reg3;
          return (1);
        }
        
        tmp_reg2 = 0x0000000F;
        while(tmp_reg2--);
      }
  }

  IWDG->KR = 0xAAAA;

  sz = (sz + 1) & ~1;                           // Adjust size for Half Words
  while (sz) {

    FLASH->SR  |=  FLASH_SR_CLR;

    FLASH->CR  |=  FLASH_PG;                  // Programming Enabled

    M32(adr) = *((unsigned int *)buf);      // Program Word
    while (FLASH->SR  & FLASH_BSY);

    FLASH->CR  &= ~FLASH_PG;                  // Programming Disabled

    // Check for Errors
    if (FLASH->SR & (FLASH_PGERR | FLASH_WRPRTERR)) {
      FLASH->SR |= FLASH_PGERR | FLASH_WRPRTERR;
      if(flash_flag == 1)
      {
          if(((adr >= 0x08000000)&&(adr < 0x08000200))
           || (adr < 0x00000200))
          {
            AFEC->TRIMR0 = tmp_reg1;
            AFEC->TRIMR1 = tmp_reg3;
            if((AFEC->TRIMR0 != tmp_reg1)||(AFEC->TRIMR1 != tmp_reg3))
            {
              AFEC->TRIMR0 = tmp_reg1;
              AFEC->TRIMR1 = tmp_reg3;
              return (1);
            }
          }
      }
      return (1);                               // Failed
    }

    // Go to next Word
    adr += 4;
    buf += 4;
    sz  -= 4;
  }
  
  if(flash_flag == 1)
  {
      if(((adr >= 0x08000000)&&(adr <= 0x08000200))
       || (adr <= 0x00000200))
      {
        AFEC->TRIMR0 = tmp_reg1;
        AFEC->TRIMR1 = tmp_reg3;
        if((AFEC->TRIMR0 != tmp_reg1)||(AFEC->TRIMR1 != tmp_reg3))
        {
          AFEC->TRIMR0 = tmp_reg1;
          AFEC->TRIMR1 = tmp_reg3;
          return (1);
        }
      }
  }
  return (0);                                   // Done
}
#endif
