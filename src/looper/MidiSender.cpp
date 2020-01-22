/*
 * MidiSender.cpp
 *
 *  Created on: 04.01.2020
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
