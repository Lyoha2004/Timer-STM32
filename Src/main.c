/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "kb.h"
#include "sdk_uart.h"
#include "pca9538.h"
#include "oled.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KB_Test( void );
void OLED_KB( uint8_t OLED_Keys[]);
void oled_Reset( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  oled_Init();

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  KB_Test();
	  HAL_Delay(500);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void KB_Test( void ) {
	UART_Transmit( (uint8_t*)"Timer start\n" );
	uint8_t R = 0, C = 0, L = 0, Row[4] = {0xF7, 0x7B, 0x3D, 0x1E}, Key,
			OldKey;
	uint8_t OLED_Digits[20] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	oled_Reset();
	oled_WriteString("Input time: ", Font_7x10, White);
	OLED_PREV_TIME(OLED_Digits, 1);

	oled_UpdateScreen();

	size_t cur_dig_num = 0;
	int update_time = 0;	// 0 - запуск таймера, 1 - начинаем ввод заново
	// Пока не нажали левую нижнюю кнопку, вводим число
	while (Check_Row(Row[0]) != 0x01) {
		if (cur_dig_num < 9) {
			for (uint8_t i = 1; i < 4; i++) {
				// Вводим цифры 1 - 9
				Key = Check_Row(Row[i]);
				if (Key != 0x00) {
					if (Key == 0x04) Key = 0x03;
					OLED_Digits[cur_dig_num] = (uint8_t)((3-i)*3 + (uint8_t)(0x04-Key+0x30));
					cur_dig_num++;
					OLED_PREV_TIME(OLED_Digits, cur_dig_num);
				}
			}

			// Вводим 0
			Key = Check_Row(Row[0]);
			if (Key == 0x02) {
				OLED_Digits[cur_dig_num] = 0x30;
				cur_dig_num++;
				OLED_PREV_TIME(OLED_Digits, cur_dig_num);
			}
		}
		Key = Check_Row(Row[0]);
		if (Key == 0x04) {
			update_time = 1;
			goto reset;
		}

		HAL_Delay(25);
	}


	while (1) {
		for (int i = 0; i < 40; i++) {
			Key = Check_Row(Row[0]);
			if (Key == 0x04) {
				update_time = 1;
				goto reset;
			}
			HAL_Delay(25);
		}
		decrease_digits(OLED_Digits, &cur_dig_num);
		OLED_PREV_TIME(OLED_Digits, cur_dig_num);
	}
	reset:

//	for ( int i = 0; i < 4; i++ ) {
//		while( !( R && C && L ) ) {
//			OldKey = Key;
//			Key = Check_Row( Row[i] );
//			if ( Key == 0x01 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Right pressed\n" );
//				R = 1;
//				OLED_Keys[2+3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			} else if ( Key == 0x02 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Center pressed\n" );
//				C = 1;
//				OLED_Keys[1+3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			} else if ( Key == 0x04 && Key != OldKey) {
//				UART_Transmit( (uint8_t*)"Left pressed\n" );
//				L = 1;
//				OLED_Keys[3*i] = 0x31;
//				OLED_KB(OLED_Keys);
//			}
//		}
//		R = C = L = 0;
//		HAL_Delay(25);
//	}
	UART_Transmit( (uint8_t*)"KB test complete\n");
}

int get_n_digits(int num) {
	if (num == 0)
		return 1;
	int res = 0;
	while (num > 0) {
		res++;
		num /= 10;
	}
	return res;
}

void decrease_digits(uint8_t *OLED_Digits, size_t *length) {
	int num = get_num_from_array(OLED_Digits, *length);
	num--;
	*length = (size_t)get_n_digits(num);
	get_array_from_num(OLED_Digits, *length, num);
}

int get_num_from_array(uint8_t *OLED_Digits, size_t length) {
	int res = 0;
	for (size_t i = 0; i < length; i++) {
		res = res * 10 + (OLED_Digits[i] - 0x30);
	}
	return res;
}

void get_array_from_num(uint8_t *OLED_Digits, size_t length, int num) {
	for (size_t i = length; i > 0; i--) {
		OLED_Digits[i-1] = (num % 10) + 0x30;
		num /= 10;
	}
}

uint8_t get_digit(uint8_t row, uint8_t key) {
	return row * 3 + key;
}

void OLED_PREV_TIME(uint8_t *OLED_Digits, size_t length) {
	oled_SetCursor(1, 15);
	for (int i = 0; i < length; i++) {
		oled_WriteChar(OLED_Digits[i], Font_7x10, White);
	}
	oled_UpdateScreen();
}

void OLED_KB( uint8_t OLED_Keys[12]) {
	for (int i = 3; i >= 0; i--) {
		oled_SetCursor(56, 5+(4-i)*10);
		for (int j = 0; j < 3; j++) {
			oled_WriteChar(OLED_Keys[j+3*i], Font_7x10, White);
		}
	}
	oled_UpdateScreen();
}

void oled_Reset( void ) {
	oled_Fill(Black);
	oled_SetCursor(0, 0);
	oled_UpdateScreen();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
