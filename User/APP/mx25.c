#include "mx25.h"

void sf_WriteStatus(uint8_t _ucValue);
void sf_WaitForWriteEnd(void);
void Write_Byte(uint8_t _ucByte);
void bsp_spiDelay(void)
{
  nrf_delay_us(2);
}
void Mx25_Port_Init(void)
{
  nrf_gpio_cfg_output(MOSI_PIN);
  nrf_gpio_cfg_output(SCLK_PIN);
  nrf_gpio_cfg_output(CS_PIN);
  nrf_gpio_cfg_input(MISO_PIN, NRF_GPIO_PIN_PULLUP);
}
/*
*********************************************************************************************************
*	函 数 名: bsp_InitSpiFlash
*	功能说明: 初始化串行Flash硬件接口（配置STM32的SPI时钟、GPIO)
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{   
    //配置GPIO 
	Mx25_Port_Init();			
    nrf_delay_ms(100); 
    
    //软件方式，使能串行Flash片选 
	CS_0();			
    
   //发送禁止写入的命令,即使能软件写保护 
	Write_Byte(CMD_DISWR);		
   
   //软件方式，禁能串行Flash片选 
	CS_1();	
   
   //等待串行Flash内部操作完成 
	sf_WaitForWriteEnd();	
	
    //解除所有BLOCK的写保护 
	sf_WriteStatus(0);		
   
    
}
static void Write_Byte(uint8_t _ucByte)
{
  	uint8_t i;

	for(i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			MOSI_1();
		}
		else
		{
			MOSI_0();
		}
		bsp_spiDelay();
		SCLK_0();
		_ucByte <<= 1;
		bsp_spiDelay();
		SCLK_1();
	}
	bsp_spiDelay();
}

static uint8_t Read_Byte(void)
{
  	uint8_t i;
	uint8_t read = 0;

	for (i = 0; i < 8; i++)
	{
        SCLK_0();
		bsp_spiDelay();
		read = read<<1;
		if (MISO_IS_HIGH())
		{
			read++;
		}
		SCLK_1();
		bsp_spiDelay();
	}
	return read;
}
/*
*********************************************************************************************************
*	函 数 名: sf_WriteEnable
*	功能说明: 向器件发送写使能命令
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
	CS_0();									/* 使能片选 */
	Write_Byte(CMD_WREN);								/* 发送命令 */
	CS_1();										/* 禁能片选 */
}
/*
*********************************************************************************************************
*	函 数 名: sf_WaitForWriteEnd
*	功能说明: 采用循环查询的方式等待器件内部写操作完成
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	CS_0();										/* 使能片选 */
	Write_Byte(CMD_RDSR);							/* 发送命令， 读状态寄存器 */
	while((Read_Byte() & WIP_FLAG) == 1);	/* 判断状态寄存器的忙标志位 */
	CS_1();										/* 禁能片选 */
}
/*
*********************************************************************************************************
*	函 数 名: sf_ReadID
*	功能说明: 读取器件ID
*	形    参:  无
*	返 回 值: 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	CS_0();										/* 使能片选 */
	Write_Byte(CMD_RDID);								/* 发送读ID命令 */
	id1 = Read_Byte();					/* 读ID的第1个字节 */
	id2 = Read_Byte();					/* 读ID的第2个字节 */
	id3 = Read_Byte();					/* 读ID的第3个字节 */
	CS_1();										/* 禁能片选 */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/*
*********************************************************************************************************
*	函 数 名: sf_WriteStatus
*	功能说明: 写状态寄存器
*	形    参:  _ucValue : 状态寄存器的值
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WriteStatus(uint8_t _ucValue)
{
		CS_0();										/* 使能片选 */
		Write_Byte(CMD_WRSR);							/* 发送命令， 写状态寄存器 */
		Write_Byte(_ucValue);							/* 发送数据：状态寄存器的值 */
		CS_1();										/* 禁能片选 */
}


/*
*********************************************************************************************************
*	函 数 名: sf_PageWrite
*	功能说明: 向一个page内写入若干字节。字节个数不能超出页面大小（4K)
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiWriteAddr ：目标区域首地址
*				_usSize ：数据个数，不能超过页面大小
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
        uint32_t i;
        sf_WriteEnable();								/* 发送写使能命令 */

        CS_0();											/* 使能片选 */
        Write_Byte(0x02);								/* 发送AAI命令(地址自动增加编程) */
        Write_Byte((_uiWriteAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
        Write_Byte((_uiWriteAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
        Write_Byte(_uiWriteAddr & 0xFF);				/* 发送扇区地址低8bit */
        if(_usSize > 256)
        {
          _usSize = 256;
        }
        for (i = 0; i < _usSize; i++)
        {
            Write_Byte(*_pBuf++);					/* 发送数据 */
        }

        CS_1();								/* 禁止片选 */

        sf_WaitForWriteEnd();						/* 等待串行Flash内部写操作完成 */

    /* 进入写保护状态 */
    CS_0();	
    Write_Byte(CMD_DISWR);
    CS_1();	

    sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
} 
/*
*********************************************************************************************************
*	函 数 名: sf_ReadBuffer
*	功能说明: 连续读取若干字节。字节个数不能超出芯片容量。
*	形    参:  	_pBuf : 数据源缓冲区；
*				_uiReadAddr ：首地址
*				_usSize ：数据个数, 可以大于PAGE_SIZE,但是不能超出芯片总容量
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
//	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
//	{
//		return;
//	}

	/* 擦除扇区操作 */
	CS_0();										/* 使能片选 */
	Write_Byte(CMD_READ);							/* 发送读命令 */
	Write_Byte((_uiReadAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	Write_Byte((_uiReadAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	Write_Byte(_uiReadAddr & 0xFF);				/* 发送扇区地址低8bit */
	while (_uiSize--)
	{
		*_pBuf++ = Read_Byte();			/* 读一个字节并存储到pBuf，读完后指针自加1 */
	}
	CS_1();										/* 禁能片选 */
}   
/*
*********************************************************************************************************
*	函 数 名: sf_EraseChip
*	功能说明: 擦除整个芯片
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_EraseChip(void)
{
	sf_WriteEnable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	CS_0();										/* 使能片选 */
	Write_Byte(CMD_BE);							/* 发送整片擦除命令 */
	CS_1();										/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}
/*
*********************************************************************************************************
*	函 数 名: sf_EraseSector
*	功能说明: 擦除指定的扇区
*	形    参:  _uiSectorAddr : 扇区地址
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	CS_0();										/* 使能片选 */
	Write_Byte(CMD_SE);								/* 发送擦除命令 */
	Write_Byte((_uiSectorAddr & 0xFF0000) >> 16);	/* 发送扇区地址的高8bit */
	Write_Byte((_uiSectorAddr & 0xFF00) >> 8);		/* 发送扇区地址中间8bit */
	Write_Byte(_uiSectorAddr & 0xFF);				/* 发送扇区地址低8bit */
	CS_1();										/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain = 256 - WriteAddr % 256; 		 	    
	if(NumByteToWrite <= pageremain)
      pageremain = NumByteToWrite;
	while(1)
	{	   
		sf_PageWrite(pBuffer, WriteAddr, pageremain);
		if(NumByteToWrite == pageremain)
          break;
	 	else 
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  
			if(NumByteToWrite > 256)
              pageremain = 256; 
			else
              pageremain = NumByteToWrite; 	 
		}
	};	    
} 

uint8_t W25QXX_BUFFER[4096];		 
void SPI_FLASH_BufferWrite(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF = W25QXX_BUFFER;	     
 	secpos = WriteAddr / 4096;  
	secoff = WriteAddr % 4096;
	secremain = 4096 - secoff;  
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);
 	if(NumByteToWrite <= secremain)
      secremain = NumByteToWrite;
	while(1) 
	{	
		sf_ReadBuffer(W25QXX_BUF, secpos * 4096, 4096);
		for(i=0; i < secremain; i++)
		{
			if(W25QXX_BUF[secoff+i] != 0XFF)break;  
		}
		if(i < secremain)
		{
			sf_EraseSector(secpos * 4096);		
			for(i = 0; i < secremain; i++)	   		
			{
				W25QXX_BUF[i+secoff] = pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); 

		}
        else 
        {
          W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);	
        }
		if(NumByteToWrite == secremain)
        {
          break;
        }
		else
		{
			secpos++;
			secoff = 0;
		   	pBuffer += secremain;  				
			WriteAddr += secremain;					   
		   	NumByteToWrite -= secremain;			
			if(NumByteToWrite > 4096)
            {
              secremain = 4096;
            }
			else 
            {
              secremain = NumByteToWrite;	
            }
		}	 
	};	 
}
