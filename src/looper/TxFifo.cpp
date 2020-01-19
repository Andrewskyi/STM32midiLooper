/*
 * TxFifo.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
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
