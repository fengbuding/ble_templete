#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "simple_uart.h"
//#include "mx25.h"
#include "nrf_adc.h"

#define TEST_SIZE 0x1111


nrf_adc_config_t adc;
void adc_init(void)
{
    adc.reference  = NRF_ADC_CONFIG_REF_VBG;
    adc.resolution = NRF_ADC_CONFIG_RES_10BIT;
    adc.scaling    = NRF_ADC_CONFIG_SCALING_INPUT_FULL_SCALE;
    nrf_adc_configure(&adc);
}
void  get_adc(void)
{
   int32_t  dat = 0;
   dat = nrf_adc_convert_single(NRF_ADC_CONFIG_INPUT_4);
   printf("adc is %d   ",dat);
   dat = dat * 1200 * 6 / 1024;
   printf("volt is %d\r\n",dat);
}
void test(void)
{
  //
}
void test2()
{}
void hahah()
{}
void main()
{
    nrf_gpio_cfg_output(19);
   // simple_uart_config(8, 9, 10, 11, false);
    simple_uart_config(8, 23, 10, 24, false);
    //adc_init();
//    bsp_InitSFlash();
//    uint32_t dat = 0;
//    uint8_t data[TEST_SIZE] ={0};
//    for(int i = 0; i < TEST_SIZE; i++)
//    {
//      data[i] = 0xaa;
//    }
//    uint8_t buf[TEST_SIZE]; 
//    dat = sf_ReadID();
//    printf("ID 0x%x\r\n",dat);
////    sf_EraseSector(0x00);
////    printf("EraseSector successful..\r\n");
//    SPI_FLASH_BufferWrite(data, 0x1000, TEST_SIZE);
//    sf_ReadBuffer(buf, 0x1000, TEST_SIZE);
//    for(int i = 0; i < TEST_SIZE; i++)
//    {
//      printf("read %d 0x%02X\r\n",i,buf[i]);
//    }
//
//    
//    for(int i = 0; i < TEST_SIZE; i++)
//    {
//      data[i] = 0x55;
//    }
//    SPI_FLASH_BufferWrite(data, 0x1000, TEST_SIZE);
//    sf_ReadBuffer(buf, 0x1000, TEST_SIZE);
//    for(int i = 0; i < TEST_SIZE; i++)
//    printf("read1 %d data is 0x%02X\r\n",i,buf[i]);
    while(1)
    { 
        nrf_gpio_pin_toggle(19);
        //get_adc();
        nrf_delay_ms(300);   
        printf("hi...\r\n");
    }
}