/*
 * MidiEventMasterBuffer.cpp
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

#include "MidiEventMasterBuffer.h"
#include <stdio.h>

MidiEventMasterBuffer::MidiEventMasterBuffer(uint8_t* const buf, uint32_t length) :
	buf(buf), length(length), idx(0),
	statusByte(0), currentTime(0), currentVariant(0)
{

}

MidiEventMasterBuffer::~MidiEventMasterBuffer()
{

}

void MidiEventMasterBuffer::nextEventToWrite()
{
	increment();
}

void MidiEventMasterBuffer::nextEventToRead()
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

	uint32_t timeDelta;

	switch(currentVariant)
	{
	case 0:
	    timeDelta = ( (M_V00*)(buf + idx + 3) )->time;
		break;
	case 3:
        timeDelta = ( (M_V00*)(buf + idx + 3) )->time | 0x80;
        break;
	case 1:
	    timeDelta = ( (M_V01*)(buf + idx + 3) )->time;
		break;
	default:
	    timeDelta = ( (M_V10*)(buf + idx + 3) )->time;
		break;
	}

	//printf("master: v=%u, dt=%u\r\n", currentVariant, timeDelta);

	currentTime += timeDelta;
	event.time = currentTime;

	//printf("nextEventToRead()2: idx=%u, v=%u\n", idx, currentVariant);
}

void MidiEventMasterBuffer::reset()
{
	idx = 0;
	eof = false;
	assign(event, *((MidiEventStorage*)buf) );
	statusByte = event.b1;
	event.time = ( (M_V10*)(buf + 3) )->time;
	currentTime = event.time;
	currentVariant = 2;
}

void MidiEventMasterBuffer::saveEvent()
{
	//printf("saveEvent()1: idx=%u, v=%u\n", idx, currentVariant);

	if(idx == 0)
	{
		statusByte = event.b1;
		currentTime = event.time;
	}

	uint32_t deltaTime = (idx==0 ? event.time : (event.time - currentTime));
	currentVariant = timeToVariant(deltaTime);

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
	case 3:
		( (M_V00*)(buf + idx + 3) )->time = deltaTime & 0x7F;
		break;
	case 1:
		( (M_V01*)(buf + idx + 3) )->time = deltaTime;
		break;
	default:
		( (M_V10*)(buf + idx + 3) )->time = deltaTime;
		break;
	}

	currentTime = event.time;

	//printf("saveEvent()2: idx=%u, v=%u\n\n", idx, currentVariant);
}

void MidiEventMasterBuffer::increment()
{
	if(!eof && (idx + sizeof(MidiEventStorage) + 3) < length)
	{
		switch(currentVariant)
		{
		case 0:
		case 3:
			idx += sizeof(MidiEventStorage);
			break;
		case 1:
			idx += sizeof(MidiEventStorage) + 1;
			break;
		default:
			idx += sizeof(MidiEventStorage) + 2;
			break;
		}

		if((idx + sizeof(MidiEventStorage) + 3) >= length)
		{
			eof = true;
		}
	}
}

uint8_t MidiEventMasterBuffer::timeToVariant(uint32_t deltaTime)
{
	if(idx == 0)
	{
		return 2;
	}
	else if(deltaTime <= 0x7F)
	{
		return 0;
	}
	else if(deltaTime <= 0xFF)
    {
        return 3;
    }
	else if(deltaTime <= 0x7FFF)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}


