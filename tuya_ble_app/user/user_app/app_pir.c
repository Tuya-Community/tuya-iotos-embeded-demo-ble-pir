#include "app_pir.h"

#define SENSOR_IO                      GPIO_PD2
#define SEN_3                          GPIO_PB4
#define SEN_2                          GPIO_PA0
#define SEN_1                          GPIO_PC2
#define OT_3                           GPIO_PC0
#define OT_2                           GPIO_PD7
#define OT_1                           GPIO_PB5

#define time_ms                       (1000)
unsigned long time_tick = 0;

APP_CTRL_DATA_T app_ctrl_data = {0};

void app_init(void)
{
	gpio_set_func(SENSOR_IO ,AS_GPIO);
	gpio_set_func(SEN_3 ,AS_GPIO);
	gpio_set_func(SEN_2 ,AS_GPIO);
	gpio_set_func(SEN_1 ,AS_GPIO);
	gpio_set_func(OT_3 ,AS_GPIO);
	gpio_set_func(OT_2 ,AS_GPIO);
	gpio_set_func(OT_1 ,AS_GPIO);
	gpio_set_input_en(SENSOR_IO, 1);
	gpio_set_input_en(SEN_3, 1);
	gpio_set_input_en(SEN_2, 1);
	gpio_set_input_en(SEN_1, 1);
	gpio_set_input_en(OT_3, 1);
	gpio_set_input_en(OT_2, 1);
	gpio_set_input_en(OT_1, 1);
}


void app_set_sensitivity(SENSITIVITY sens)
{
    switch (sens)
    {
    case sensitivity_3m:
        gpio_write(SEN_3, 1);
        gpio_write(SEN_2, 0);
        gpio_write(SEN_1, 0);
        break;
    case sensitivity_5m:
    	gpio_write(SEN_3, 0);
    	gpio_write(SEN_2, 1);
    	gpio_write(SEN_1, 0);
        break;
    case sensitivity_7m:
    	gpio_write(SEN_3, 0);
    	gpio_write(SEN_2, 0);
    	gpio_write(SEN_1, 1);
        break;
    default:
        break;
    }
}

void app_set_ontime(ONTIME ontime)
{
    switch (ontime)
    {
    case ontime_30s:
    	gpio_write(OT_3, 0);
    	gpio_write(OT_2, 0);
    	gpio_write(OT_1, 1);
        break;
    case ontime_60s:
    	gpio_write(OT_3, 0);
    	gpio_write(OT_2, 1);
    	gpio_write(OT_1, 0);
        break;
    case ontime_120s:
    	gpio_write(OT_3, 1);
    	gpio_write(OT_2, 0);
    	gpio_write(OT_1, 0);
        break;
    default:
        break;
    }
}

void app_ctrl_handle(void)
{
	if(!clock_time_exceed(time_tick, time_ms*1000)) {
		return ;
	}
	time_tick = clock_time();

    TUYA_APP_LOG_INFO("-----enter user app handle-------\n");

    if(gpio_read(SENSOR_IO) == 0) {
    	app_ctrl_data.sensor_state = none;
    }else {
    	app_ctrl_data.sensor_state = presence;
    }

    app_set_sensitivity(app_ctrl_data.sens);
    app_set_ontime(app_ctrl_data.ontime);

    uint8_t dp_value = 0;

    dp_value = app_ctrl_data.sensor_state;
    app_dp_report(DP_SENSOR_STATE, &dp_value, Enum);

}

void app_dp_report(uint8_t dp_id, uint8_t *dp_value, DP_TYPE_T dp_type)
{
	uint8_t dp_buf[50] = {0};
    uint8_t dp_len = 0;
	dp_buf[0] = dp_id;
	dp_buf[1] = dp_type;

	switch(dp_type) {
	case Raw:

		break;
	case Bool:
		dp_len = 5;
	    dp_buf[2] = 0x00;
	    dp_buf[3] = 0x01;
	    dp_buf[4] = *dp_value;
		break;
	case Value:
		dp_len = 8;
	    dp_buf[2] = 0x00;
	    dp_buf[3] = 0x04;
	    dp_buf[4] = (uint8_t)(*dp_value >> 24);
	    dp_buf[5] = (uint8_t)(*dp_value >> 16);
	    dp_buf[6] = (uint8_t)(*dp_value >> 8);
	    dp_buf[7] = (uint8_t)*dp_value;
		break;
	case String:
		break;
	case Enum:
		dp_len = 5;
	    dp_buf[2] = 0x00;
	    dp_buf[3] = 0x01;
	    dp_buf[4] = *dp_value;
		break;
	default:
		break;
	}
    tuya_ble_dp_data_report(dp_buf,dp_len);
}

void app_dp_handle(uint8_t *dp_data)
{
	if((dp_data[0] == DP_SENSOR_SENS)&&(dp_data[1] == Enum)){
		app_ctrl_data.sens = dp_data[3];
	}
	if((dp_data[0] == DP_SENSOR_ONTIME)&&(dp_data[1] == Enum)){
		app_ctrl_data.ontime = dp_data[3];
	}
}
