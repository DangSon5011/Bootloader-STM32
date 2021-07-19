
#include "main.h"
#include "usart.h"
#include "gpio.h"

#define ADDRESS_NEW_START 0x08008000
#define MY_ADDRESS 0x08004000
void SystemClock_Config(void);
uint8_t rx_buff[8]; 
uint32_t data;
uint32_t address = ADDRESS_NEW_START;
uint8_t indexx=0;
void Write_flash(uint32_t* address,uint32_t *data);
uint8_t atoi8(uint8_t xx);
uint8_t state_write=0;
//uint32_t* Address_boot=(uint32_t*)0x08020000;
FLASH_EraseInitTypeDef f;
int main(void)
{
  SCB->VTOR= MY_ADDRESS;
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_FLASH_Unlock();		
	f.TypeErase= FLASH_TYPEERASE_SECTORS;
	f.Sector=2;
	f.NbSectors=1;
	f.VoltageRange=FLASH_VOLTAGE_RANGE_3;
	uint32_t sector_error=0;
	HAL_FLASHEx_Erase(&f,&sector_error);
	HAL_FLASH_Lock();
	HAL_UART_Receive_IT(&huart2,rx_buff,8);
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
		 
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);
		HAL_Delay(400);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
		HAL_Delay(400);
//		HAL_UART_Receive(&huart2,rx_buff,8,100);
		
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
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
	if(huart->Instance==huart2.Instance) 
	{
		HAL_UART_Receive_IT(&huart2,rx_buff,8);
		if(rx_buff[0]==83 && rx_buff[1]==83 && rx_buff[2]==83)
		{

			state_write=1;
		}
		else if(rx_buff[0]==80&&rx_buff[1]==80&&rx_buff[2]==80)
		{
			state_write=0;
			// them cho nhay boot
//			*Address_boot=(uint32_t)ADDRESS_NEW_START;
			HAL_FLASH_Unlock();
			
			f.TypeErase= FLASH_TYPEERASE_SECTORS;
			f.Sector=5;
			f.NbSectors=1;
			f.VoltageRange=FLASH_VOLTAGE_RANGE_3;
			uint32_t sector_error=0;
			HAL_FLASHEx_Erase(&f,&sector_error);		
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,0x08020000,(uint64_t)0x08008000);
			HAL_FLASH_Lock();
			NVIC_SystemReset();
		}
		else if(state_write==1)
		{
		data=0x00000000;
		data |= (uint32_t)((atoi8(rx_buff[0])<<4)|atoi8(rx_buff[1]));
	  data |= (uint32_t)((atoi8(rx_buff[2])<<4)|atoi8(rx_buff[3]))<<8;
		data |= (uint32_t)((atoi8(rx_buff[4])<<4)|atoi8(rx_buff[5]))<<16;
		data |= (uint32_t)((atoi8(rx_buff[6])<<4)|atoi8(rx_buff[7]))<<24;		
		Write_flash(&address,&data);				
		}
	}
}


uint8_t atoi8(uint8_t xx)
{
	if (47 <xx && xx<58)
	{
		return (xx-48);
	}
	else if( 64<xx && xx<71)
	{
			return (xx-55);
	}
	return xx;
}

void Write_flash(uint32_t* address1,uint32_t *data)
{
				HAL_FLASH_Unlock();
//			FLASH_EraseInitTypeDef f;
//			f.TypeErase= FLASH_TYPEERASE_SECTORS;
//			f.Sector= address;
//			f.NbSectors=1;
//			uint32_t sector_error=0;
//			HAL_FLASHEx_Erase(&f,&sector_error);
	
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,*address1,*data);
				*address1+=4;
			  HAL_FLASH_Lock();
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
