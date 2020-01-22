/*
 * Leds.cpp
 *
 *  Created on: 18.01.2020
 *      Author: apaluch
 *
 *      MIT License

Copyright (c) 2020 apaluch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <looper/Leds.h>

Leds::Leds() :
	blinkCounter(0),
	slowBlinkMask(0),
	fastBlinkMask(0)
{

}

Leds::~Leds()
{

}

void Leds::init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin =
			LOOP_HALF0 | LOOP_HALF1 |
			LOOP_STATE0 | LOOP_STATE1 |
			OVERFLOW0 | OVERFLOW1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);
}

void Leds::on(uint16_t leds)
{
	GPIO_ResetBits(GPIOB, static_cast<uint16_t>(leds));
	slowBlinkMask &= ~leds;
	fastBlinkMask &= ~leds;
}

void Leds::off(uint16_t leds)
{
	GPIO_SetBits(GPIOB, static_cast<uint16_t>(leds));
	slowBlinkMask &= ~leds;
	fastBlinkMask &= ~leds;
}

void Leds::setState(uint16_t leds, bool state)
{
	if(state)
	{
		on(leds);
	}
	else
	{
		off(leds);
	}
}

void Leds::slowBlink(uint16_t leds)
{
	slowBlinkMask |= leds;
	fastBlinkMask &= ~leds;
}

void Leds::fastBlink(uint16_t leds)
{
	fastBlinkMask |= leds;
	slowBlinkMask &= ~leds;
}

void Leds::tick()
{
	if(blinkCounter == 250)
	{
		GPIO_ResetBits(GPIOB, fastBlinkMask);
	}
	else if(blinkCounter == 500)
	{
		GPIO_SetBits(GPIOB, fastBlinkMask);
		GPIO_ResetBits(GPIOB, slowBlinkMask);
	}
	else if(blinkCounter == 750)
	{
		GPIO_ResetBits(GPIOB, fastBlinkMask);
	}
	else if(blinkCounter == 1000)
	{
		GPIO_SetBits(GPIOB, fastBlinkMask);
		GPIO_SetBits(GPIOB, slowBlinkMask);
	}

	if(blinkCounter >= 1000)
	{
		blinkCounter  = 0;

	}
	else
	{
		blinkCounter ++;
	}
}
