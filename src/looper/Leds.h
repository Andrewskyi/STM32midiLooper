/*
 * Leds.h
 *
 *  Created on: 18.01.2020
 *      Author: apaluch
 *
 *      MIT License
 *      Copyright (c) 2020 apaluch

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

#ifndef LOOPER_LEDS_H_
#define LOOPER_LEDS_H_

#include <stdint.h>
#include <stm32f10x_gpio.h>

class Leds {
public:
	enum LED : uint16_t {
		LOOP_HALF0 = GPIO_Pin_5,
		LOOP_HALF1 = GPIO_Pin_6,
		LOOP_STATE0 = GPIO_Pin_7,
		LOOP_STATE1 = GPIO_Pin_8,
		OVERFLOW0 = GPIO_Pin_10,
		OVERFLOW1 = GPIO_Pin_11
	};

	Leds();
	virtual ~Leds();

	void init();
	void on(uint16_t leds);
	void off(uint16_t leds);
	void setState(uint16_t leds, bool state);
	void slowBlink(uint16_t leds);
	void fastBlink(uint16_t leds);
	void tick();

private:
	volatile uint32_t blinkCounter;
	volatile uint16_t slowBlinkMask;
	volatile uint16_t fastBlinkMask;
};

#endif /* LOOPER_LEDS_H_ */
