#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include <MK20D5.H> 


#define BITBAND(Reg,Bit) (*((uint32_t volatile*)(0x42000000u + (32u*((uint32_t)&(Reg) - (uint32_t)0x40000000u)) + (4u*((uint32_t)(Bit))))))


#define		high	1        //硬件电路为反逻辑
#define		low		0

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;


#define sFLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define sFLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /*!< Read identification */
#define sFLASH_CMD_SE             0xD8  /*!< Sector Erase instruction */
#define sFLASH_CMD_BE             0xC7  /*!< Bulk Erase instruction */

#define sFLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         0x00//0xA5
#define sFLASH_SPI_PAGESIZE       0x100

#define sFLASH_M25P128_ID         0x202018
#define sFLASH_M25P64_ID          0x202017
#define sFLASH_M25P16_ID          0x202013




#define sFLASH_CS_LOW()       GPIO_ResetBits()

#define sFLASH_CS_HIGH()      GPIO_SetBits()  

#define M25P16_CS  BITBAND(PTC->PDOR,2)
#define	M25P16_CLK	BITBAND(PTC->PDOR,5)	      
#define	M25P16_MOSI	BITBAND(PTC->PDOR,6)	  	 
#define M25P16_MISO BITBAND(PTC->PDIR,7)	

#define M25P16_CKL_EN BITBAND(PTA->PDOR,4)	
#define	M25P16_MOSI_EN	BITBAND(PTB->PDOR,0)	




void pin_init();
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_StartReadSequence(uint32_t ReadAddr);
uint32_t sFLASH_ReadID(void);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint8_t sFLASH_ReadNByte(uint8_t Reg,uint16_t num,uint8_t *pdata);
uint8_t sFLASH_ReadByte(uint8_t Reg);
uint8_t sFLASH_SendByte(uint8_t byte);
void sFLASH_WriteEnable(void);
void sFLASH_WaitForWriteEnd(void);

void Test(void);
uint8_t write_spiflash(uint16_t addr,uint8_t *buf);


#endif

