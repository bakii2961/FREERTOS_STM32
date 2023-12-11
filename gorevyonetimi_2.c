#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);


void vContinuousTask( void *pvParameters);
void vPeriodicTask( void *pvParameters);



int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();


static  const char *veri1 = "merhaba1\r\n";
static  const char *veri2 = "merhaba2\r\n";

xTaskCreate(vContinuousTask, "Task1",configMINIMAL_STACK_SIZE , (void*)veri1, 1, NULL);
xTaskCreate(vPeriodicTask, "Task2",configMINIMAL_STACK_SIZE , (void*)veri2, 1, NULL);
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







void vContinuousTask( void *pvParameters){
char *Gorevadi;
const TickType_t msgecikme250 = pdMS_TO_TICKS(100);
Gorevadi=( char * )pvParameters;
for(;;){
	HAL_UART_Transmit(&huart2, (uint8_t *)Gorevadi, 10, 0xFFFF);
	vTaskDelay(msgecikme250);

	/*vTaskDelay, FreeRTOS'ta görevlerin belirli bir süre boyunca beklemesini sağlayan bir fonksiyondur. Bu fonksiyon,
	görevin belirli bir süre boyunca uyumasını ve diğer görevlere CPU zaman dilimi vermesini sağlar.
	xTicksToDelay: Görevin ne kadar süre boyunca bekleyeceğini belirten bir değerdir. Bu değer, sistem zaman birimi olan
	tick cinsinden belirtilir. Örneğin, 100 tick, 1 saniyeye denk gelebilir, bu durumda pdMS_TO_TICKS(100) şeklinde zaman dilimi dönüşümü yapılabilir.
	*/
	}

}




void vPeriodicTask( void *pvParameters){
char *Gorevadi;
Gorevadi=( char * )pvParameters;
TickType_t sonUyanmazamani;
sonUyanmazamani= xTaskGetTickCount();
const TickType_t xDelay30ms = pdMS_TO_TICKS(200);
for(;;){
	HAL_UART_Transmit(&huart2, (uint8_t *)Gorevadi, 10, 0xFFFF);
	vTaskDelayUntil(&sonUyanmazamani, xDelay30ms);
	/*İKİ GÖREV ÖNCELİKLERİ AYNI OLURSA BİRİNDE UTİL DİĞERİ TASK OLURSA VE BEKLEME SÜRELERİ AYNI OLURSA ÇAKIŞMA OLUR UTİL YANİ PERİYODİK OLAN BASMA YAPAR DİĞERİ BASILAMAZ*/


	/*
	 * pxPreviousWakeTime: Görevin ne zaman uyanacağını belirlemek için kullanılan bir işaretçidir. Bu işaretçi, zamanın önceki bir değerini tutar ve fonksiyon çağrıldığında bu değer üzerine bir artış yaparak belirli bir periyodik çalışmayı sağlar.
	xTimeIncrement: Görevin her çalışması arasındaki zaman dilimini belirler. Bu değer, görevin ne zaman uyanacağını hesaplarken kullanılır.
	 */
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


