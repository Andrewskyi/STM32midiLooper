/*
 * MidiSender.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiSender.h"

MidiSender::MidiSender(_MIDI_sendByte sendFunc) :
	sendFunc(sendFunc), writePos(0), readPos(0), fifoLen(0), currentByte(0)
{

}

MidiSender::~MidiSender()
{
}

bool MidiSender::sendMidi(char b1, char b2, char b3)
{
	if(fifoLen < MIDI_SENDER_QUEUE_LEN)
	{
		MidiEvent& ev = eventQueue[writePos];

		ev.buf[0] = b1;
		ev.buf[1] = b2;
		ev.buf[2] = b3;

		if(writePos < (MIDI_SENDER_QUEUE_LEN - 1))
		{
			writePos++;
		}
		else
		{
			writePos = 0;
		}

		fifoLen++;
		tick();

		return true;
	}

	return false;
}

void MidiSender::tick()
{
	if(fifoLen == 0)
	{
		return;
	}

	if(currentByte < 3)
	{
		MidiEvent& ev = eventQueue[readPos];

		if(sendFunc(ev.buf[currentByte]))
		{
			currentByte++;
		}
	}

	if(currentByte == 3)
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
		currentByte = 0;
	}
}
