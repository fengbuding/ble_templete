#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "simple_uart.h"
//#include "mx25.h"
#include "nrf_adc.h"

#define TEST_SIZE 0x1111

void branch_add()
{}
void branch1_add()
{}
void branch2_add()
{}
void branch3_add()
{}
void branch4_add()
{}
void branch6_add()
{}
void branch8_add()
{}
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
void main()
{
    nrf_gpio_cfg_output(19);
    while(1)
    { 
        nrf_gpio_pin_toggle(19);
        //get_adc();
        nrf_delay_ms(300);   
        printf("hi...\r\n");
    }
}

void branch10_add()
{}