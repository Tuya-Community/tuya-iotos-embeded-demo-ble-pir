# Tuya IoTOS Embeded Demo BLE pir

[English](./README.md) | [中文](./README_zh.md) 



## 简介 

本 Demo 基于涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded Ble SDK，使用涂鸦BLE系列模组快速组建一个pir传感器的 demo，具有检测人体存在、手机app远程设置灵敏度和警报持续时间的功能。



## 快速上手 

### 开发环境搭建 

+ IDE 根据芯片原厂 SDK 要求进行安装。

+ Tuya BLE SDK Demo Project 下载地址见下表所致。请参考各分支下的 `README.md` 文件完成工程导入。

  |   芯片平台   |   型号   |                           下载地址                           |
  | :----------: | :------: | :----------------------------------------------------------: |
  |    Nordic    | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
  |   Realtek    | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
  |    Telink    | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
  | Silicon Labs |   BG21   |                     正在规划中，敬请期待                     |
  |    Beken     | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
  |    Beken     |  BK3432  | [ tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
  |   Cypress    |  Psoc63  | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |



### 编译烧录

+ 工程导入

  本 demo 拉取到本地后，将 `tuya_ble_app` 文件夹和本地环境中的`telink_kite_ble_sdk_v3.4.0_20190816/ble_sdk_multimode/tuya_ble_app` 文件夹中的代码文件进行替换，即可编译，烧录，连接APP。
  
+ 代码修改

  1. 在 `tuya_ble_app_demo.h` 填入在涂鸦 IoT 工作台创建的智能产品 PID。
     ```c
     #define APP_PRODUCT_ID     "xxxxxxxx"
     ```
     将其中 `xxxxxxxx` 替换为PID。
  2. 在 `tuya_ble_app_demo.c` 填入申请的授权码（uuid 和 auth key）。
     ```c
     static const char auth_key_test[] = "yyyyyyyy";
     static const char device_id_test[] = "zzzzzzzz";
     ```
      其中 `yyyyyyyy` 替换为 auth key， `zzzzzzzz` 替换为 uuid.
  
+ 编译运行Demo代码

  修改好后编译代码，并下载固件至硬件运行（根据所选的芯片型号有可能还需要下载 stack 和 bootloader），观察 log 日志 ，并用第三方蓝牙调试 App（例如 IOS 下的 lightBlue）扫描确认设备有没有正常广播。



### 文件介绍 

|              文件名              |                            功能                             |
| :------------------------------: | :---------------------------------------------------------: |
|       tuya_ble_app_demo.c        | application 示例主文件，存放 SDK 初始化函数、消息处理函数等 |
|       tuya_ble_app_demo.h        |                           头文件                            |
|        tuya_ble_app_ota.c        |                        OTA 相关代码                         |
|        tuya_ble_app_ota.h        |                           头文件                            |
|    custom_app_product_test.c     |                   自定义产测项目相关实现                    |
|    custom_app_product_test.h     |                           头文件                            |
| custom_app_uart_common_handler.c |           UART 通用对接实现代码文件，不使用可忽略           |
| custom_app_uart_common_handler.h |                           头文件                            |
|     custom_tuya_ble_config.h     |                    application 配置文件                     |



### 应用入口

入口文件：/tuya_ble_app/tuya_ble_app_demo.c

+ `void tuya_ble_app_init(void)` 对Tuya IoTOS Embeded Ble SDK 进行一些必要的初始化，该函数只执行一次。
+ `void app_exe()` 该函数用来执行用户应用代码，该函数循环执行。



### dp点相关 

|  函数名  | tuya_ble_dp_data_report                                      |
| :------: | :----------------------------------------------------------- |
| 函数原型 | tuya_ble_status_t tuya_ble_dp_data_report(uint8_t *p_data,uint32_t len); |
| 功能概述 | 上报dp点数据。                                               |
|   参数   | p_data [in] : dp点数据。len[in] : 数据长度，最大不能超过`TUYA_BLE_REPORT_MAX_DP_DATA_LEN`。 |
|  返回值  | TUYA_BLE_SUCCESS ：发送成功；<br/>TUYA_BLE_ERR_INVALID_PARAM：参数无效；<br/>TUYA_BLE_ERR_INVALID_STATE：当前状态不支持发送，例如蓝牙断开；<br/>TUYA_BLE_ERR_NO_MEM：内存申请失败；<br/>TUYA_BLE_ERR_INVALID_LENGTH：数据长度错误 ;<br/>TUYA_BLE_ERR_NO_EVENT：其他错误。 |
|   备注   | Application通过调用该函数上报 DP 点数据到 手机app。          |

参数说明：

涂鸦云平台是以 dp 点的方式管理数据，任何设备产生的数据都需要抽象为 dp 点的形式，一个完整的dp点数据由四部分组成（具体参考 IoT 工作台上的相关介绍）：

Dp_id： 1个字节，在开发平台注册的 dp_id 序号。

Dp_type：1 个字节，dp点类型。

​	`#define DT_RAW 0` raw类型；

​	`#define DT_BOOL 1` 布尔类型；

​	`#define DT_VALUE 2` 数值类型，其范围在iot平台注册时指定；

​	`#define DT_STRING 3` 字符串类型；

​	`#define DT_ENUM 4` 枚举类型；

​	`#define DT_BITMAP 5` 位映射类型；

Dp_len：1 个字节或者两个字节，目前蓝牙仅支持一个字节，即单个 dp 点数据最长 255 个字节。

Dp_data：数据，dp_len 个字节。



该 dp 点上报函数的参数 p_data 指向的数据必须以下表格式组装上报：

| Dp点1的数据 | ~       |        |         |      | Dp点n的数据 |         |        |         |
| :---------- | :------ | :----- | :------ | :--- | :---------- | :------ | :----- | :------ |
| 1           | 2       | 3      | 4~      | ~    | n           | n+1     | n+2    | n+3~    |
| Dp_id       | Dp_type | Dp_len | Dp_data | ~    | Dp_id       | Dp_type | Dp_len | Dp_data |

调用该函数时，参数 len 的最大长度为 `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`（当前为255+3）。



## 相关文档 

+ [BLE SDK 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D) 
+ [BLE SDK Demo 说明](https://developer.tuya.com/cn/docs/iot/device-development/embedded-software-development/module-sdk-development-access/ble-chip-sdk/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o) 
+ [涂鸦 Demo 中心](https://developer.tuya.com/demo)  



## 技术支持 

您可以通过以下方法获得涂鸦的支持:

+ 开发者中心：https://developer.tuya.com
+ 帮助中心: https://support.tuya.com/help
+ 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/) 

