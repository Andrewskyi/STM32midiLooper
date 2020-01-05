/*
 * MidiSender.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiSender.h"

MidiSender::MidiSender(_MIDI_sendByte sendFunc) :
	sendFunc(sendFunc), currentLen(0)
{

}

MidiSender::~MidiSender()
{
}

bool MidiSender::sendMidi(char b1, char b2, char b3)
{
	if(currentLen > 0)
	{
		return false;
	}

	buf[2] = b1;
	buf[1] = b2;
	buf[0] = b3;
	currentLen = 3;

	tick();

	return true;
}

void MidiSender::tick()
{
	if(currentLen > 0 && sendFunc(buf[currentLen-1]))
	{
		currentLen--;
	}
}
