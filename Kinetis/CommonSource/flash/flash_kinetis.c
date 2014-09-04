/*****************************************************************************
 * (c) Copyright 2011, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      flash_kinetis.c
 * @author    R20253
 * @version   1.0.17.0
 * @date      Feb-15-2013
 * @brief     Flash programming driver
 * @par       
 * @include   
 * @par       
 * @include         
 ******************************************************************************/

#include "kinetis_params.h"
#include "flash_kinetis.h"
#include "bootloader.h"



  
FCC0B_STR CommandObj;


//  internal driver function
extern LWord FLASH_FlashCommandSequenceStart(Byte index);

/********************************************************
* Init Function 
*
********************************************************/
void FLASH_Initialization(void)
{
  LWord i;
  volatile Byte* ptr_FlashCommandSequenceStart = ((Byte*)FLASH_FlashCommandSequenceStart - 1);
  
  
  //  inititalization of flash clock module 
   FLASH_INIT_FLASH_CLOCK; // done in main function
}


/********************************************************
* Function for Programming of one Long Word 
*
********************************************************/
LWord FLASH_ProgramLongWord(LWord destination, LWord data32b)
{
  /* preparing passing parameter to program the flash block */



		
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FLASH_PROGRAM_LONGWORD;
  CommandObj.regsLong.fccob7654 = data32b; 
/*
  if(destination == 0x500)
  {
		CommandObj.regsLong.fccob7654  = 0x9804B003;
  }
*/
#if defined(KINETIS_E)
	return FLASH_FlashCommandSequenceStart(PROGRAM_LONGWORD_INDEX);
#else
	return FLASH_FlashCommandSequenceStart(PROGRAM_LONGWORD_INDEX);
#endif

  
}

/********************************************************
* Function for Programming of one Long Word 
*
********************************************************/
LWord FLASH_ProgramPhrase(LWord destination, LWord * data64b)
{
  /* preparing passing parameter to program the flash block */
  
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FLASH_PROGRAM_PHRASE;
  CommandObj.regsLong.fccob7654 = data64b[0];
  CommandObj.regsLong.fccobBA98 = data64b[1];
  
  return FLASH_FlashCommandSequenceStart(PROGRAM_PHRASE_INDEX);
}

LWord data = 0;

/********************************************************
* Function for Programming of section by simple longs
*
********************************************************/
LWord FLASH_ProgramSectionByLongs(LWord destination, LWord* pSource, LWord size)
{ 

  LWord *pdata;

  pdata = pSource;
  
  while(size--)
  {  
/*
  	if(destination == 0x500)
  	{
  		data = *(uint32_t *)0x1fffff00;
		//FLASH_EraseSector(0x5000);
		FLASH_ProgramLongWord(0x6008, data);
        FLASH_ProgramLongWord(0x600c, *pdata);
		FLASH_ProgramLongWord(0x6010, (LWord)pdata);
	}
*/	
	data = *pdata;
    
    if(FLASH_ProgramLongWord(destination, *pdata++) != FLASH_OK)
    {
		return FLASH_FAIL;
    }
	
    destination += 4;

	//don't program sec bit
	if(destination == 0x40C)
	{
          
          if(HWFSEC == LOCK_MCU)
  	  {
		*pdata = 0xFFFFFFFD;
	  }
	  else
	  {
		  *pdata = 0xFFFFFFFE;

	  }
          
	}
        

  }
  return FLASH_OK;
}

/********************************************************
* Function for Programming of one section (maximum is 2048 Bytes) 
*
********************************************************/
LWord FLASH_ProgramSectionByPhrases(LWord destination, LWord* pSource, LWord size)
{
  LWord my_size;
  
  // check the size of memory 
  if(size >= FLASH_BURST_RAM_SIZE - 2)
    return FLASH_FAIL;
  
  if(destination & 0x00000003)
    return FLASH_FAIL;
  
  // it's neccessary alligment on start of buffer?
  if(destination & 0x00000004)
  {
    LWord tmp_buff[2];
    tmp_buff[0] = 0xFFFFFFFF;    
    tmp_buff[1] = *(pSource++);
    
    //  call flash phrase
    if(FLASH_ProgramPhrase((destination & 0xFFFFFFF8), tmp_buff))
      return FLASH_FAIL;
    
    destination += 4;    
  }
  
  my_size = size & 0xFFFFFFFE;
 
  if(my_size)
  {
    //  flash all phrases
    while(my_size)
    {
      //  call flash phrase
      if(FLASH_ProgramPhrase(destination, pSource))
        return FLASH_FAIL;
    
      pSource += 2;
      destination += 8;
      my_size -= 2;      
    }
  }
  
  // it's neccessary alligment on end of buffer?
  if(size & 0x00000001)
  {
    LWord tmp_buff[2];
    tmp_buff[0] = *pSource;    
    tmp_buff[1] = 0xFFFFFFFF;
    
    //  call flash phrase
    if(FLASH_ProgramPhrase(destination, tmp_buff))
      return FLASH_FAIL;           
  }
  return FLASH_OK;
}

/********************************************************
* Function for erasing of flash memory sector (0x800)
*
********************************************************/
LWord FLASH_EraseSector(LWord destination)
{  
  LWord ret = 0, value = 0;
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FLASH_ERASE_SECTOR;

  #if defined(KINETIS_E)
		ret =  FLASH_FlashCommandSequenceStart(ERASE_BLOCK_INDEX);
  #else
		ret =  FLASH_FlashCommandSequenceStart(ERASE_BLOCK_INDEX);
  #endif

  /*
  //Unlock the sec bit
  if((destination >= 0x400)&&(destination < 0x800))
  {
  	  if(HWFSEC == LOCK_MCU)
  	  {
		value = 0xFFFFFFFD;
		FLASH_ProgramLongWord(0x40C,value);
	  }
	  else
	  {
		  value = 0xFFFFFFFE;
		  FLASH_ProgramLongWord(0x40C,value);
	  }
	  
  }
  */
  
  return ret;
}



