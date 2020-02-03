/*
 * MidiEventSlaveBuffer.h
 *
 *  Created on: 23.01.2020
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

#ifndef LOOPER_MIDIEVENTBUFFER_SLAVE_H_
#define LOOPER_MIDIEVENTBUFFER_SLAVE_H_

#include "MidiEventStorage.h"
#include "MidiEventBuffer.h"


class MidiEventSlaveBuffer : public MidiEventBuffer
{
public:
	MidiEventSlaveBuffer(uint8_t* const buf, uint32_t length);
	virtual ~MidiEventSlaveBuffer();

	void nextEventToWrite();
	void nextEventToRead();
	void reset();
	void saveEvent();
private:
	uint8_t* const buf;
	const uint32_t length;
	volatile uint32_t idx;
	volatile uint8_t statusByte;
	volatile uint32_t currentTime;
	volatile uint32_t currentLoop;
	uint32_t currentVariant;

	void increment();
	uint8_t getVariant(int32_t deltaTime, uint32_t deltaLoop);
	uint8_t timeToVariant(int32_t deltaTime);
	uint8_t loopToVariant(uint32_t deltaLoop);
};




#endif /* LOOPER_MIDIEVENTBUFFER_SLAVE_H_ */
