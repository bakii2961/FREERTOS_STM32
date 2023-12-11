#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

typedef enum{

	eGonderici1, // sıralama yaptık
	eGonderici2
} VeriKaynagim_t;

typedef struct
{
	uint8_t ucValue;  // veri değişken aldık
	VeriKaynagim_t eDataSource;

} VeriYapim_t;

static VeriYapim_t xStructsToSend[ 2 ] = { // dizi içinde sıraya koyduk
		{ 100 , eGonderici1},
		{ 200 , eGonderici2}
};


TaskHandle_t xTask2Handle = NULL;

static void vSenderTask( void *pvParameters);
static void vReceiverTask( void *pvParameters);

QueueHandle_t xQueue;

int main(void)
{
    HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  HAL_UART_Transmit(&huart2, (uint8_t *)"UART BASLADI\r\n", 14, 0xFFFF);



	xQueue = xQueueCreate( 3, sizeof( VeriYapim_t ) ); // 3 elemanlı VeriYapim_t boyutunda alan aç

	if( xQueue != NULL )//kuyruk oluşursa task de oluştur deediik
	{

		xTaskCreate( vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, &(xStructsToSend[0]), 2, NULL );
		xTaskCreate( vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, &(xStructsToSend[1]), 2, NULL );
		xTaskCreate( vReceiverTask, "Receiver", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
		vTaskStartScheduler();
	}
	else
	{

		 if (HAL_UART_Transmit(&huart2, (uint8_t *)"KUYRUK BASLAMADI\n\t", 12, 10)!= HAL_OK)
		 {
			 Error_Handler();
		 }
	}

  while (1);


}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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


static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}
static void vSenderTask( void *pvParameters )
{

	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

	for( ;; )
	{

		xStatus = xQueueSendToBack( xQueue, pvParameters,xTicksToWait );// kuyruğun sonuna bizim değerimizi ekledik , "0" zamanlama

		if( xStatus != pdPASS ) ///* Kuyruk dolu olduğu  için gönderme işlemi tamamlanamadı kontrol mekanıması olarak düşün
		{



		}
	}
}
static void vReceiverTask( void *pvParameters )
{
	VeriYapim_t xReceivedStructure;
	BaseType_t xStatus;

	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
	char buffer[25];
	for( ;; )
	{

		if( uxQueueMessagesWaiting( xQueue ) != 3 )
		{
			 HAL_UART_Transmit(&huart2, (uint8_t *)"Kuyrugun Dolu Olmasi gerekir\r\n", 29, 0XFFFF);

		}

		xStatus = xQueueReceive( xQueue,&xReceivedStructure , 0 );
		if( xStatus == pdPASS )
		{
		if( xReceivedStructure.eDataSource==eGonderici1)
		{
			sprintf(buffer,"received: %d \t\n", xReceivedStructure.ucValue);
			HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 29, 0xFFFF);
			//printf(buffer);
		}
		}
		if(xReceivedStructure.eDataSource==eGonderici2)
		{	sprintf(buffer,"received: %d \t\n", xReceivedStructure.ucValue);
			HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 29, 0xFFFF);
						printf(buffer);
		}
		else {
			//printf("kuyruktan veri alınmadı");

		}




	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif


