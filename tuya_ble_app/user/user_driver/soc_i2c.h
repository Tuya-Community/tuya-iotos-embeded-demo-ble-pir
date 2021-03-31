#ifndef __SOC_I2C_H__
#define __SOC_I2C_H__

#include "tuya_ble_common.h"
#include "tuya_ble_log.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief IIC gpio structure
 */
typedef struct
{
    uint8_t ucSDA_IO;
    uint8_t ucSCL_IO;
}i2c_pin_t;

/**
 * @berief: SOC i2c init
 * @param {i2c_pin_t i2c_pin --> i2c init gpio struct}
 * @return: none
 * @retval: none
 */
void opSocI2CInit(i2c_pin_t *i2c_pin);

/**
 * @berief: SOC i2c SDA reset proc
 * @param {none}
 * @return: none
 * @retval: none
 */
void vI2CSDAReset(void);

/**
 * @berief: SOC i2c SDA set proc
 * @param {none}
 * @return: none
 * @retval: none
 */
void vI2CSDASet(void);

/**
 * @berief: SOC i2c SCL set proc
 * @param {none}
 * @return: OPERATE_RET
 * @retval: none
 */
void vI2CSCLReset(void);

/**
 * @berief: SOC i2c SCL set proc
 * @param {none}
 * @return: none
 * @retval: none
 */
void vI2CSCLSet(void);

void vI2CSCLRelease(void);

void vI2CSDARelease(void);
/**
 * @berief: SOC i2c SDA(output) read
 * @param {none}
 * @return: OPERATE_RET
 * @retval: none
 */
uint8_t ucI2CSDAOutputRead(void);

/**
 * @berief: SOC i2c SDA(input) read
 * @param {none}
 * @return: OPERATE_RET
 * @retval: none
 */
uint8_t ucI2CSDAInputRead(void);

/**
 * @berief: SOC i2c SCL(input) read
 * @param {none}
 * @return: OPERATE_RET
 * @retval: none
 */
uint8_t ucI2CSCLInputRead(void);

/**
 * @berief: SOC i2c delay
 * @param {none}
 * @attention: delay time need to adapter according to system
 *              e.g:
 *                  usDelayTime = usDelayTime * 2;
 *                  usDelayTime = usDelayTime * 0.6;
 * @retval: none
 */
void vI2CDelay(uint16_t usDelayTime);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_I2C_H__ */
