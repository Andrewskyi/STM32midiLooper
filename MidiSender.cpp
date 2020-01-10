/*
 * MidiSender.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiSender.h"

MidiSender::MidiSender(_MIDI_sendByte sendFunc) :
	sendFunc(sendFunc), writePos(0), readPos(0), fifoLen(0)
{

}

MidiSender::~MidiSender()
{
}

bool MidiSender::sendMidi(char*const buf, uint32_t length)
{
	if((fifoLen + length) <= MIDI_SENDER_QUEUE_LEN)
	{
		for(uint32_t i=0; i<length; i++)
		{
			bytesQueue[writePos] = buf[i];

			if(writePos < (MIDI_SENDER_QUEUE_LEN - 1))
			{
				writePos++;
			}
			else
			{
				writePos = 0;
			}
		}

		fifoLen += length;
		tick();

		return true;
	}

	return false;
}

bool MidiSender::sendRealTimeMidi(char b)
{
	return sendFunc(b);
}

bool MidiSender::sendMidi(char b1, char b2, char b3)
{
	char buf2[3] = {b1, b2, b3};

	return sendMidi(buf2, 3);
}

void MidiSender::tick()
{
	if(fifoLen == 0)
	{
		return;
	}

	if(sendFunc(bytesQueue[readPos]))
	{
		if(readPos < (MIDI_SENDER_QUEUE_LEN - 1))
		{
			readPos++;
		}
		else
		{
			readPos = 0;
		}

		fifoLen--;
	}
}
