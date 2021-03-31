#ifndef __APP_PIR_H__
#define __APP_PIR_H__

#include "tuya_ble_common.h"
#include "tuya_ble_log.h"

#define DP_SENSOR_SENS          (101)
#define DP_SENSOR_ONTIME        (102)
#define DP_SENSOR_STATE         (103)
typedef enum {
    sensitivity_3m = 0,
    sensitivity_5m,
    sensitivity_7m,
}SENSITIVITY;

typedef enum {
    ontime_30s = 0,
    ontime_60s,
    ontime_120s,
}ONTIME;

typedef enum {
    none = 0,
    presence,
}SENSOR_STATE;

typedef struct {
    SENSITIVITY sens;
    ONTIME ontime;
    SENSOR_STATE sensor_state;
}APP_CTRL_DATA_T;

typedef enum {
	Raw = 0,
	Bool,
	Value,
	String,
	Enum,
	Bitmap,
}DP_TYPE_T;

void app_init(void);
void app_ctrl_handle(void);
void app_dp_report(uint8_t dp_id, uint8_t *dp_value, DP_TYPE_T dp_type);
void app_dp_handle(uint8_t *dp_data);

#endif
