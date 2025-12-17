/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_GIM_Pin GPIO_PIN_0
#define LED_GIM_GPIO_Port GPIOA
#define LED_CHA_Pin GPIO_PIN_1
#define LED_CHA_GPIO_Port GPIOA
#define LED_SHOOT_Pin GPIO_PIN_4
#define LED_SHOOT_GPIO_Port GPIOA
#define GIMBAL_Pin GPIO_PIN_6
#define GIMBAL_GPIO_Port GPIOA
#define SHOOT_Pin GPIO_PIN_7
#define SHOOT_GPIO_Port GPIOA
#define CHASSIS_Pin GPIO_PIN_1
#define CHASSIS_GPIO_Port GPIOB
#define LED_SYM_Pin GPIO_PIN_9
#define LED_SYM_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
