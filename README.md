# Tuya IoTOS Embeded Demo BLE pir

[English](./README.md) | [中文](./README_zh.md) 



## Introduction 

Based on the Tuya Smart Cloud Platform, Tuya Smart APP, IoTOS Embeded Ble SDK, this demo uses Tuya BLE series modules to quickly build a pir sensor demo, which has the functions of detecting the presence of a human body and remotely setting the sensitivity and alarm duration of the mobile app.



## Quick start 

### Development Environment Setup 

+ The IDE is installed according to the original chip SDK requirements.

+ The Tuya BLE SDK Demo Project is available for download at the following address. Please refer to the `README.md` file under each branch to complete the project import.

| Chip Platform |  Model   |                       Download Address                       |
| :-----------: | :------: | :----------------------------------------------------------: |
|    Nordic     | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
|    Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
|    Telink     | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
| Silicon Labs  |   BG21   |              Planning is underway, stay tuned!               |
|     Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
|     Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
|    Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |



### Compile & Burn

+ Project import

  After pulling this demo locally, replace the `tuya_ble_app` folder with the code files in the `telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/tuya_ble_app` folder in your local environment to compile, burn and connect the app.
  
+ Code modification

  1. Fill in `tuya_ble_app_demo.h` with the PID of the smart product created in the Tuya IoT workbench.
     ```c
     #define APP_PRODUCT_ID "xxxxxxxxx"
     ```
     Replace `xxxxxxxx` with the PID.

  2. Fill in the requested authorization code (uuid and auth key) in `tuya_ble_app_demo.c`.
     ```c
     static const char auth_key_test[] = "yyyyyyyyyyy";
     static const char device_id_test[] = "zzzzzzzzzzz";
     ```
      where `yyyyyyyy` is replaced by auth key and `zzzzzzzz` is replaced by uuid.
  
+ Compile and run the demo code

  Compile the code after modification, and download the firmware to the hardware to run (depending on the selected chip model, you may also need to download the stack and bootloader), observe the log logs, and use a third-party Bluetooth debugging app (such as lightBlue under IOS) to scan the device to confirm whether it is broadcasting properly.



### File description 

|             Filename             |                           Function                           |
| :------------------------------: | :----------------------------------------------------------: |
|       tuya_ble_app_demo.c        | The application example master file, which holds the SDK initialization functions, message handling functions, etc. |
|       tuya_ble_app_demo.h        |                           Headers                            |
|        tuya_ble_app_ota.c        |                      OTA Related Codes                       |
|        tuya_ble_app_ota.h        |                           Headers                            |
|    custom_app_product_test.c     | Customization of the implementation of the project related to production testing |
|    custom_app_product_test.h     |                           Headers                            |
| custom_app_uart_common_handler.c | UART generic docking implementation code file, not used can be ignored |
| custom_app_uart_common_handler.h |                           Headers                            |
|     custom_tuya_ble_config.h     |                application configuration file                |



### Application entry

Entry file: /tuya_ble_app/tuya_ble_app_demo.c

+ `void tuya_ble_app_init(void)` Perform some necessary initialization of the Tuya IoTOS Embeded Ble SDK, this function is executed only once.
+ `void app_exe()` This function is used to execute the user application code, this function executes in a loop.



### About dp points

|   Function Name    | tuya_ble_dp_data_report                                      |
| :----------------: | :----------------------------------------------------------- |
| Function Prototype | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
| Function Overview  | Report dp point data.                                        |
|     Parameters     | p_data [in] : dp point data. len[in] : data length, the maximum cannot exceed `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`. |
|       Return       | TUYA_BLE_SUCCESS : Sending success;<br/>TUYA_BLE_ERR_INVALID_PARAM : Invalid parameter;<br/>TUYA_BLE_ERR_INVALID_STATE : Current status does not support sending, such as Bluetooth disconnect;<br/>TUYA_BLE_ERR_NO_ MEM: memory request failed;<br/>TUYA_BLE_ERR_INVALID_LENGTH: data length error ;<br/>TUYA_BLE_ERR_NO_EVENT: other errors. |
|       Notes        | The application reports the DP point data to the mobile app by calling this function. |

Parameter Description.

The Tuya Cloud Platform manages data in the form of dp points. Any data generated by the device needs to be abstracted in the form of dp points. A complete dp point data consists of four parts (refer to the relevant introduction on the IoT workbench for details).

Dp_id: 1 byte, the dp_id serial number registered in the development platform.

Dp_type: 1 byte, dp point type.

`#define DT_RAW 0` raw type.

`#define DT_BOOL 1` Boolean type.

`#define DT_VALUE 2` numeric type, whose range is specified at iot platform registration.

`#define DT_STRING 3` string type.

`#define DT_ENUM 4` enumerated type.

`#define DT_BITMAP 5` bitmap type.

Dp_len: 1 byte or two bytes, currently Bluetooth only supports one byte, i.e. a maximum of 255 bytes of data for a single dp point.

Dp_data: data, dp_len bytes.



The data referred to by the argument p_data of this dp point reporting function must be assembled and reported in the following table format.

| Data of Dp point 1 | ~       |        |         |      | Data of Dp point n |         |        |         |
| :----------------- | :------ | :----- | :------ | :--- | :----------------- | :------ | :----- | :------ |
| 1                  | 2       | 3      | 4~      | ~    | n                  | n+1     | n+2    | n+3~    |
| Dp_id              | Dp_type | Dp_len | Dp_data | ~    | Dp_id              | Dp_type | Dp_len | Dp_data |

When this function is called, the maximum length of the parameter len is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN` (currently 255+3).



## Related Documents 

+ [BLE SDK Description](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D) 

+ [BLE SDK Demo Description](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o) 
+ [Tuya Demo Center](https://developer.tuya.com/demo)  



## Technical Support 

You can get support for Tuya in the following ways:

+ Developer Center：https://developer.tuya.com
+ Help Center https://support.tuya.com/help
+ Technical Support Work Order Center: [https://service.console.tuya.com](https://service.console.tuya.com/) 