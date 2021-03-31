//#include "app_pir_light.h"
#include "../drivers/8258/adc.h"
#define LUX_ADC_PIN GPIO_PC4

//===================================battery party===============================================
void adc_gpio_ain_init(void)
{
	//set misc channel en,  and adc state machine state cnt 2( "set" stage and "capture" state for misc channel)
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel
	//set "capture state" length for misc channel: 240
	//set "set state" length for misc channel: 10
	//adc state machine  period  = 24M/250 = 96K, T = 10.4 uS
	adc_set_state_length(240, 0, 10);  	//set R_max_mc,R_max_c,R_max_s
	//set misc channel use differential_mode (telink advice: only differential mode is available)
	//single mode adc source, PB4 for example: PB4 positive channel, GND negative channel
	gpio_set_func(LUX_ADC_PIN, AS_GPIO);
	gpio_setup_up_down_resistor(LUX_ADC_PIN, PM_PIN_UP_DOWN_FLOAT);
	gpio_set_input_en(LUX_ADC_PIN, 0);
	gpio_set_output_en(LUX_ADC_PIN, 0);
	gpio_write(LUX_ADC_PIN, 0);
//#if ((PIR_LIGHT_SEL != PIR_LIGHT_1WAY)&&(PIR_LIGHT_SEL != PIR_LIGHT_2WAY))
//	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B1P, GND);
//#else
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, C4P, GND);
//#endif
	//set misc channel resolution 14 bit
	//notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
	adc_set_resolution(ADC_MISC_CHN, RES14);  //set resolution
	//set misc channel vref 1.2V
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);  					//set channel Vref
	//set misc t_sample 6 cycle of adc clockADC_VREF_VBAT_N:  6 * 1/4M
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);  	//Number of ADC clock cycles in sampling phase

	//set Analog input pre-scaling 1/8
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
}

void adc_vbat_detect_init(void)
{
	//set misc channel en,  and adc state machine state cnt 2( "set" stage and "capture" state for misc channel)
	adc_set_chn_enable_and_max_state_cnt(ADC_MISC_CHN, 2);  	//set total length for sampling state machine and channel
	//set "capture state" length for misc channel: 240
	//set "set state" length for misc channel: 10
	//adc state machine  period  = 24M/250 = 96K, T = 10.4 uS
	adc_set_state_length(240, 0, 10);  	//set R_max_mc,R_max_c,R_max_s
	//telink advice: you must choose one gpio with adc function to output high level(voltage will equal to vbat), then use adc to measure high level voltage
	gpio_set_func(GPIO_PB0, AS_GPIO);
	gpio_set_input_en(GPIO_PB0, 0);
	gpio_set_output_en(GPIO_PB0, 1);
	gpio_write(GPIO_PB0, 1);
	//set misc channel use differential_mode(telink advice: only differential mode is available)
	adc_set_ain_channel_differential_mode(ADC_MISC_CHN, B0P, GND);
	//set misc channel resolution 14 bit
	//notice that: in differential_mode MSB is sign bit, rest are data,  here BIT(13) is sign bit
	adc_set_resolution(ADC_MISC_CHN, RES14);   //set resolution
	//set misc channel vref 1.2V
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_1P2V);  					//set channel Vref
	//set misc t_sample 6 cycle of adc clock:  6 * 1/4M
	adc_set_tsample_cycle(ADC_MISC_CHN, SAMPLING_CYCLES_6);  	//Number of ADC clock cycles in sampling phase
	//set Analog input pre-scaling 1/8
	adc_set_ain_pre_scaler(ADC_PRESCALER_1F8);
}

void lux_adc_init(void)
{
    ////Step 1: power off sar adc/////////////////////////////////////////////////////////
	/******power off sar adc********/
	adc_power_on_sar_adc(0);
////Step 2: Config some common adc settings(user can not change these)/////////////////
	/******enable signal of 24M clock to sar adc********/
	adc_enable_clk_24m_to_sar_adc(1);
	/******set adc sample clk as 4MHz******/
	adc_set_sample_clk(5); //adc sample clk= 24M/(1+5)=4M
	/******set adc L R channel Gain Stage bias current trimming******/
	adc_set_left_gain_bias(GAIN_STAGE_BIAS_PER100);
	adc_set_right_gain_bias(GAIN_STAGE_BIAS_PER100);
////Step 3: Config adc settings  as needed /////////////////////////////////////////////
#if 1//(TEST_ADC_SELECT == TEST_ADC_GPIO)
	adc_gpio_ain_init();
#else// (TEST_ADC_SELECT == TEST_ADC_VBAT)
	adc_vbat_detect_init();
#endif
////Step 4: power on sar adc/////////////////////////////////////////////////////////
	/******power on sar adc********/
	adc_power_on_sar_adc(1);
////////////////////////////////////////////////////////////////////////////////////////
}



