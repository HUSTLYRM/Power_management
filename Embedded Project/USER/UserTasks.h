#ifndef USERTASKS_H
#define USERTASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "INA226.h"

#define POWER_MAX (50.0f)
#define BufferEnergyMax (60.0f)

// 电源序号定义
typedef enum {
    CHASSIS_Power = 0,
    SHOOT_Power = 1,
    GIMBAL_Power = 2,
    ALL_power = 3
} Power_t;

// 电源状态定义
typedef enum {
    Power_OFF = 0,
    Power_ON = 1
} PowerState_t;

int8_t INA226_Init_App(void);
void PowerLimit(void);
void ADC_Task(void const *argument);
void LED_Task(void const *argument);
void PowerStateSet(Power_t power, PowerState_t state);
void Power_Task(void const *argument);

#endif /* USERTASKS_H */
