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

class MidiSender {
public:
	MidiSender(_MIDI_sendByte sendFunc);
	virtual ~MidiSender();

	bool sendMidi(char b1, char b2, char b3);
	void tick();
private:
	_MIDI_sendByte sendFunc;
	char buf[3];
	uint32_t currentLen;
};

#endif /* MIDISENDER_H_ */
