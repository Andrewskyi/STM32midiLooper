#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "string.h"
#include <stdio.h>
#include <stdint.h>

#include "looper/SystemOut.h"
#include <looper/Leds.h>
#include "looper/MidiLoop.h"
#include "looper/MidiSender.h"
#include "looper/MidiReceiver.h"


void usart_init(void)
{
	/* Enable USART1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	/* Enable USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USART1 Tx (PA.09), USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10), USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = /*GPIO_Pin_10 |*/ GPIO_Pin_3;
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
	USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;

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

bool sysMsg1, sysMsg2;

extern "C" void SysTick_Handler()
{
	if(sysMsg1 != sysMsg2)
	{
		sysMsg1 = sysMsg2;
	}
}

bool usart1send(char b)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
		return false;
	}

	USART_SendData(USART1, b);

	return true;
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

#define CONDITION_CHANGED(arg) [](uint32_t value) { \
          /* special value (hope never occur)*/     \
	static uint32_t oldValue = 0xD3A4E1F2;          \
	bool changed = (oldValue != value);             \
	oldValue = value;                               \
	                                                \
	return changed;                                 \
}(arg)                                              \


#define MIDI_TIME_DIV (2000)
#define EV_LEN 400
MidiLoop::MidiEvent midiEvents[EV_LEN];
MidiLoop::MidiEvent midiEvents2[EV_LEN];

uint32_t specialKey1;
uint32_t specialKey2;

Leds leds;
MidiSender sender(sendMidi);
MidiReceiver receiver(recMidi, &sender);

MidiLoop masterLoop(midiEvents, EV_LEN, sender);
MidiLoop slaveLoop(midiEvents2, EV_LEN, sender, &masterLoop);

SystemOut sysOut(usart1send);

extern "C" void sysOutSend(char *buf, uint32_t length)
{
	sysOut.send(buf, length);
}

int main(void)
{
	// Set System clock
	SetSysClockTo72();

	// Initialize USART
	usart_init();

	// SysTick Init
	SysTick_Config(SystemCoreClock / MIDI_TIME_DIV);

    //USARTSend(" Hello.\r\nUSART1 is ready.\r\n");
	specialKey1 = 0;
    specialKey2 = 0;

    leds.init();
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

			if(specialKey1 == 0 && b3 > 0)
			{
				specialKey1 = key;
			}
			else if(specialKey2 == 0 && b3 > 0 && key != specialKey1)
			{
				specialKey2 = key;
			}
			else if((key == specialKey1) && b3 > 0 &&
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
			else if((b1 & 0xF0) == 0xB0) // control change
			{
				if(b2 == 14) // tempo change
				{
					int32_t correction = ((static_cast<int>(b3) - 63) << 4);
					uint32_t divider = MIDI_TIME_DIV + correction;
					SysTick_Config(SystemCoreClock / divider);
				}
			}
			else if(key != specialKey1 &&
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
    	receiver.tick();
    	sender.tick();
    	sysOut.tick();


    	if(sysMsg1 == sysMsg2)
		{
			sysMsg2 = !sysMsg1;

			masterLoop.midiTimeTick();
			slaveLoop.midiTimeTick();

			leds.tick();
		}

    	// loop half indication
    	if(CONDITION_CHANGED(masterLoop.isSecondHalf))
    	{
			if(!masterLoop.isSecondHalf)
			{
				leds.on(Leds::LOOP_HALF0);
				leds.off(Leds::LOOP_HALF1);
			}
			else
			{
				leds.off(Leds::LOOP_HALF0);
				leds.on(Leds::LOOP_HALF1);
			}
    	}

    	// loop overflow indication
    	if(CONDITION_CHANGED(masterLoop.loopFull || slaveLoop.loopFull))
    	{
			leds.setState(Leds::OVERFLOW0,
				masterLoop.loopFull || slaveLoop.loopFull);
    	}

    	// sender / receiver overflow indication
    	if(CONDITION_CHANGED(sender.overflow))
    	{
    		leds.setState(Leds::OVERFLOW1, sender.overflow);
    	}

    	// loop state indication
    	if(CONDITION_CHANGED(masterLoop.stateChangeRequested))
		{
			leds.fastBlink(Leds::LOOP_STATE0);
		}
		else if(CONDITION_CHANGED(slaveLoop.stateChangeRequested))
		{
			leds.fastBlink(Leds::LOOP_STATE1);
		}

    	if(specialKey1 == 0)
		{
			leds.fastBlink(Leds::LOOP_STATE0 | Leds::LOOP_STATE1);
		}
		else if(specialKey1 != 0 && specialKey2 == 0)
		{
			leds.off(Leds::LOOP_STATE0);
			leds.fastBlink(Leds::LOOP_STATE1);
		}
		else if(CONDITION_CHANGED(masterLoop.currentState) ||
				CONDITION_CHANGED(slaveLoop.currentState))
		{
			leds.setState(Leds::LOOP_STATE0,
					masterLoop.currentState == MidiLoop::RECORDING);
			leds.setState(Leds::LOOP_STATE1,
					slaveLoop.currentState == MidiLoop::RECORDING);
		}


    }
}

