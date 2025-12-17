#include "UserTasks.h"

/* INA226句柄 */
INA226_HandleTypeDef hina226;

/* 测量数据存储 */
float current_value = 0.0f;
float power_value = 0.0f;
float voltage_value = 0.0f;
float voltage_bus = 0.0f;
uint32_t measurement_count = 0;

/* 测量标志位 */
volatile uint8_t measurement_flag = 0;

float Z = BufferEnergyMax; //缓冲能量
float P1 = POWER_MAX; //功率上限

/* 底盘断电标志 */
uint8_t limit_flag;

/**
 * @brief  INA226初始化
 * @retval 初始化状态
 */
int8_t INA226_Init_App(void)
{
    // 初始化参数
    uint8_t i2c_addr = INA226_I2C_ADDR; // I2C地址
    float shunt_resistor = 0.01f;        // 分流电阻值0.01Ω
    float max_current = 20.0f;           // 最大电流20A

    // 调用驱动初始化函数
    return INA226_Init(&hina226, &hi2c1, i2c_addr, shunt_resistor, max_current);
}

/**
 * @brief  功率限制函数，当缓冲能量被耗尽时，将会使底盘断电5秒
 * @retval 无
 */

void PowerLimit(void)
{
    Z = Z - (power_value - P1) * 0.1;

    if(Z <= 0.0f)
    {
        Z = 0.0f;
        limit_flag = 1;
    }
    else if(Z > BufferEnergyMax)
    {
        Z = BufferEnergyMax;
    }
}

/**
 * @brief  ADC任务，初始化INA226，以10Hz的频率读取电流和功率
 * @retval 无
 */
int8_t ret;
void ADC_Task(void const *argument)
{
    // uint32_t xLastWakeTime = osKernelSysTick();
    // const uint32_t xPeriod = 100;

    ret = INA226_Init_App();
    for (;;)
    {
        ret = INA226_ReadCurrent(&hina226, &current_value);
        if (ret != INA226_OK)
        {
            // 错误处理
        }

        ret = INA226_ReadPower(&hina226, &power_value);
        if (ret != INA226_OK)
        {
            // 错误处理
        }

        INA226_ReadShuntVoltage(&hina226, &voltage_value);

        INA226_ReadBusVoltage(&hina226, &voltage_bus);

        // 数据处理和输出
        measurement_count++;

        PowerLimit();

        // osDelayUntil(&xLastWakeTime, xPeriod);
        osDelay(100);
    }
}

/**
 * @brief  系统LED任务，让SYSTEM_LED每秒闪烁一次
 * @retval 无
 */
void LED_Task(void const *argument)
{
    // uint32_t xLastWakeTime = osKernelSysTick();
    // const uint32_t xPeriod = 500;

    for (;;)
    {
        HAL_GPIO_TogglePin(LED_SYM_GPIO_Port, LED_SYM_Pin);
        
        // osDelayUntil(&xLastWakeTime, xPeriod);
        osDelay(500);
    }
}

/**
  * @brief  设置电源通断的函数
  * @param  power: 电源序号
  * @param  state: 电源状态
  * @retval 无
  */
void PowerStateSet(Power_t power, PowerState_t state)
{
    switch (power)
    {
    case CHASSIS_Power:
        HAL_GPIO_WritePin(CHASSIS_GPIO_Port, CHASSIS_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_CHA_GPIO_Port, LED_CHA_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;

    case SHOOT_Power:
        HAL_GPIO_WritePin(SHOOT_GPIO_Port, SHOOT_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_SHOOT_GPIO_Port, LED_SHOOT_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;

    case GIMBAL_Power:
        HAL_GPIO_WritePin(GIMBAL_GPIO_Port, GIMBAL_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_GIM_GPIO_Port, LED_GIM_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;

    case ALL_power:
        HAL_GPIO_WritePin(CHASSIS_GPIO_Port, CHASSIS_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_CHA_GPIO_Port, LED_CHA_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(SHOOT_GPIO_Port, SHOOT_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_SHOOT_GPIO_Port, LED_SHOOT_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        HAL_GPIO_WritePin(GIMBAL_GPIO_Port, GIMBAL_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_GIM_GPIO_Port, LED_GIM_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
        break;
    
    default:
        break;
    }
}

/**
 * @brief  电源管理任务，负责控制三路供电的通断
 * @retval 无
 */
void Power_Task(void const *argument)
{
    PowerStateSet(ALL_power, Power_ON);
    for(;;)
    {
        if(limit_flag)
        {
            limit_flag = 0;
            PowerStateSet(CHASSIS_Power, Power_OFF);           
            osDelay(5000);
            Z = BufferEnergyMax;
            PowerStateSet(CHASSIS_Power, Power_ON);
        }
        osDelay(1);
    }
}
