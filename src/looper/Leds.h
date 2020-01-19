/*
 * Leds.h
 *
 *  Created on: 18.01.2020
 *      Author: apaluch
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
