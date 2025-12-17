#include "INA226.h"
#include <math.h>

// 私有函数声明
static int8_t INA226_ReadRegister(INA226_HandleTypeDef *hina, uint8_t reg, uint16_t *value);
static int8_t INA226_WriteRegister(INA226_HandleTypeDef *hina, uint8_t reg, uint16_t value);
static uint16_t INA226_CalculateCalibration(float max_current, float shunt_resistor);

/**
  * @brief  初始化INA226设备
  * @param  hina: INA226句柄指针
  * @param  hi2c: I2C句柄指针
  * @param  addr: I2C设备地址
  * @param  shunt_resistor: 分流电阻值(Ω)
  * @param  max_current: 最大预期电流(A)
  * @retval 错误代码
  */
int8_t INA226_Init(INA226_HandleTypeDef *hina, I2C_HandleTypeDef *hi2c, 
                   uint8_t addr, float shunt_resistor, float max_current) {
    // 参数检查
    if (hina == NULL || hi2c == NULL) {
        return INA226_ERROR_INVALID_PARAM;
    }
    
    // 初始化句柄
    hina->hi2c = hi2c;
    hina->i2c_addr = addr;
    hina->shunt_resistor = shunt_resistor;
    
    // 计算LSB和校准值
    hina->current_lsb = max_current / 32768.0f;  // 15位有效数据
    hina->power_lsb = 25.0f * hina->current_lsb; // 功率LSB = 25 × 电流LSB
    hina->calibration_value = INA226_CalculateCalibration(max_current, shunt_resistor);
    
    // // 复位设备
    // int8_t ret = INA226_Reset(hina);
    // if (ret != INA226_OK) {
    //     return ret;
    // }
    
    // 配置校准寄存器
    INA226_SetCalibration(hina, hina->calibration_value);
    
    // 设置默认配置
    uint16_t config = INA226_DEFAULT_CONFIG;
    return INA226_SetConfig(hina, config);
}

/**
  * @brief  读取电流值
  * @param  hina: INA226句柄指针
  * @param  current: 输出的电流值(A)
  * @retval 错误代码
  */
int16_t raw_value_glo;
int8_t INA226_ReadCurrent(INA226_HandleTypeDef *hina, float *current) {
    uint16_t raw_value;
    int8_t ret = INA226_ReadRegister(hina, INA226_REG_CURRENT, &raw_value);
    INA226_ReadRegister(hina, INA226_REG_CURRENT, &raw_value_glo);
    if (ret == INA226_OK) {
        // 处理符号位（第15位）
        int16_t signed_value = (int16_t)raw_value;
        *current = signed_value * hina->current_lsb;
    }
    
    return ret;
}

/**
  * @brief  读取功率值
  * @param  hina: INA226句柄指针
  * @param  power: 输出的功率值(W)
  * @retval 错误代码
  */
int8_t INA226_ReadPower(INA226_HandleTypeDef *hina, float *power) {
    uint16_t raw_value;
    int8_t ret = INA226_ReadRegister(hina, INA226_REG_POWER, &raw_value);
    
    if (ret == INA226_OK) {
        *power = raw_value * hina->power_lsb;
    }
    
    return ret;
}

/**
  * @brief  读取总线电压
  * @param  hina: INA226句柄指针
  * @param  voltage: 输出的总线电压(V)
  * @retval 错误代码
  */
int8_t INA226_ReadBusVoltage(INA226_HandleTypeDef *hina, float *voltage) {
    uint16_t raw_value;
    int8_t ret = INA226_ReadRegister(hina, INA226_REG_BUS_VOLTAGE, &raw_value);
    
    if (ret == INA226_OK) {
        *voltage = raw_value * 0.00125f;  // LSB = 1.25mV
    }
    
    return ret;
}

/**
  * @brief  读取分流电压
  * @param  hina: INA226句柄指针
  * @param  voltage: 输出的分流电压(V)
  * @retval 错误代码
  */
uint16_t ShuntVoltage;
uint16_t ShuntVoltage_t;
int8_t INA226_ReadShuntVoltage(INA226_HandleTypeDef *hina, float *voltage) {
    uint16_t raw_value;
    int8_t ret = INA226_ReadRegister(hina, INA226_REG_SHUNT_VOLTAGE, &raw_value);
    INA226_ReadRegister(hina, INA226_REG_SHUNT_VOLTAGE, &ShuntVoltage);

    if (ret == INA226_OK) {
        // 处理有符号数（二进制补码）
        int16_t signed_value = (int16_t)raw_value;
        *voltage = signed_value * 0.0000025f;  // LSB = 2.5μV
    }

//    if(ShuntVoltage & 0x8000)
//    {
//      ShuntVoltage = ~(ShuntVoltage - 1);
//    }
    
    return ret;
}

/**
  * @brief  复位INA226设备
  * @param  hina: INA226句柄指针
  * @retval 错误代码
  */
int8_t INA226_Reset(INA226_HandleTypeDef *hina) {
    // 设置复位位（第15位）
    uint16_t current_config;
    int8_t ret;
    
    // 先读取当前配置
    ret = INA226_ReadRegister(hina, INA226_REG_CONFIG, &current_config);
    if (ret != INA226_OK) {
        return ret;
    }
    
    // 只设置RST位，保持其他位不变
    uint16_t reset_cmd = current_config | (1 << 15);
    
    // 写入修改后的值
    return INA226_WriteRegister(hina, INA226_REG_CONFIG, reset_cmd);
}

/**
  * @brief  设置配置寄存器
  * @param  hina: INA226句柄指针
  * @param  config: 配置值
  * @retval 错误代码
  */
int8_t INA226_SetConfig(INA226_HandleTypeDef *hina, uint16_t config) {
    return INA226_WriteRegister(hina, INA226_REG_CONFIG, config);
}

/**
  * @brief  设置校准寄存器
  * @param  hina: INA226句柄指针
  * @param  cal_value: 校准值
  * @retval 错误代码
  */
int8_t INA226_SetCalibration(INA226_HandleTypeDef *hina, uint16_t cal_value) {
    return INA226_WriteRegister(hina, INA226_REG_CALIBRATION, cal_value);
}

/**
  * @brief  读取制造商ID（用于设备检测）
  * @param  hina: INA226句柄指针
  * @param  manufacturer_id: 输出的制造商ID
  * @retval 错误代码
  */
int8_t INA226_ReadManufacturerID(INA226_HandleTypeDef *hina, uint16_t *manufacturer_id) {
    return INA226_ReadRegister(hina, INA226_REG_MANUFACTURER_ID, manufacturer_id);
}

// ========================= 私有函数实现 =========================

/**
  * @brief  读取16位寄存器
  */
static int8_t INA226_ReadRegister(INA226_HandleTypeDef *hina, uint8_t reg, uint16_t *value) {
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    status = HAL_I2C_Mem_Read(hina->hi2c, hina->i2c_addr, reg, 
                             I2C_MEMADD_SIZE_8BIT, data, 2, 100);
    
    if (status != HAL_OK) {
        return INA226_ERROR_I2C;
    }
    
    // 组合字节（大端格式）
    *value = (data[0] << 8) | data[1];
    return INA226_OK;
}

/**
  * @brief  写入16位寄存器
  */
static int8_t INA226_WriteRegister(INA226_HandleTypeDef *hina, uint8_t reg, uint16_t value) {
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    // 分解为字节（大端格式）
    data[0] = (value >> 8) & 0xFF;  // 高字节
    data[1] = value & 0xFF;         // 低字节
    
    status = HAL_I2C_Mem_Write(hina->hi2c, hina->i2c_addr, reg,
                              I2C_MEMADD_SIZE_8BIT, data, 2, 100);
    
    return (status == HAL_OK) ? INA226_OK : INA226_ERROR_I2C;
}

/**
  * @brief  计算校准寄存器值
  */
static uint16_t INA226_CalculateCalibration(float max_current, float shunt_resistor) {
    float current_lsb = max_current / 32768.0f;
    float cal_float = 0.00512f / (current_lsb * shunt_resistor);
    
    // 限制在校准寄存器范围内
    if (cal_float > 65535.0f) {
        cal_float = 65535.0f;
    } else if (cal_float < 0.0f) {
        cal_float = 0.0f;
    }
    
    return (uint16_t)roundf(cal_float);
}
