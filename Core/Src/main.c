#include "main.h"
#include "sys.h"
#include "tim.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
//任务优先级
#define START_TASK_PRIO 1
//任务堆栈大小
#define START_STK_SIZE 256
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define INTERRUPT_TASK_PRIO 2
//任务堆栈大小
#define INTERRUPT_STK_SIZE 256
//任务句柄
TaskHandle_t INTERRUPTTask_Handler;
//任务函数
void interrupt_task(void *p_arg);

void task2_task(void *pvParameters);
void task1_task(void *pvParameters);
void start_task(void *pvParameters);
void key_task(void *pvParameters);

void SystemClock_Config(void);

extern u8 aRxBuffer[RXBUFFERSIZE]; // HAL库使用的串口接收缓冲
int main(void)
{

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_Init();
    SystemClock_Config();
    delay_init(180);
    uart_init(115200);
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);

//    while (1)
//    {
//        printf("usart receive: %s  \r\n", USART_RX_BUF);
//        //
//        delay_xms(500);
//    }

        xTaskCreate((TaskFunction_t )start_task,            //任务函数
                    (const char*    )"start_task",          //任务名称
                    (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                    (void*          )NULL,                  //传递给任务函数的参数
                    (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                    (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
        vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //进入临界区
    //创建中断测试任务
    xTaskCreate((TaskFunction_t)interrupt_task,          //任务函数
                (const char *)"interrupt_task",          //任务名称
                (uint16_t)INTERRUPT_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                            //传递给任务函数的参数
                (UBaseType_t)INTERRUPT_TASK_PRIO,        //任务优先级
                (TaskHandle_t *)&INTERRUPTTask_Handler); //任务句柄
    vTaskDelete(StartTask_Handler);                      //删除开始任务
    taskEXIT_CRITICAL();                                 //退出临界区
}

//中断测试任务函数
void interrupt_task(void *pvParameters)
{
    static u32 total_num = 0;
    while (1)
    {
        total_num += 1;
        LED0 = ~LED0;
        vTaskDelay(1000);
        printf("usart receive: %s  \r\n", USART_RX_BUF);
        if (total_num >= 5)
        {
            //添加串口打印中断关闭的提示语
            printf("中断关闭\r\n");

            // 添加 关闭中断代码。
            portDISABLE_INTERRUPTS();
            //延时5S

            delay_xms(1000);
            delay_xms(1000);

            //添加串口打印打开中断的提示语
            printf("打开中断\r\n");

            //添加中断使能代码
            portENABLE_INTERRUPTS();
        }
    }
}

