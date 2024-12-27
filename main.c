/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "MCP23008.h"
#include "LiquidCrystal_C.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
I2C_HandleTypeDef hi2c1;
MCP23008_HandleTypeDef hmcp;
LiquidCrystal_C lcd;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE BEGIN 2 */
  // Initialize MCP23008 and LCD
  MCP23008_Init(&hi2c1, &hmcp, 0x20); // MCP23008 at I2C address 0x20
  MCP23008_SetDirection(&hmcp, 0x00); // Set all MCP23008 pins as outputs

  LCD_Init(&lcd, &hmcp,
		  1, // four-bit mode
		  1, 255, 2, // RS=GP1, RW=invalid, EN=GP2
		  3, 4, 5, 6, // D0..D3 (these map to LCD D3..D6 in 4-bit mode)
		  0, 0, 0, 0); // D4..D7 not used in 4-bit mode
  LCD_Begin(&lcd, 16, 2, LCD_5x8DOTS); // 16x02 LCD
  LCD_SetBacklight(&lcd, true); // turn on backlight

  // Test 1: Write test
  LCD_Clear(&lcd);
  LCD_SetCursor(&lcd, 0, 0);
  LCD_WriteString(&lcd, "Hello world!");
  LCD_SetCursor(&lcd, 0, 1);
  LCD_WriteString(&lcd, "Test 1 success!");
  HAL_Delay(2000);

  // Test 2: Cursor and blink control
  LCD_Clear(&lcd);
  LCD_WriteString(&lcd, "Cursor Test");
  LCD_SetCursor(&lcd, 0, 1);
  LCD_Cursor(&lcd); // Show cursor
  HAL_Delay(2000);
  LCD_Blink(&lcd); // Blink cursor
  HAL_Delay(2000);
  LCD_NoBlink(&lcd); // Stop blinking
  HAL_Delay(1000);
  LCD_NoCursor(&lcd); // Hide cursor
  HAL_Delay(1000);
  LCD_WriteString(&lcd, "Test 2 success!");
  HAL_Delay(2000);

  // Test 3: Scrolling
  LCD_Clear(&lcd);
  LCD_WriteString(&lcd, "Scrolling test");
  LCD_SetCursor(&lcd, 0, 1);
  HAL_Delay(1000);
  for (int i=0; i<16; i++){
	  LCD_ScrollDisplayLeft(&lcd);
	  HAL_Delay(300);
  }
  for (int i=0; i<16; i++){
	  LCD_ScrollDisplayRight(&lcd);
	  HAL_Delay(300);
  }
  LCD_WriteString(&lcd, "Test 3 success!");

  // Test 4: Custom character
  uint8_t smiley[8] = {
          0x00, //  .....
          0x0A, //  .#.#.
          0x0A, //  .#.#.
          0x00, //  .....
          0x11, //  #...#
          0x0E, //  .###.
          0x00, //  .....
          0x00  //  .....
  };
  LCD_CreateChar(&lcd, 0, smiley); // Store smiley in CGRAM slot 0
  LCD_Clear(&lcd);
  LCD_WriteString(&lcd, "Custom char test");
  LCD_SetCursor(&lcd, 0, 1);
  LCD_WriteString(&lcd, "Test 4 success ");
  LCD_WriteChar(&lcd, 0); // Display the smiley
  HAL_Delay(2000);

  // Test 5: Backlight toggle
  LCD_Clear(&lcd);
  LCD_WriteString(&lcd, "Backlight test");
  for (int i=0; i<5; i++){
	  LCD_SetBacklight(&lcd, false); // Backlight off
	  HAL_Delay(300);
	  LCD_SetBacklight(&lcd, true); // Backlight on
	  HAL_Delay(300);
  }
  LCD_SetCursor(&lcd, 0, 1);
  LCD_WriteString(&lcd, "Test 5 success!");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
