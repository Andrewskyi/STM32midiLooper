/*
 * MidiSender.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
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
