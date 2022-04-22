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
//�������ȼ�
#define START_TASK_PRIO 1
//�����ջ��С
#define START_STK_SIZE 256
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define INTERRUPT_TASK_PRIO 2
//�����ջ��С
#define INTERRUPT_STK_SIZE 256
//������
TaskHandle_t INTERRUPTTask_Handler;
//������
void interrupt_task(void *p_arg);

void task2_task(void *pvParameters);
void task1_task(void *pvParameters);
void start_task(void *pvParameters);
void key_task(void *pvParameters);

void SystemClock_Config(void);

extern u8 aRxBuffer[RXBUFFERSIZE]; // HAL��ʹ�õĴ��ڽ��ջ���
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

        xTaskCreate((TaskFunction_t )start_task,            //������
                    (const char*    )"start_task",          //��������
                    (uint16_t       )START_STK_SIZE,        //�����ջ��С
                    (void*          )NULL,                  //���ݸ��������Ĳ���
                    (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                    (TaskHandle_t*  )&StartTask_Handler);   //������
        vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //�����ٽ���
    //�����жϲ�������
    xTaskCreate((TaskFunction_t)interrupt_task,          //������
                (const char *)"interrupt_task",          //��������
                (uint16_t)INTERRUPT_STK_SIZE,            //�����ջ��С
                (void *)NULL,                            //���ݸ��������Ĳ���
                (UBaseType_t)INTERRUPT_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&INTERRUPTTask_Handler); //������
    vTaskDelete(StartTask_Handler);                      //ɾ����ʼ����
    taskEXIT_CRITICAL();                                 //�˳��ٽ���
}

//�жϲ���������
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
            //��Ӵ��ڴ�ӡ�жϹرյ���ʾ��
            printf("�жϹر�\r\n");

            // ��� �ر��жϴ��롣
            portDISABLE_INTERRUPTS();
            //��ʱ5S

            delay_xms(1000);
            delay_xms(1000);

            //��Ӵ��ڴ�ӡ���жϵ���ʾ��
            printf("���ж�\r\n");

            //����ж�ʹ�ܴ���
            portENABLE_INTERRUPTS();
        }
    }
}

