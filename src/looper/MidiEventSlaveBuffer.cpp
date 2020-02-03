/*
 * MidiEventSlaveBuffer.cpp
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

#include "MidiEventSlaveBuffer.h"
#include <stdio.h>

MidiEventSlaveBuffer::MidiEventSlaveBuffer(uint8_t* const buf, uint32_t length) :
	buf(buf), length(length), idx(0),
	statusByte(0), currentTime(0), currentLoop(0), currentVariant(0)
{

}

MidiEventSlaveBuffer::~MidiEventSlaveBuffer()
{

}

void MidiEventSlaveBuffer::nextEventToWrite()
{
	increment();
}

void MidiEventSlaveBuffer::nextEventToRead()
{
	increment();

	if(buf[idx] > 0x7F)
	{
	    //printf("nextEventToRead(): STATUS, idx=%u, v=%u\n", idx, currentVariant);
		const auto inEvent = (MidiEventStorage*)(buf + idx);
		assign(event, *inEvent);
		currentVariant = inEvent->variant;
		statusByte = event.b1;
	}
	else
	{
		//printf("nextEventToRead(): NO_STATUS, idx=%u, v=%u\n", idx, currentVariant);
		const auto inEvent = (MidiEventWithoutStatusStorage*)(buf + idx);
		assign(event, *inEvent);
		currentVariant = inEvent->variant;
		event.b1 = statusByte;
		idx--;
	}

	switch(currentVariant)
	{
	case 0:
		currentTime += ( (S_V00*)(buf + idx + 3) )->time;
		break;
	case 1:
	    currentLoop += ( (S_V01*)(buf + idx + 3) )->loop;
		currentTime += ( (S_V01*)(buf + idx + 3) )->time;
		break;
	case 2:
	    currentLoop += ( (S_V10*)(buf + idx + 3) )->loop;
        currentTime += ( (S_V10*)(buf + idx + 3) )->time;
        break;
	default:
	    currentLoop += ( (S_V11*)(buf + idx + 3) )->loop;
		currentTime += ( (S_V11*)(buf + idx + 3) )->time;
		break;
	}

	//printf("slave: v=%u\r\n", currentVariant);

	event.loopIdx = currentLoop;
	event.time = currentTime;

	//printf("nextEventToRead()2: idx=%u, v=%u\n", idx, currentVariant);
}

void MidiEventSlaveBuffer::reset()
{
	idx = 0;
	eof = false;
	assign(event, *((MidiEventStorage*)buf) );
	statusByte = event.b1;
	event.loopIdx = ( (S_V11*)(buf + 3) )->loop;
	event.time = ( (S_V11*)(buf + 3) )->time;
	currentTime = event.time;
	currentLoop = event.loopIdx;
	currentVariant = 3;
}

void MidiEventSlaveBuffer::saveEvent()
{
	//printf("saveEvent()1: idx=%u, v=%u\n", idx, currentVariant);

	if(idx == 0)
	{
		statusByte = event.b1;
		currentTime = event.time;
		currentLoop = event.loopIdx;
	}

	int32_t deltaTime = (idx==0 ? event.time : (event.time - currentTime));
	uint32_t deltaLoop = (idx==0 ? event.loopIdx : (event.loopIdx - currentLoop));
	currentVariant = getVariant(deltaTime, deltaLoop);

	if(statusByte == event.b1 && idx != 0)
	{
	    MidiEventWithoutStatusStorage& outEvent = *( (MidiEventWithoutStatusStorage*)(buf + idx) );

		assign(outEvent, event);
		outEvent.variant = currentVariant;
		idx--;
	}
	else
	{
		MidiEventStorage& outEvent = *((MidiEventStorage*)(buf + idx));

		assign(outEvent, event);
		outEvent.variant = currentVariant;
		statusByte = event.b1;
	}

	//printf("write time: idx=%u, v=%u, time=%u\n", idx + 3, currentVariant, deltaTime);

	switch(currentVariant)
	{
	case 0:
		( (S_V00*)(buf + idx + 3) )->time = deltaTime;
		break;
	case 1:
		( (S_V01*)(buf + idx + 3) )->loop = deltaLoop;
		( (S_V01*)(buf + idx + 3) )->time = deltaTime;
		break;
	case 2:
        ( (S_V10*)(buf + idx + 3) )->loop = deltaLoop;
        ( (S_V10*)(buf + idx + 3) )->time = deltaTime;
        break;
	default:
		( (S_V11*)(buf + idx + 3) )->loop = deltaLoop;
		( (S_V11*)(buf + idx + 3) )->time = deltaTime;
		break;
	}

	currentTime = event.time;
	currentLoop = event.loopIdx;

	//printf("saveEvent()2: idx=%u, v=%u\n\n", idx, currentVariant);
}

void MidiEventSlaveBuffer::increment()
{
	if(!eof && (idx + sizeof(MidiEventStorage) + 4) < length)
	{
		switch(currentVariant)
		{
		case 0:
			idx += sizeof(MidiEventStorage);
			break;
		case 1:
			idx += sizeof(MidiEventStorage) + 1;
			break;
		case 2:
            idx += sizeof(MidiEventStorage) + 2;
            break;
		default:
			idx += sizeof(MidiEventStorage) + 4;
			break;
		}

		if((idx + sizeof(MidiEventStorage) + 4) >= length)
		{
			eof = true;
		}
	}
}

uint8_t MidiEventSlaveBuffer::getVariant(int32_t deltaTime, uint32_t deltaLoop)
{
    uint8_t timeVariant = timeToVariant(deltaTime);
    uint8_t loopVariant = loopToVariant(deltaLoop);

    return (timeVariant > loopVariant) ? timeVariant : loopVariant;
}

uint8_t MidiEventSlaveBuffer::timeToVariant(int32_t deltaTime)
{
	if(idx == 0)
	{
		return 3;
	}
	else if(deltaTime >= 0 && deltaTime <= int32_t(0x7F))
	{
		return 0;
	}
	else if(deltaTime >= int32_t(~0xFFF) && deltaTime <= int32_t(0xFFF))
	{
		return 1;
	}
	else if(deltaTime >= int32_t(~0x1FFFF) && deltaTime <= int32_t(0x1FFFF))
    {
        return 2;
    }
	else
	{
		return 3;
	}
}

uint8_t MidiEventSlaveBuffer::loopToVariant(uint32_t deltaLoop)
{
    if(idx == 0)
    {
        return 3;
    }
    else if(deltaLoop == 0)
    {
        return 0;
    }
    else if(deltaLoop <= 3) // 2b
    {
        return 1;
    }
    else if(deltaLoop <= 31) // 5b
    {
        return 2;
    }
    else
    {
        return 3;
    }
}


