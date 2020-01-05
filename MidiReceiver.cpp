/*
 * MidiReceiver.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiReceiver.h"

MidiReceiver::MidiReceiver(_MIDI_recByte recFunc) :
	recFunc(recFunc), bytesCount(0)
{
}

MidiReceiver::~MidiReceiver()
{
}

bool MidiReceiver::nextEvent(char& b1, char& b2, char& b3)
{
	if(recFunc(buf[bytesCount]))
	{
		bytesCount++;

		if(bytesCount==1 && (buf[0]&0xF0) != 0x90 &&
				(buf[0]&0xF0) != 0x80)
		{
			bytesCount = 0;
		}
		else if(bytesCount>1 && buf[bytesCount-1] > 127)
		{
			bytesCount = 0;
		}
		else if(bytesCount==3)
		{
			b1 = buf[0];
			b2 = buf[1];
			b3 = buf[2];

			bytesCount = 0;

			return true;
		}
	}

	return false;
}
