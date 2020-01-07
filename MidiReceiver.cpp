/*
 * MidiReceiver.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiReceiver.h"
#include <stdio.h>

MidiReceiver::MidiReceiver(_MIDI_recByte recFunc) :
	recFunc(recFunc), runningStatusByte(buf[0]), bytesCount(0)
{
}

MidiReceiver::~MidiReceiver()
{
}

bool MidiReceiver::nextEvent(char& b1, char& b2, char& b3)
{
	char b;

	if(recFunc(b))
	{
		if((b & 0xF0) == 0x90 || (b & 0xF0) == 0x80)
		{
			runningStatusByte = b;
			bytesCount = 1;
			//printf("1\r\n");
		}
		else if((b & 0xF0) == 0xF0)
		{
			runningStatusByte = 0;
			bytesCount = 0;
			//printf("2\r\n");
		}
		else if(runningStatusByte == 0)
		{
			// simply ignore the messages
			//printf("3\r\n");
			return false;
		}
		else if(runningStatusByte != 0 && b <= 127 && bytesCount < 3)
		{
			buf[bytesCount] = b;
			//printf("4:%X\r\n", bytesCount);
			bytesCount++;
		}
		else
		{
			//printf("5\r\n");
			runningStatusByte = 0;
			bytesCount = 0;
			return false;
		}

		if(bytesCount == 3)
		{
			b1 = buf[0];
			b2 = buf[1];
			b3 = buf[2];

			bytesCount = 1;
			//printf("6\r\n");
			return true;
		}
	}

	return false;
}
