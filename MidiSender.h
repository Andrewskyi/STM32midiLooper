/*
 * MidiSender.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#ifndef MIDISENDER_H_
#define MIDISENDER_H_

#include <stdint.h>

typedef bool (*_MIDI_sendByte)(char byte);

#define MIDI_SENDER_QUEUE_LEN 30

class MidiSender {
public:
	MidiSender(_MIDI_sendByte sendFunc);
	virtual ~MidiSender();

	bool sendMidi(char*const buf, uint32_t length);
	bool sendRealTimeMidi(char b);
	bool sendMidi(char b1, char b2, char b3);
	void tick();
private:
	_MIDI_sendByte sendFunc;
	char bytesQueue[MIDI_SENDER_QUEUE_LEN];
	uint32_t writePos;
	uint32_t readPos;
	uint32_t fifoLen;
};

#endif /* MIDISENDER_H_ */
