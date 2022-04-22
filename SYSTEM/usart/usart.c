#include "usart.h"
#include "delay.h"
#include "main.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h" //os ʹ��
#endif


#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0)
        ; //ѭ������,ֱ���������
    USART1->DR = (u8)ch;
    return ch;
}
#endif

#if EN_USART1_RX //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
// bit15��	������ɱ�־
// bit14��	���յ�0x0d
// bit13~0��	���յ�����Ч�ֽ���Ŀ

u16 USART_RX_STA = 0; //����״̬���
u8 aRxBuffer[RXBUFFERSIZE];       // HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; // UART���.


 void uart_init(u32 bound)
{
	//UART ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1

	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������

}

// UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
// huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_Initure;

    if (huart->Instance == USART1) //����Ǵ���1�����д���1 MSP��ʼ��
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();  //ʹ��GPIOAʱ��
        __HAL_RCC_USART1_CLK_ENABLE(); //ʹ��USART1ʱ��

        GPIO_Initure.Pin = GPIO_PIN_9;             // PA9
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;       //�����������
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; //����

        HAL_GPIO_Init(GPIOA, &GPIO_Initure); //��ʼ��PA9

        GPIO_Initure.Pin = GPIO_PIN_10; // PA10
        GPIO_Initure.Mode = GPIO_MODE_INPUT;
        GPIO_Initure.Pull = GPIO_PULLUP;     //����
        HAL_GPIO_Init(GPIOA, &GPIO_Initure); //��ʼ��PA10

#if EN_USART1_RX
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn); //ʹ��USART1�ж�ͨ��
#endif
    }
}

//(u8 *)
void USART1_IRQHandler(void)
{
    u32 timeout = 0;
    u32 maxDelay = 0x1FFFF;

    HAL_UART_IRQHandler(&UART1_Handler); //����HAL���жϴ����ú���

    timeout = 0;
    while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) //�ȴ�����
    {
        timeout++; ////��ʱ����
        if (timeout > maxDelay)
            break;
    }

    timeout = 0;
    while (HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if (timeout > maxDelay)
            break;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    if (huart->Instance == USART1) //����Ǵ���1
    {

        if ((USART_RX_STA & 0x8000) == 0) //����δ���
        {
            if (USART_RX_STA & 0x4000) //���յ���0x0d
            {
                if (aRxBuffer[0] != 0x0a)
                    USART_RX_STA = 0; //���մ���,���¿�ʼ
                else
                {
                    USART_RX_STA |= 0x8000; //���������
                    for (int i = (USART_RX_STA & 0X3FFF); i < 200; i++)
                    {
                        USART_RX_BUF[i] = '\0';
                    }
                    USART_RX_STA = 0; //���մ���,���¿�ʼ

                    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
                }
            }
            else //��û�յ�0X0D
            {
                if (aRxBuffer[0] == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0];
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1))
                        USART_RX_STA = 0; //�������ݴ���,���¿�ʼ����
                }
            }
        }
    }
}

#endif
