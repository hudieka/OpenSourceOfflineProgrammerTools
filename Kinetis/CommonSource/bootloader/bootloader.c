 /******************************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved                       
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file      bootloader.c
*
* @author    b01119
* 
* @version   0.0.32.0
* 
* @date      May-7-2013
* 
* @brief     Bootloader state machine file
*
******************************************************************************/

#include "bootloader.h"
#include "flash_kinetis.h"
#include "wdg.h"

LWord *Pbuf  = 0;


extern int __main(void);
extern unsigned long __BOOT_STACK_ADDRESS[];
void Delay(int time);

#if defined(__IAR_SYSTEMS_ICC__)
	// Suppress the warning message in IAR with non - alligned member in structure.
	#pragma diag_suppress=Pa039

	#pragma section = "IntVectTable"
  #pragma location = "IntVectTable"

  __root const vector_entry __vector_table[16] = 
#elif defined(__ARMCC_VERSION)
#pragma diag_suppress=61
    __attribute__((section(".IntVectTable"))) const vector_entry __vector_table[16] __attribute__((used)) =   

#elif defined(__CWCC__)
#pragma  define_section vectors_rom ".vectors_rom" far_abs R
	__declspec(vectors_rom) vector_entry __vector_table[16] =     		
#endif    
{
   (pointer*)__BOOT_STACK_ADDRESS,  /* Initial SP           */
   (vector_entry)__main,                            /* Initial PC           */
   Boot_ResetMCU,     /* Non-maskable Interrupt (NMI) */
   Boot_ResetMCU,     /* Hard Fault */
   Boot_ResetMCU,     /* MemManage Fault */
   Boot_ResetMCU,     /* Bus Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU,     /* Usage Fault */
   Boot_ResetMCU      /* Usage Fault */   
};




//-----------------------------------------------------------------------------
// FUNCTION:    Boot_ResetMCU
// SCOPE:       Bootloader application system function
// DESCRIPTION: The function resets the MCU and support CONNECTED mode
//              
// PARAMETERS:  enableBootloader - TRUE/FALSE
//              
// RETURNS:     none
//-----------------------------------------------------------------------------  
void Boot_ResetMCU(void)
{ 
  while(1)
  {};   
}


void Delay(int time)
{
  int i = 0, j = 0;
  for(i = 0; i < time; i++)
  {
      for(j = 0; j < 1000; j++)
      {
          ;
      }
  }
  
}



//-----------------------------------------------------------------------------
// FUNCTION:    Bootloader
// SCOPE:       Bootloader application system function 
// DESCRIPTION: This is main function of the project it substitute normal main function

//              
// PARAMETERS:  none
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
/* Entry point of bootloader */
/* Entry point of bootloader */

int __main(void) @".mainptr"
{
  uint32_t chk = 0 , i = 0;
  uint32_t *p;
  uint32_t *pram;
  uint32_t *pflash;
  uint32_t unsec = 0xfffffffe;
  
  
  /* initialize any hardware specific issues */
  #if BOOTLOADER_INT_WATCHDOG == 1
    WDG_Enable(); 
  #else
    WDG_Disable();    
  #endif

  FLASH_Initialization();

  #if 0

  SIM_SCGC5_REG(SIM_BASE_PTR) |= SIM_SCGC5_PORTB_MASK;
  SIM_SCGC5_REG(SIM_BASE_PTR) |= SIM_SCGC5_PORTD_MASK;
  PORT_PCR_REG(PORTB_BASE_PTR,18) = PORT_PCR_MUX(1);
  PORT_PCR_REG(PORTB_BASE_PTR,19) = PORT_PCR_MUX(1);
  PORT_PCR_REG(PORTD_BASE_PTR,1) = PORT_PCR_MUX(1);

  GPIO_PDDR_REG(PTB_BASE_PTR) |= (1 << 18);
  GPIO_PDDR_REG(PTB_BASE_PTR) |= (1 << 19);
  GPIO_PDDR_REG(PTD_BASE_PTR) |= (1 << 1);
  
  
  while(1)
  {

	if(HWBFR == 0x500)
	{

        if(*((unsigned int*)0x1fffff00) == 0x00)
	    {
			GPIO_PDDR_REG(PTB_BASE_PTR) &= ~(1 << 19);
	    }
	}
  
  }

  //while(1)
  //{
	//GPIO_PTOR_REG(PTB_BASE_PTR) |= (1 << 18);
    //Delay(2000);
  //}
  #endif
  
  
  // this setup all clock
  //INIT_CLOCKS_TO_MODULES

  HWSID = SECTOR_SIZE;
  HWFCG1 = RAMBUF_SIZE;
  HWFCG2 = RAMBUF_ADDR;
  
  HWESR = CPU_WAIT;
  
  while(1)
  {
	  
	  if(HWCSR == CMD_RUN)
	  {

		  if(HWCMR == CMD_ERASE)
		  {

			  HWESR = CPU_RUN;
			  //if((HWBFR) < CPUInfo->PFlashSize)
			  {
				  if(FLASH_EraseSector(HWBFR) == FLASH_OK)
				  {
                                      /*
                                      if((HWBFR >= 0x400)&&(HWBFR < 0x800))
                                      {
                                          FLASH_ProgramSectionByLongs(0x40C, &unsec, 1);
                                      }
                                      */
                                      HWERR = ERR_NONE;
				  }
				  else
				  {
					  HWERR |= ERR_FLASH;
				  }
  
			  }
			  
			  HWCMR = CMD_IDLE;
			  HWESR = CPU_STOP;
			  
		  }
		  else if(HWCMR == CMD_PROGRA)
		  {
			  HWESR = CPU_RUN;
  
			  {
			      Pbuf = (LWord *)RAMBUF_ADDR;
				  HWBUFTEST = (uint32_t)Pbuf;
				  if(FLASH_ProgramSectionByLongs(HWBFR, Pbuf, HWBCR) == FLASH_OK)
				  {
					  
					  HWERR = ERR_NONE;
				  }
				  else
				  {
					  HWERR |= ERR_FLASH;
				  }

				  //check ram and flash
				  #if 1
				  pram = HWBUF;
				  pflash = (uint32_t *)HWBFR;
				  
				  for(i = 0; i< HWBCR; i++)
				  {
				  	if(*pram != *pflash)
				  	{
				  		HWERR |= ERR_CHK;
						break;
					}
					
				  }

				  chk = 0;
				  p = (uint32_t *)HWBFR;

				  for(i = 0; i< HWBCR; i++)
				  {
					  chk += *p;
					  p++;
				  }

				  HWCHK = chk;
				  #endif
  
			  }
			  
			  HWCMR = CMD_IDLE;
			  HWESR = CPU_STOP;
  
		  }
		  else
		  {
			  HWERR |= ERR_CMD;
		  }
  
		  HWCSR = CMD_DONW;
  
	  }
  
  
  }


}
