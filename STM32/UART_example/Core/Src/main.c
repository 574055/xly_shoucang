/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

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

/* USER CODE BEGIN PV */
const uint8_t RFID_cmdnub[40][26] =
  {
    {0xBB, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x7E,},       //0. Hardware version 
    {0xBB, 0x00, 0x03, 0x00, 0x01, 0x01, 0x05, 0x7E,},       //1. Software version 
    {0xBB, 0x00, 0x03, 0x00, 0x01, 0x02, 0x06, 0x7E,},       //2. manufacturers  
    {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E,},             //3. Single polling instruction 
    {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x27, 0x10, 0x83, 0x7E,}, //4. Multiple polling instructions 
    {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E,},             //5. Stop multiple polling instructions 
    { 0xBB, 0x00, 0x0C, 0x00, 0x13, 0x01, 0x00, 0x00, 0x00, 0x20,
      0x60, 0x00, 0x30, 0x75, 0x1F, 0xEB, 0x70, 0x5C, 0x59, 0x04,
      0xE3, 0xD5, 0x0D, 0x70, 0xAD, 0x7E,
    },                        //6. Set the SELECT parameter instruction 
    {0xBB, 0x00, 0x0B, 0x00, 0x00, 0x0B, 0x7E,},              //7. Get the SELECT parameter 
    {0xBB, 0x00, 0x12, 0x00, 0x01, 0x01, 0x14, 0x7E,},        //8. Set the SELECT mode 
    { 0xBB, 0x00, 0x39, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x03,
      0x00, 0x00, 0x00, 0x08, 0x4D, 0x7E,
    },                      //9. Read label data storage area 
    { 0xBB, 0x00, 0x49, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x03,
      0x00, 0x00, 0x00, 0x04, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x71, 0x7E
    },   //10. Write the label data store 
    { 0xBB, 0x00, 0x82, 0x00, 0x07, 0x00, 0x00, 0xFF,
      0xFF, 0x02, 0x00, 0x80, 0x09, 0x7E,
    },                       //11. Lock the LOCK label data store 
    { 0xBB, 0x00, 0x65, 0x00, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x67,
      0x7E,
    },                                                //12. Inactivate the kill tag 
    {0xBB, 0x00, 0x11, 0x00, 0x02, 0x00, 0xC0, 0xD3, 0x7E,}, //13. Set communication baud rate 
    {0xBB, 0x00, 0x0D, 0x00, 0x00, 0x0D, 0x7E,},            //14. Get parameters related to the Query command 
    {0xBB, 0x00, 0x0E, 0x00, 0x02, 0x10, 0x20, 0x40, 0x7E,}, //15. Set the Query parameter 
    {0xBB, 0x00, 0x07, 0x00, 0x01, 0x01, 0x09, 0x7E,},      //16. Set up work area 
    {0xBB, 0x00, 0x08, 0x00, 0x00, 0x08, 0x7E,},            //17. Acquire work locations 
    {0xBB, 0x00, 0xAB, 0x00, 0x01, 0x01, 0xAC, 0x7E,},      //18. Set up working channel 
    {0xBB, 0x00, 0xAA, 0x00, 0x00, 0xAA, 0x7E,},            //19. Get the working channel 
    {0xBB, 0x00, 0xAD, 0x00, 0x01, 0xFF, 0xAD, 0x7E,},      //20. Set to automatic frequency hopping mode
    { 0xBB, 0x00, 0xA9, 0x00, 0x06, 0x05, 0x01, 0x02,
      0x03, 0x04, 0x05, 0xC3, 0x7E,
    },                             //21. Insert the working channel 
    {0xBB, 0x00, 0xB7, 0x00, 0x00, 0xB7, 0x7E,},            //22. Acquire transmitting power 
    {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E,}, //23. Set the transmitting power 
    {0xBB, 0x00, 0xB0, 0x00, 0x01, 0xFF, 0xB0, 0x7E,},      //24. Set up transmitting continuous carrier 
    {0xBB, 0x00, 0xF1, 0x00, 0x00, 0xF1, 0x7E,},            //25. Gets the receiving demodulator parameters 
    {0xBB, 0x00, 0xF0, 0x00, 0x04, 0x03, 0x06, 0x01, 0xB0, 0xAE, 0x7E,}, //26. Set the receiving demodulator parameters 
    {0xBB, 0x00, 0xF2, 0x00, 0x00, 0xF2, 0x7E,},            //27. Test the RF input block signal 
    {0xBB, 0x00, 0xF3, 0x00, 0x00, 0xF3, 0x7E,},            //28. Test the RSSI signal at the RF input 
    {0x00},
    {0xBB, 0x00, 0x17, 0x00, 0x00, 0x17, 0x7E,},            //30. Module hibernation 
    {0xBB, 0x00, 0x1D, 0x00, 0x01, 0x02, 0x20, 0x7E,},      //31. Idle hibernation time of module
    {0xBB, 0x00, 0x04, 0x00, 0x03, 0x01, 0x01, 0x03, 0x0C, 0x7E,}, //32. The IDLE mode 
    {0xBB, 0x00, 0xE1, 0x00, 0x05, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xE4, 0x7E,}, //33.NXP G2X label supports ReadProtect/Reset ReadProtect command 
    {0xBB, 0x00, 0xE3, 0x00, 0x05, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0xE7, 0x7E,}, //34. The NXP G2X label supports the CHANGE EAS directive 
    {0xBB, 0x00, 0xE4, 0x00, 0x00, 0xE4, 0x7E,},            //35. The NXP G2X tag supports the EAS_ALARM directive 
    {0xBB, 0x00, 0xE0, 0x00, 0x06, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xE4, 0x7E,}, //36.NXP G2X label 16bits config-word 
    { 0xBB, 0x00, 0xE5, 0x00, 0x08, 0x00, 0x00, 0xFF,
      0xFF, 0x01, 0x01, 0x40, 0x00, 0x2D, 0x7E,
    },                   //37.Impinj Monza 4 Qt tags support Qt instructions 
    { 0xBB, 0x00, 0xD3, 0x00, 0x0B, 0x00, 0x00, 0xFF,
      0xFF, 0x01, 0x03, 0x00, 0x00, 0x01, 0x07, 0x00, 0xE8, 0x7E,
    },   //38.The BlockPermalock directive permanently locks blocks of a user's Block 
    {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E} // 39. multiscan x20
  };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Sendcommand(uint8_t com_nub);
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  char buffer[24];
  char msg[30];

  

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    uint8_t data[] = "HELLO WORLD123 \r\n";
    // uint8_t newline[] = "\r\n";

    Sendcommand(3);

    uint8_t arr[23] = {0};
    uint8_t i = 0, myInt = 0;
    if (HAL_UART_Receive(&huart1, buffer, 1, 1000) == HAL_OK && buffer[0] == 0xBB)
    {
      while (buffer[0] != 0x7E)
      {
        HAL_UART_Receive(&huart1, buffer, 1, 1000);
        arr[i] = buffer[0]; //add buffer char to array
        i++;
      }
      sprintf(msg, "The sum is: %d \n\r", arr);
      HAL_UART_Transmit(&huart2, arr, sizeof(arr), 100);
    }
    

    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    HAL_Delay (5000);

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Sendcommand(uint8_t com_nub)
{
  uint8_t b = 5; //start from 5, because there's no command which is shorter than 5 digits
  while (RFID_cmdnub[com_nub][b] != 0x7E)
  {
    b++;
  }
  HAL_UART_Transmit (&huart1, RFID_cmdnub[com_nub], b+1, 10);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
