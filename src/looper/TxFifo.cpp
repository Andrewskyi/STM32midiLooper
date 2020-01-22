/*
 * TxFifo.cpp
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

#include "TxFifo.h"

TxFifo::TxFifo(char*const buf, const uint32_t length, TxFifo_sendByte sendFunc) :
        overflow(_overflow),
		sendFunc(sendFunc),
		queue(buf), FIFO_LENGTH(length),
		writePos(0), readPos(0), fifoLen(0) {

}

TxFifo::~TxFifo() {
}

bool TxFifo::send(const char* buf, uint32_t length) {

	if ((fifoLen + length) <= FIFO_LENGTH) {
		for (uint32_t i = 0; i < length; i++) {
			queue[writePos] = buf[i];

			if (writePos < (FIFO_LENGTH - 1)) {
				writePos++;
			} else {
				writePos = 0;
			}
		}

		fifoLen += length;
		tick();

		_overflow = false;

		return true;
	}

	_overflow = true;


	return false;
}

void TxFifo::tick() {
	if (fifoLen == 0) {
		return;
	}

	if (sendFunc(queue[readPos])) {
		if (readPos < (FIFO_LENGTH - 1)) {
			readPos++;
		} else {
			readPos = 0;
		}

		fifoLen--;
	}
}
