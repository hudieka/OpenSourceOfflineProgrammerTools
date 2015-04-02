

#include "spi_flash.h"
#include <stdint.h>

#define		DELAYS	20		 //min:128

#define	DELAY_us	250		 //min:128

uint32_t g_ulM25P16ID = 0 ;

void GPIO_ResetBits()
{
	M25P16_CS = 0;
}

void GPIO_SetBits() 
{
	M25P16_CS = 1;
}

void DelayS(uint16_t delay)
{
	uint8_t i;
	if(delay > 50)
	{
		delay = DELAYS;
	}

	for(i = 0; i < delay;i++)
	{
		;
	}
	//while(delay--);
}



void sFLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
  sFLASH_SendByte(sFLASH_CMD_SE);
  /*!< Send SectorAddr high nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  sFLASH_SendByte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Erases the entire FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_EraseBulk(void)
{
  /*!< Send write enable instruction */
  sFLASH_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  sFLASH_SendByte(sFLASH_CMD_BE);
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  sFLASH_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sFLASH_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sFLASH_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH.
  * @retval None
  */
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        sFLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        sFLASH_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

      sFLASH_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}


/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void sFLASH_StartReadSequence(uint32_t ReadAddr)
{
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  sFLASH_SendByte(sFLASH_CMD_READ);

  /*!< Send the 24-bit address of the address to read from -------------------*/
  /*!< Send ReadAddr high nibble address byte */
  sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte */
  sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte */
  sFLASH_SendByte(ReadAddr & 0xFF);
}


uint32_t sFLASH_ReadID(void)
{

  pin_init();
	
  /*!< Select the FLASH: Chip Select low */
  M25P16_CKL_EN = 1;
  M25P16_MOSI_EN = 1;
  DelayS(49);
  sFLASH_CS_LOW();

  sFLASH_ReadNByte(sFLASH_CMD_RDID,4,(uint8_t *)(&g_ulM25P16ID));

  sFLASH_CS_HIGH();

  M25P16_CKL_EN = 0;
  M25P16_MOSI_EN = 0;
  DelayS(49);
  return 0;
}
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{

	while(NumByteToRead-- )
	{
		sFLASH_CS_LOW();
		
		sFLASH_SendByte(sFLASH_CMD_READ);
		
		/*!< Send ReadAddr high nibble address byte to read from */
		sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
		/*!< Send ReadAddr medium nibble address byte to read from */
		sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
		/*!< Send ReadAddr low nibble address byte to read from */

		*pBuffer = sFLASH_ReadByte((ReadAddr & 0xFF));
			
		/*!< Deselect the FLASH: Chip Select high */
		sFLASH_CS_HIGH();

		pBuffer++;
		ReadAddr++;

	}



}

#if 0
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	
	/*!< Send "Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_READ);
	
	/*!< Send ReadAddr high nibble address byte to read from */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte to read from */
	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	sFLASH_SendByte(ReadAddr & 0xFF);

	//sFLASH_SendByte(sFLASH_DUMMY_BYTE);

	sFLASH_ReadNByte(sFLASH_DUMMY_BYTE,NumByteToRead,pBuffer);
		
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();

}
#endif
uint8_t sFLASH_ReadNByte(uint8_t Reg,uint16_t num,uint8_t *pdata)
{
	uint8_t i = 0,Regdata = 0;
	uint8_t AD_Out_Data;

	Regdata = Reg;

	while (num--)
	{
		M25P16_CLK = high;
		M25P16_MOSI = low; 

		//--------------写入通信命令------------------------------------
		for(i=0;i<8;i++)
		{
			M25P16_CLK = low;
			DelayS(DELAY_us);
			if((Regdata & 0x80) == 0x80)
			{	
				M25P16_MOSI = high;
			}
			else
			{ 
				M25P16_MOSI = low;
			}
			DelayS(DELAY_us);	
			Regdata <<= 1;	
			M25P16_CLK = high;
			DelayS(DELAY_us);
		}
		Regdata = sFLASH_DUMMY_BYTE;
		
		AD_Out_Data = 0;
		for(i=0;i<8;i++)
		{
			M25P16_CLK = low;
			DelayS(DELAY_us); 
			M25P16_CLK = high;
			DelayS(DELAY_us); 
			AD_Out_Data = (AD_Out_Data<<1) | M25P16_MISO; 
			DelayS(DELAY_us); 
		}	
		*pdata = AD_Out_Data;
		pdata++;
		
		M25P16_CLK = high;
		M25P16_MOSI = low;


	}



	return 1;
}


uint8_t sFLASH_ReadByte(uint8_t Reg)
{
	uint8_t i;
	uint8_t AD_Out_Data;

	M25P16_CLK = high;
	M25P16_MOSI = low; 
	//M25P16_CS  = low;
 
	//--------------写入通信命令------------------------------------
	for(i=0;i<8;i++)
	{
		M25P16_CLK = low;
		if((Reg & 0x80) == 0x80)
		{	
			M25P16_MOSI = high;
		}
		else
		{ 
			M25P16_MOSI = low;
		}
		DelayS(DELAY_us);	
		Reg <<= 1;	
		M25P16_CLK = high;
		DelayS(DELAY_us);
	}

	for(i=0;i<8;i++)
	{
		M25P16_CLK = low;
		DelayS(DELAY_us);	
		M25P16_CLK = high;
		DelayS(DELAY_us);	
		AD_Out_Data = (AD_Out_Data<<1) | M25P16_MISO; 
		DelayS(DELAY_us);	
	}

	//M25P16_CS = high;
	M25P16_CLK = high;
	M25P16_MOSI = low;

	return AD_Out_Data;


}

uint8_t sFLASH_SendByte(uint8_t byte)
{
	uint8_t i;
	
	M25P16_CLK = high;
	M25P16_MOSI = low; 
	//M25P16_CS  = low;
	DelayS(DELAY_us); 
	//--------------写入通信命令------------------------------------
	for(i=0;i<8;i++)
	{
		M25P16_CLK = low;	
		if((byte & 0x80) == 0x80)
		{	
			M25P16_MOSI = high;
		}
		else
		{ 
			M25P16_MOSI = low;
		}
		DelayS(DELAY_us); 
		byte <<= 1;	
		M25P16_CLK = high;	
		DelayS(DELAY_us);
	}
	M25P16_CLK = high;
	//M25P16_CS = high;
	M25P16_MOSI = low;
	
	return 1;
}

void sFLASH_WriteEnable(void)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Write Enable" instruction */
	sFLASH_SendByte(sFLASH_CMD_WREN);

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}


void sFLASH_WaitForWriteEnd(void)
{
	uint8_t flashstatus = 0;
	
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	
	/*!< Send "Read Status Register" instruction */
	//sFLASH_SendByte(sFLASH_CMD_RDSR);
	
	/*!< Loop as long as the memory is busy with a write cycle */
	do
	{
	  /*!< Send a dummy byte to generate the clock needed by the FLASH
	  and put the value of the status register in FLASH_Status variable */
	  flashstatus = sFLASH_ReadByte(sFLASH_CMD_RDSR);
	
	}
	while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */
	
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();

}





#define  FLASH_WriteAddress     0x000000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

uint8_t Tx_Buffer[20] = {0x12,0x34,0x56,0x78,0x90,0x55};
uint8_t Rx_Buffer[20] = {0};

void Test()
{
	sFLASH_EraseSector(FLASH_SectorToErase);
	sFLASH_WriteBuffer(Tx_Buffer, FLASH_WriteAddress, 20);
	sFLASH_ReadBuffer(Rx_Buffer, FLASH_ReadAddress, 20);

}


uint8_t write_spiflash(uint16_t addr,uint8_t *buf)
{
	uint8_t ret = 0 , i = 0; 

	//判断ID
	while(1)
	{
		sFLASH_CS_LOW();
 		sFLASH_ReadID();
		sFLASH_CS_HIGH();
		i++;
		if((i>3)||(g_ulM25P16ID == 0x1320))
		{
			i = 0;
			break;
		}
	}


	if(g_ulM25P16ID == 0x1320)
	{
		if((addr%1024) == 0)
		{
			sFLASH_EraseSector(addr*128);
		}
		sFLASH_WriteBuffer(buf,addr*128,128);
		g_ulM25P16ID = 0;				//下次通过读取ID
		ret = 1;

	}
	else
	{
		ret = 0;
	}

	return ret;

}



void pin_init()
{

	PORTC->PCR[2] |= PORT_PCR_MUX(1);
	PORTC->PCR[5] |= PORT_PCR_MUX(1);
	PORTC->PCR[6] |= PORT_PCR_MUX(1);
	PORTC->PCR[7] |= PORT_PCR_MUX(1);
	
	PORTA->PCR[4] |= PORT_PCR_MUX(1);
	PORTB->PCR[0] |= PORT_PCR_MUX(1);

	PTC->PDDR |= (1 << 2)|(1<<5)|(1 << 6);
	PTB->PDDR |= (1 << 0);
	PTA->PDDR |= (1 << 4);


}





