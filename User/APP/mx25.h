#ifndef _MX25_H_
#define _MX25_H_

#include "nrf_gpio.h"
#include "nrf_delay.h"

#define MOSI_PIN  24
#define MISO_PIN  23
#define SCLK_PIN  25
#define CS_PIN    30

#define SCLK_0()  NRF_GPIO->OUTCLR = (1UL << SCLK_PIN)
#define SCLK_1()  NRF_GPIO->OUTSET = (1UL << SCLK_PIN)
#define MOSI_0()  NRF_GPIO->OUTCLR = (1UL << MOSI_PIN)
#define MOSI_1()  NRF_GPIO->OUTSET = (1UL << MOSI_PIN)
#define CS_0()    NRF_GPIO->OUTCLR = (1UL << CS_PIN)
#define CS_1()    NRF_GPIO->OUTSET = (1UL << CS_PIN)
#define MISO_IS_HIGH() (((NRF_GPIO->IN >> MISO_PIN) & 0x1UL) == 1)

#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define CMD_SE        0x20		/* ������������ */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */
#define  SPI_FLASH_PageSize  256

void Mx25_Port_Init(void);
uint32_t sf_ReadID(void);
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_EraseChip(void);
void sf_EraseSector(uint32_t _uiSectorAddr);
void bsp_InitSFlash(void);
void SPI_FLASH_BufferWrite(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
#endif