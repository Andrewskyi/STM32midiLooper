#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "string.h"
#include <stdio.h>
#include <stdint.h>

#include "MidiLoop.h"
#include "MidiSender.h"
#include "MidiReceiver.h"

//#define RX_BUF_SIZE 80
//volatile char RX_FLAG_END_LINE = 0;
//volatile char RXi;
//volatile char RXc;
//volatile char RX_BUF[RX_BUF_SIZE] = {'\0'};
//
//void clear_RXBuffer(void) {
//	for (RXi=0; RXi<RX_BUF_SIZE; RXi++)
//		RX_BUF[RXi] = '\0';
//	RXi = 0;
//}

void usart_init(void)
{
	/* Enable USART1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//	/* NVIC Configuration */
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/* Enable the USARTx Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USART1 Tx (PA.09), USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10), USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure the USART1 */
	USART_InitTypeDef USART_InitStructure;

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
		- USART Clock disabled
		- USART CPOL: Clock is active low
		- USART CPHA: Data is captured on the middle
		- USART LastBit: The clock pulse of the last data bit is not output to
			the SCLK pin
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);

	/* MIDI UART */
	USART_InitStructure.USART_BaudRate = 31250;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);

	/* Enable USART2 */
	USART_Cmd(USART2, ENABLE);

	/* Enable the USART1 Receive interrupt: this interrupt is generated when the
		USART1 receive data register is not empty */
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

//void USART1_IRQHandler(void)
//{
//    if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
//	{
//    		RXc = USART_ReceiveData(USART1);
//    		RX_BUF[RXi] = RXc;
//    		RXi++;
//
//    		if (RXc != 13) {
//    			if (RXi > RX_BUF_SIZE-1) {
//    				clear_RXBuffer();
//    			}
//    		}
//    		else {
//    			RX_FLAG_END_LINE = 1;
//    		}
//
//			//Echo
//    		USART_SendData(USART1, RXc);
//	}
//}

extern "C" {
void USARTSend(char *pucBuffer)
{
    while (*pucBuffer)
    {
        USART_SendData(USART1, *pucBuffer++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
}
}

void SetSysClockTo72(void)
{
	ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
    	//FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(0x00010000, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */

        /* Go to infinite loop */
        while (1)
        {
        }
    }
}

#define MIDI_TIME_DIV (2000)
#define EV_LEN 400
MidiLoop::MidiEvent midiEvents[EV_LEN];
MidiLoop::MidiEvent midiEvents2[EV_LEN];
bool masterRecording = true;
bool slaveRecording = true;

uint32_t specialKey;
uint32_t specialKey2;

uint32_t blink = 0;

MidiLoop* theLoop=0;
MidiLoop* theSlaveLoop=0;
bool sysMsg1, sysMsg2;

extern "C" void SysTick_Handler()
{
//	uint8_t pinState2 = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7);
//	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)(pinState2 ? 0 : 1));

	if(sysMsg1 != sysMsg2)
	{
		sysMsg1 = sysMsg2;
	}

	blink++;

//	if(theLoop && theSlaveLoop)
//		theSlaveLoop->timeTick( theLoop->timeTick() );
}

bool sendMidi(char b)
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{
		return false;
	}

	USART_SendData(USART2, b);

	return true;
}

bool recMidi(char& b)
{
	if((USART2->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		b = USART_ReceiveData(USART2);

		return true;
	}

	return false;
}

int main(void)
{
	// Set System clock
	SetSysClockTo72();

	/* Initialize LED which connected to PC13 */
	GPIO_InitTypeDef  GPIO_InitStructure;
	// Enable PORTC Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_13); // Set C13 to Low level ("0")

    // Initialize USART
    usart_init();

//    /* Configure temp sys tick pin */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// SysTick Init
	SysTick_Config(SystemCoreClock / MIDI_TIME_DIV);

    //USARTSend(" Hello.\r\nUSART1 is ready.\r\n");
	MidiSender sender(sendMidi);
	MidiReceiver receiver(recMidi);

    MidiLoop masterLoop(midiEvents, EV_LEN, sender, true);
    MidiLoop slaveLoop(midiEvents2, EV_LEN, sender, false);
    theLoop = &masterLoop;
    theSlaveLoop = &slaveLoop;

    masterRecording = true;
    slaveRecording = false;
    specialKey = 0;
    specialKey2 = 0;

    masterLoop.endLoop();
    slaveLoop.endLoop();

    while (1)
    {
    	char b1;
		char b2;
		char b3;
		uint32_t key;

		// receive MIDI events
    	if (receiver.nextEvent(b1, b2, b3))
		{
    		key = (b1<<8)|b2;

				if(specialKey == 0 && b3 > 0)
				{
					specialKey = key;
					continue;
				}

				if(specialKey2 == 0 && b3 > 0 && key != specialKey)
				{
					specialKey2 = key;
					continue;
				}

				if((key == specialKey) && b3 > 0 &&
						slaveLoop.currentState != MidiLoop::RECORDING)
				{
					if(masterLoop.currentState == MidiLoop::RECORDING)
					{
						masterLoop.endLoop();
						//printf("end master loop\r\n");
					}
					else
					{
						masterLoop.beginLoop();
						//printf("begin master loop\r\n");
					}
				}
				else if((key == specialKey2) && b3 > 0 &&
						masterLoop.currentState != MidiLoop::RECORDING)
				{
					if(slaveLoop.currentState == MidiLoop::RECORDING)
					{
						slaveLoop.endLoop();
						//printf("end slave loop\r\n");
					}
					else
					{
						slaveLoop.beginLoop();
						//printf("begin slave loop\r\n");
					}
				}
				else if(key != specialKey &&
						masterLoop.currentState == MidiLoop::RECORDING)
				{
					//printf("4:evt\r\n");
					masterLoop.midiEvent(b1, b2, b3);
				}
				else if(key != specialKey2 &&
						slaveLoop.currentState == MidiLoop::RECORDING)
				{
					//printf("4:evt\r\n");
					slaveLoop.midiEvent(b1, b2, b3);
				}
		}

    	masterLoop.tick();
    	slaveLoop.tick();
    	sender.tick();

    	if(sysMsg1 == sysMsg2)
		{
			sysMsg2 = !sysMsg1;

			if(theLoop && theSlaveLoop)
				theSlaveLoop->timeTick(
					theLoop->timeTick()
				)
				;
		}

    	// special key assignment
    	if(specialKey == 0 || specialKey2 == 0)
    	{
    		if(blink >= (specialKey == 0 ? 500 : 250))
    		{
    			uint8_t pinState = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13);
    			GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(pinState ? 0 : 1));
    			blink = 0;
    		}
    	}
    	else
    	{
    		uint8_t pinState = (masterLoop.currentState == MidiLoop::RECORDING) ||
    				(slaveLoop.currentState == MidiLoop::RECORDING);
    		GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(pinState ? 0 : 1));
    	}

    }
}

