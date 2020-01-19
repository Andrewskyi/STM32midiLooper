/*
 * MidiSender.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiSender.h"

MidiSender::MidiSender(TxFifo_sendByte sendFunc) :
	TxFifo(buffer, MidiSender_BUFFER_LENGTH, sendFunc)
{

}

MidiSender::~MidiSender()
{
}

bool MidiSender::sendMidi(const char* buf, uint32_t length)
{

	//printf("%u,%u\r\n", fifoLen, length);

	return send(buf, length);
}

bool MidiSender::sendRealTimeMidi(char b)
{
	return sendFunc(b);
}

bool MidiSender::sendMidi(char b1, char b2, char b3)
{
	char buf2[3] = { b1, b2, b3 };

	return sendMidi(buf2, 3);
}
