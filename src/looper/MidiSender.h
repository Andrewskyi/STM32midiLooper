/*
 * MidiSender.h
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

#ifndef MIDISENDER_H_
#define MIDISENDER_H_

#include "TxFifo.h"

#define MidiSender_BUFFER_LENGTH 32

class MidiSender : public TxFifo
{
public:
	MidiSender(TxFifo_sendByte sendFunc);
	virtual ~MidiSender();

	bool sendMidi(const char* buf, uint32_t length);
	bool sendMidi(char b1, char b2, char b3);
	bool sendRealTimeMidi(char b);
private:
	char buffer[MidiSender_BUFFER_LENGTH];
};

#endif /* MIDISENDER_H_ */
