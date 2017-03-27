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
*	�� �� ��: bsp_InitSpiFlash
*	����˵��: ��ʼ������FlashӲ���ӿڣ�����STM32��SPIʱ�ӡ�GPIO)
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{   
    //����GPIO 
	Mx25_Port_Init();			
    nrf_delay_ms(100); 
    
    //�����ʽ��ʹ�ܴ���FlashƬѡ 
	CS_0();			
    
   //���ͽ�ֹд�������,��ʹ�����д���� 
	Write_Byte(CMD_DISWR);		
   
   //�����ʽ�����ܴ���FlashƬѡ 
	CS_1();	
   
   //�ȴ�����Flash�ڲ�������� 
	sf_WaitForWriteEnd();	
	
    //�������BLOCK��д���� 
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
*	�� �� ��: sf_WriteEnable
*	����˵��: ����������дʹ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
	CS_0();									/* ʹ��Ƭѡ */
	Write_Byte(CMD_WREN);								/* �������� */
	CS_1();										/* ����Ƭѡ */
}
/*
*********************************************************************************************************
*	�� �� ��: sf_WaitForWriteEnd
*	����˵��: ����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	CS_0();										/* ʹ��Ƭѡ */
	Write_Byte(CMD_RDSR);							/* ������� ��״̬�Ĵ��� */
	while((Read_Byte() & WIP_FLAG) == 1);	/* �ж�״̬�Ĵ�����æ��־λ */
	CS_1();										/* ����Ƭѡ */
}
/*
*********************************************************************************************************
*	�� �� ��: sf_ReadID
*	����˵��: ��ȡ����ID
*	��    ��:  ��
*	�� �� ֵ: 32bit������ID (���8bit��0����ЧIDλ��Ϊ24bit��
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	CS_0();										/* ʹ��Ƭѡ */
	Write_Byte(CMD_RDID);								/* ���Ͷ�ID���� */
	id1 = Read_Byte();					/* ��ID�ĵ�1���ֽ� */
	id2 = Read_Byte();					/* ��ID�ĵ�2���ֽ� */
	id3 = Read_Byte();					/* ��ID�ĵ�3���ֽ� */
	CS_1();										/* ����Ƭѡ */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}

/*
*********************************************************************************************************
*	�� �� ��: sf_WriteStatus
*	����˵��: д״̬�Ĵ���
*	��    ��:  _ucValue : ״̬�Ĵ�����ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WriteStatus(uint8_t _ucValue)
{
		CS_0();										/* ʹ��Ƭѡ */
		Write_Byte(CMD_WRSR);							/* ������� д״̬�Ĵ��� */
		Write_Byte(_ucValue);							/* �������ݣ�״̬�Ĵ�����ֵ */
		CS_1();										/* ����Ƭѡ */
}


/*
*********************************************************************************************************
*	�� �� ��: sf_PageWrite
*	����˵��: ��һ��page��д�������ֽڡ��ֽڸ������ܳ���ҳ���С��4K)
*	��    ��:  	_pBuf : ����Դ��������
*				_uiWriteAddr ��Ŀ�������׵�ַ
*				_usSize �����ݸ��������ܳ���ҳ���С
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
        uint32_t i;
        sf_WriteEnable();								/* ����дʹ������ */

        CS_0();											/* ʹ��Ƭѡ */
        Write_Byte(0x02);								/* ����AAI����(��ַ�Զ����ӱ��) */
        Write_Byte((_uiWriteAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
        Write_Byte((_uiWriteAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
        Write_Byte(_uiWriteAddr & 0xFF);				/* ����������ַ��8bit */
        if(_usSize > 256)
        {
          _usSize = 256;
        }
        for (i = 0; i < _usSize; i++)
        {
            Write_Byte(*_pBuf++);					/* �������� */
        }

        CS_1();								/* ��ֹƬѡ */

        sf_WaitForWriteEnd();						/* �ȴ�����Flash�ڲ�д������� */

    /* ����д����״̬ */
    CS_0();	
    Write_Byte(CMD_DISWR);
    CS_1();	

    sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
} 
/*
*********************************************************************************************************
*	�� �� ��: sf_ReadBuffer
*	����˵��: ������ȡ�����ֽڡ��ֽڸ������ܳ���оƬ������
*	��    ��:  	_pBuf : ����Դ��������
*				_uiReadAddr ���׵�ַ
*				_usSize �����ݸ���, ���Դ���PAGE_SIZE,���ǲ��ܳ���оƬ������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	/* �����ȡ�����ݳ���Ϊ0���߳�������Flash��ַ�ռ䣬��ֱ�ӷ��� */
//	if ((_uiSize == 0) ||(_uiReadAddr + _uiSize) > g_tSF.TotalSize)
//	{
//		return;
//	}

	/* ������������ */
	CS_0();										/* ʹ��Ƭѡ */
	Write_Byte(CMD_READ);							/* ���Ͷ����� */
	Write_Byte((_uiReadAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	Write_Byte((_uiReadAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	Write_Byte(_uiReadAddr & 0xFF);				/* ����������ַ��8bit */
	while (_uiSize--)
	{
		*_pBuf++ = Read_Byte();			/* ��һ���ֽڲ��洢��pBuf�������ָ���Լ�1 */
	}
	CS_1();										/* ����Ƭѡ */
}   
/*
*********************************************************************************************************
*	�� �� ��: sf_EraseChip
*	����˵��: ��������оƬ
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_EraseChip(void)
{
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	CS_0();										/* ʹ��Ƭѡ */
	Write_Byte(CMD_BE);							/* ������Ƭ�������� */
	CS_1();										/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}
/*
*********************************************************************************************************
*	�� �� ��: sf_EraseSector
*	����˵��: ����ָ��������
*	��    ��:  _uiSectorAddr : ������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	CS_0();										/* ʹ��Ƭѡ */
	Write_Byte(CMD_SE);								/* ���Ͳ������� */
	Write_Byte((_uiSectorAddr & 0xFF0000) >> 16);	/* ����������ַ�ĸ�8bit */
	Write_Byte((_uiSectorAddr & 0xFF00) >> 8);		/* ����������ַ�м�8bit */
	Write_Byte(_uiSectorAddr & 0xFF);				/* ����������ַ��8bit */
	CS_1();										/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
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
