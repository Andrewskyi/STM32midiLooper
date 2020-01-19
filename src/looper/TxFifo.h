/*
 * TxFifo.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#ifndef _TX_FIFO_H_
#define _TX_FIFO_H_

#include <stdint.h>

typedef bool (*TxFifo_sendByte)(char byte);

class TxFifo {
public:
	volatile const bool& overflow;

	TxFifo(char*const buf, const uint32_t length, TxFifo_sendByte sendFunc);
	virtual ~TxFifo();

	bool send(const char* buf, uint32_t length);
	void tick();
protected:
	TxFifo_sendByte sendFunc;
private:
	char*const queue;
	const uint32_t FIFO_LENGTH;
	volatile uint32_t writePos;
	volatile uint32_t readPos;
protected:
	volatile uint32_t fifoLen;
private:
	volatile bool _overflow;
};

#endif /* _TX_FIFO_H_ */
