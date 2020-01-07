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

#define MIDI_SENDER_QUEUE_LEN 10

class MidiSender {
public:
	MidiSender(_MIDI_sendByte sendFunc);
	virtual ~MidiSender();

	bool sendMidi(char b1, char b2, char b3);
	void tick();
private:
	struct MidiEvent
	{
		char buf[3];
	};

	_MIDI_sendByte sendFunc;
	MidiEvent eventQueue[MIDI_SENDER_QUEUE_LEN];
	uint32_t writePos;
	uint32_t readPos;
	uint32_t fifoLen;
	uint32_t currentByte;
};

#endif /* MIDISENDER_H_ */
