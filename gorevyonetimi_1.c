#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);


void Task1( void *pvParameters);
void Task2( void *pvParameters);


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();


static  const char *veri1 = "merhaba1\r\n";
static  const char *veri2 = "merhaba2\r\n";

xTaskCreate(Task1, "Task1",configMINIMAL_STACK_SIZE , (void*)veri1, 1, NULL);
xTaskCreate(Task2, "Task2",configMINIMAL_STACK_SIZE , (void*)veri2, 2, NULL);
//xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
vTaskStartScheduler();
  while (1){

  }

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
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




void Task1( void *pvParameters){

char *Gorevadi;
const TickType_t msgecikme250 = pdMS_TO_TICKS(250);

Gorevadi=( char * )pvParameters;

for(;;){
		  HAL_UART_Transmit(&huart2, (uint8_t *)Gorevadi, 10, 0xFFFF);
		//  for(volatile uint32_t i=0; i<100000; i++);
		  vTaskDelay(msgecikme250);
	}
}




void Task2( void *pvParameters){
char *Gorevadi;
const TickType_t msgecikme250 = pdMS_TO_TICKS(250);

Gorevadi=( char * )pvParameters;

for(;;){
		HAL_UART_Transmit(&huart2, (uint8_t *)Gorevadi, 10, 0xFFFF);
		//for(volatile uint32_t i=0; i<100000; i++);
		 vTaskDelay(msgecikme250); // bununla birlikte biz kodu beklemekten ziyade kapatıyoruz kapalı kaldığı durumda direk düşük öncelikli göreve geçmiş oluyor

		/* FreeRTOS'ta bekleme için for döngüsü kullanmak doğru bir yaklaşım değildir. Görevlerin uyuması veya belirli bir süre boyunca beklemesi için FreeRTOS,
			for döngüsü, CPU'yu sürekli olarak işgal eder ve diğer görevlerin çalışmasına engel olabilir. Bu durum, genellikle gerçek zamanlı sistemlerde istenmeyen bir durumdur
			 çünkü diğer görevlerin zamanında çalışmasını engelleyebilir ve sistem performansını olumsuz etkileyebilir.
		 */
		}
}









void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
