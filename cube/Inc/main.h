/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define EL_HI_Pin GPIO_PIN_13
#define EL_HI_GPIO_Port GPIOC
#define EL_LOW_Pin GPIO_PIN_14
#define EL_LOW_GPIO_Port GPIOC
#define OUT_1_Pin GPIO_PIN_0
#define OUT_1_GPIO_Port GPIOC
#define OUT_2_Pin GPIO_PIN_1
#define OUT_2_GPIO_Port GPIOC
#define SPI1_CS_N_Pin GPIO_PIN_4
#define SPI1_CS_N_GPIO_Port GPIOA
#define FLASH_WP_N_Pin GPIO_PIN_4
#define FLASH_WP_N_GPIO_Port GPIOC
#define FLASH_HOLD_N_Pin GPIO_PIN_5
#define FLASH_HOLD_N_GPIO_Port GPIOC
#define REL_1_Pin GPIO_PIN_1
#define REL_1_GPIO_Port GPIOB
#define REL_2_Pin GPIO_PIN_2
#define REL_2_GPIO_Port GPIOB
#define REL_3_Pin GPIO_PIN_10
#define REL_3_GPIO_Port GPIOB
#define REL_4_Pin GPIO_PIN_11
#define REL_4_GPIO_Port GPIOB
#define REL_5_Pin GPIO_PIN_12
#define REL_5_GPIO_Port GPIOB
#define REL_6_Pin GPIO_PIN_13
#define REL_6_GPIO_Port GPIOB
#define REL_7_Pin GPIO_PIN_14
#define REL_7_GPIO_Port GPIOB
#define REL_8_Pin GPIO_PIN_15
#define REL_8_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_6
#define LED_G_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_7
#define LED_R_GPIO_Port GPIOC
#define SW_4_Pin GPIO_PIN_8
#define SW_4_GPIO_Port GPIOA
#define SW_3_Pin GPIO_PIN_9
#define SW_3_GPIO_Port GPIOA
#define SW_2_Pin GPIO_PIN_10
#define SW_2_GPIO_Port GPIOA
#define SW_1_Pin GPIO_PIN_11
#define SW_1_GPIO_Port GPIOA
#define USART_4_DIR_Pin GPIO_PIN_15
#define USART_4_DIR_GPIO_Port GPIOA
#define USART_5_DIR_Pin GPIO_PIN_3
#define USART_5_DIR_GPIO_Port GPIOB
#define USART_1_DIR_Pin GPIO_PIN_5
#define USART_1_DIR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
