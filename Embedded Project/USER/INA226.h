#ifndef INA226_H
#define INA226_H

#include "stm32f0xx_hal.h"
#include "i2c.h"

/* I2C地址定义 */
#define INA226_I2C_ADDR               (0x40 << 1)  // 默认地址，A1=A0=GND

/* 寄存器地址定义 */
#define INA226_REG_CONFIG             (0x00)
#define INA226_REG_SHUNT_VOLTAGE      (0x01)
#define INA226_REG_BUS_VOLTAGE        (0x02)
#define INA226_REG_POWER              (0x03)
#define INA226_REG_CURRENT            (0x04)
#define INA226_REG_CALIBRATION        (0x05)
#define INA226_REG_MASK_ENABLE        (0x06)
#define INA226_REG_ALERT_LIMIT        (0x07)
#define INA226_REG_MANUFACTURER_ID    (0xFE)
#define INA226_REG_DIE_ID             (0xFF)

/* 配置寄存器位定义 */
#define INA226_CONFIG_RST_BIT         (15)
#define INA226_CONFIG_AVG_BITS        (9)
#define INA226_CONFIG_VBUSCT_BITS     (6)
#define INA226_CONFIG_VSHCT_BITS      (3)
#define INA226_CONFIG_MODE_BITS       (0)

/* 平均模式配置 */
#define INA226_AVG_1                 (0x00)
#define INA226_AVG_4                 (0x01)
#define INA226_AVG_16                (0x02)
#define INA226_AVG_64                (0x03)
#define INA226_AVG_128               (0x04)
#define INA226_AVG_256               (0x05)
#define INA226_AVG_512               (0x06)
#define INA226_AVG_1024              (0x07)

/* 转换时间配置 */
#define INA226_CT_140US              (0x00)
#define INA226_CT_204US              (0x01)
#define INA226_CT_332US              (0x02)
#define INA226_CT_588US              (0x03)
#define INA226_CT_1100US             (0x04)
#define INA226_CT_2116US             (0x05)
#define INA226_CT_4156US             (0x06)
#define INA226_CT_8244US             (0x07)

/* 工作模式配置 */
#define INA226_MODE_POWERDOWN        (0x00)
#define INA226_MODE_SHUNT_TRIG       (0x01)
#define INA226_MODE_BUS_TRIG         (0x02)
#define INA226_MODE_SHUNT_BUS_TRIG   (0x03)
#define INA226_MODE_POWERDOWN2       (0x04)
#define INA226_MODE_SHUNT_CONT       (0x05)
#define INA226_MODE_BUS_CONT         (0x06)
#define INA226_MODE_SHUNT_BUS_CONT   (0x07)

/* 默认配置值 */
#define INA226_DEFAULT_CONFIG        (0x413F)  // 连续模式，1.1ms转换时间，不平均

/* 错误代码定义 */
#define INA226_OK                    (0)
#define INA226_ERROR_I2C             (-1)
#define INA226_ERROR_NOT_READY       (-2)
#define INA226_ERROR_INVALID_PARAM   (-3)

/* 结构体定义 */
typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t i2c_addr;
    float current_lsb;           // 电流LSB (A/bit)
    float power_lsb;             // 功率LSB (W/bit)
    float shunt_resistor;        // 分流电阻值 (Ω)
    uint16_t calibration_value; // 校准寄存器值
} INA226_HandleTypeDef;

/* 函数声明 */
int8_t INA226_Init(INA226_HandleTypeDef *hina, I2C_HandleTypeDef *hi2c, 
                   uint8_t addr, float shunt_resistor, float max_current);
int8_t INA226_ReadCurrent(INA226_HandleTypeDef *hina, float *current);
int8_t INA226_ReadPower(INA226_HandleTypeDef *hina, float *power);
int8_t INA226_ReadBusVoltage(INA226_HandleTypeDef *hina, float *voltage);
int8_t INA226_ReadShuntVoltage(INA226_HandleTypeDef *hina, float *voltage);
int8_t INA226_Reset(INA226_HandleTypeDef *hina);
int8_t INA226_SetConfig(INA226_HandleTypeDef *hina, uint16_t config);
int8_t INA226_SetCalibration(INA226_HandleTypeDef *hina, uint16_t cal_value);
int8_t INA226_ReadManufacturerID(INA226_HandleTypeDef *hina, uint16_t *manufacturer_id);

#endif /* INA226_H */
