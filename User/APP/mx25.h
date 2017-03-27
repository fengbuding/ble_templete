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

#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
#define CMD_DISWR	  0x04		/* 禁止写, 退出AAI状态 */
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_BE        0xC7		/* 批量擦除命令 */
#define CMD_SE        0x20		/* 擦除扇区命令 */

#define WIP_FLAG      0x01		/* 状态寄存器中的正在编程标志（WIP) */
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