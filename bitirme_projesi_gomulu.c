
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "HX711_2.h"
#include "HX711_2_1.h"
#include "hx711.h"


#define QUEUE_LENGTH  10
#define DATA_LENGTH   1

float weight;

typedef enum {
eUsartData,
eUsartData2,
eSensorData,
eSensorData1
} DataSource_t;

typedef struct {
 DataSource_t source;
 int32_t data;
} DataItem_t;

hx711_t loadcell;
TaskHandle_t xTask2Handle = NULL;
QueueHandle_t xQueue;
QueueHandle_t xQueue1;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void vSenderTask( void *pvParameters);
static void vReceiverTask( void *pvParameters);
static void HX711_2( void *pvParameters);
static void HX711( void *pvParameters);
static void vReceiverTask1( void *pvParameters);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  power_down();
  hx711_init(&loadcell, HX711_CLK_GPIO_Port, HX711_CLK_Pin, HX711_DATA_GPIO_Port, HX711_DATA_Pin);
  hx711_coef_set(&loadcell, 354.5);
  hx711_tare(&loadcell, 10);
  power_up();
  set_scale(1.f);
  begin(128);
  leddriver_init();
  HAL_UART_Transmit(&huart2, (uint8_t *)"UART BASLADI\r\n", 14, 0xFFFF);
  xQueue = xQueueCreate(1, sizeof(DataItem_t));
  xQueue1 = xQueueCreate(1, sizeof(DataItem_t));

	if( xQueue != NULL )
	{

		xTaskCreate( vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
		xTaskCreate( HX711, "HX711", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
		xTaskCreate( HX711_2, "HX711_2", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
		xTaskCreate( vReceiverTask, "Receiver", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
		xTaskCreate( vReceiverTask1, "Receiver1", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
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

void leddriver_init(void)
{
	 RCC->AHB1ENR |= (1<<3);

	 GPIOD->MODER |= (1<<16);
	 GPIOD->MODER &= ~(1<<17);
	 GPIOD->MODER |= (1<<18);
	 GPIOD->MODER &= ~(1<<19);
	 GPIOD->MODER |= (1<<20);
	 GPIOD->MODER &= ~(1<<21);
	 GPIOD->MODER |= (1<<22);
	 GPIOD->MODER &= ~(1<<23);

	 GPIOD->OTYPER &= ~(1<<8);
	 GPIOD->OTYPER &= ~(1<<9);
	 GPIOD->OTYPER &= ~(1<<10);
	 GPIOD->OTYPER &= ~(1<<11);
	 GPIOD->OSPEEDR |= (16<<0);
	 GPIOD->OSPEEDR |= (17<<0);
	 GPIOD->OSPEEDR |= (18<<0);
	 GPIOD->OSPEEDR |= (19<<0);
	 GPIOD->OSPEEDR |= (20<<0);
	 GPIOD->OSPEEDR |= (21<<0);
	 GPIOD->OSPEEDR |= (22<<0);
	 GPIOD->OSPEEDR |= (23<<0);






	 GPIOD->MODER |= (1<<24);
	 GPIOD->MODER &= ~(1<<25);
	 GPIOD->MODER |= (1<<26);
	 GPIOD->MODER &= ~(1<<27);
	 GPIOD->MODER |= (1<<28);
	 GPIOD->MODER &= ~(1<<29);
	 GPIOD->MODER |= (1<<30);
	 GPIOD->MODER &= ~(1<<31);

	 GPIOD->OTYPER &= ~(1<<12);
	 GPIOD->OTYPER &= ~(1<<13);
	 GPIOD->OTYPER &= ~(1<<14);
	 GPIOD->OTYPER &= ~(1<<15);
	 GPIOD->OSPEEDR |= (24<<0);
	 GPIOD->OSPEEDR |= (25<<0);
	 GPIOD->OSPEEDR |= (26<<0);
	 GPIOD->OSPEEDR |= (27<<0);
	 GPIOD->OSPEEDR |= (28<<0);
	 GPIOD->OSPEEDR |= (29<<0);
	 GPIOD->OSPEEDR |= (30<<0);
	 GPIOD->OSPEEDR |= (31<<0);
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();


  HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET);


  HAL_GPIO_WritePin(HX711_CLK_GPIO_Port, HX711_CLK_Pin, GPIO_PIN_RESET);


  GPIO_InitStruct.Pin = SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SCK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DT_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = HX711_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HX711_CLK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = HX711_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HX711_DATA_GPIO_Port, &GPIO_InitStruct);


}

static void vSenderTask(void *pvParameters) {
    char data[DATA_LENGTH];

    HAL_StatusTypeDef status;

    while (1) {
        status = HAL_UART_Receive(&huart2, (uint8_t *)data, DATA_LENGTH, HAL_MAX_DELAY);

        if (status == HAL_OK) {
            int32_t intData = atoi(data);
            DataItem_t item = { eUsartData, intData };
            DataItem_t item2 = { eUsartData2, intData };
            xQueueSend(xQueue, &item, portMAX_DELAY);
            xQueueSend(xQueue1, &item2, portMAX_DELAY);
        }
    }
}

static void HX711( void *pvParameters){
    int32_t sensor;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(400);
    char buffer[20];

	{
	    char data[DATA_LENGTH];

	    HAL_StatusTypeDef status;

	    while (1) {
	            sensor = get_units(2);
	            sensor = sensor /20;
	            sensor = sensor *0.0942;

	            DataItem_t item = { eSensorData, sensor };
	            xQueueSend(xQueue, &item, portMAX_DELAY);
	        }
	}
}

static void HX711_2( void *pvParameters){

    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(300);
    char buffer[20];
    int32_t weight;

	{
	    char data[DATA_LENGTH];

	    HAL_StatusTypeDef status;

	    while (1) {
	    		weight = hx711_weight(&loadcell, 1);
	    		weight= weight/5.6;

	            DataItem_t item = { eSensorData1, weight };
	            xQueueSend(xQueue1, &item, portMAX_DELAY);
	        }
	}
}

static void vReceiverTask1(void *pvParameters) {
    DataItem_t receivedItem;
    BaseType_t xStatus;
    int32_t lReceivedValue;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(150);
    char buffer[20];

    for (;;) {
    	  xStatus = xQueueReceive(xQueue1, &receivedItem, xTicksToWait);
    	  if (xStatus == pdPASS) {
    	  if (receivedItem.source == eUsartData2) {
    	                  sprintf(buffer, "USART222 Received: %d \r\n", receivedItem.data);
    	                  vTaskDelay(pdMS_TO_TICKS(100));
    	                  /*HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    	                  if (receivedItem.data==1){
    	                  	leddriver1_on();
    	                  }
    	                  if (receivedItem.data==2){
    	                             	leddriver1_off();
    	                             }*/
    	                  if ( receivedItem.data==1){
    	                	for (int i = 0; i < 100; i++) {
    	                	 GPIOD->ODR |= (1<<12);
    	                	 GPIOD->ODR &= ~(1<<13);
    	                	 GPIOD->ODR &= ~(1<<14);
    	                	 GPIOD->ODR &= ~(1<<15);
    	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	 GPIOD->ODR &= ~(1<<12);
    	                	 GPIOD->ODR |= (1<<13);
    	                	 GPIOD->ODR &= ~(1<<14);
    	                	 GPIOD->ODR &= ~(1<<15);
    	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	 GPIOD->ODR &= ~(1<<12);
    	                	 GPIOD->ODR &= ~(1<<13);
    	                	 GPIOD->ODR |= (1<<14);
    	                	 GPIOD->ODR &= ~(1<<15);
    	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	 GPIOD->ODR &= ~(1<<12);
    	                	 GPIOD->ODR &= ~(1<<13);
    	                	 GPIOD->ODR &= ~(1<<14);
    	                	 GPIOD->ODR |= (1<<15);
    	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	}

    	                	  }


    	  }
    	  	  	  	  	  else if (receivedItem.source == eSensorData1) {
    	                  sprintf(buffer, "------: %d\r\n", receivedItem.data);
    	                  HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    	                  if(receivedItem.data <-500){
    	                	 for (int i = 0; i < 100; i++) {
    	                	 GPIOD->ODR &= ~(1<<12);;
     	                	 GPIOD->ODR &= ~(1<<13);
     	                	 GPIOD->ODR &= ~(1<<14);
     	                	 GPIOD->ODR |= (1<<15);
     	                	 vTaskDelay(pdMS_TO_TICKS(2));
     	                	 GPIOD->ODR &= ~(1<<12);
     	                	 GPIOD->ODR &= ~(1<<13);
     	                	 GPIOD->ODR |= (1<<14);
     	                	 GPIOD->ODR &= ~(1<<15);
     	                	 vTaskDelay(pdMS_TO_TICKS(2));
     	                	 GPIOD->ODR &= ~(1<<12);
     	                	 GPIOD->ODR |= (1<<13);
     	                	 GPIOD->ODR &= ~(1<<14);;
     	                	 GPIOD->ODR &= ~(1<<15);;
     	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	 GPIOD->ODR |= (1<<12);
    	                	 GPIOD->ODR &= ~(1<<13);
    	                	 GPIOD->ODR &= ~(1<<14);
    	                	 GPIOD->ODR &= ~(1<<15);
     	                	 vTaskDelay(pdMS_TO_TICKS(2));
    	                	 }

    	                  }

    	              }
    	          }
    }
}
static void vReceiverTask(void *pvParameters) {
    DataItem_t receivedItem;
    BaseType_t xStatus;
    int32_t lReceivedValue;

    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
    char buffer[20];

    for (;;) {
        xStatus = xQueueReceive(xQueue, &receivedItem, xTicksToWait);
        if (xStatus == pdPASS) {
            if (receivedItem.source == eUsartData) {
                sprintf(buffer, "USART Received: %d \r\n", receivedItem.data);
                HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
                if (receivedItem.data==3){
                	for (int i = 0; i < 100; i++) {
                		GPIOD->ODR |= (1<<8);
                		GPIOD->ODR &= ~(1<<9);
                		GPIOD->ODR &= ~(1<<10);
                		GPIOD->ODR &= ~(1<<11);
                		vTaskDelay(pdMS_TO_TICKS(2));
                		GPIOD->ODR &= ~(1<<8);
                		GPIOD->ODR |= (1<<9);
                		GPIOD->ODR &= ~(1<<10);
                		GPIOD->ODR &= ~(1<<11);
                		vTaskDelay(pdMS_TO_TICKS(2));
                		GPIOD->ODR &= ~(1<<8);
                		GPIOD->ODR &= ~(1<<9);
                		GPIOD->ODR |= (1<<10);
                		GPIOD->ODR &= ~(1<<11);
                		vTaskDelay(pdMS_TO_TICKS(2));
                		GPIOD->ODR &= ~(1<<8);
                		GPIOD->ODR &= ~(1<<9);
                		GPIOD->ODR &= ~(1<<10);
                		GPIOD->ODR |= (1<<11);
                		vTaskDelay(pdMS_TO_TICKS(2));
                	}
                }

            }   else if (receivedItem.source == eSensorData) {
                sprintf(buffer, " 1111111111: %d\r\n", receivedItem.data);
                HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
                if (receivedItem.data<-500){
                	 for (int i = 0; i < 100; i++) {
                		 GPIOD->ODR &= ~(1<<8);;
                		 GPIOD->ODR &= ~(1<<9);
                		 GPIOD->ODR &= ~(1<<10);
                		 GPIOD->ODR |= (1<<11);
                		 vTaskDelay(pdMS_TO_TICKS(2));
                		 GPIOD->ODR &= ~(1<<8);
                		 GPIOD->ODR &= ~(1<<9);
                		 GPIOD->ODR |= (1<<10);
                		 GPIOD->ODR &= ~(1<<11);
                		 vTaskDelay(pdMS_TO_TICKS(2));
                		 GPIOD->ODR &= ~(1<<8);
                		 GPIOD->ODR |= (1<<9);
                		 GPIOD->ODR &= ~(1<<10);;
                		 GPIOD->ODR &= ~(1<<11);;
                		 vTaskDelay(pdMS_TO_TICKS(2));
                		 GPIOD->ODR |= (1<<8);
                		 GPIOD->ODR &= ~(1<<9);
                		 GPIOD->ODR &= ~(1<<10);
                		 GPIOD->ODR &= ~(1<<11);
                		 vTaskDelay(pdMS_TO_TICKS(2));
                	 }

                }
            }
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
