/*
 * MidiReceiver.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#ifndef MIDIRECEIVER_H_
#define MIDIRECEIVER_H_

#include <stdint.h>
#include "MidiSender.h"

typedef bool (*_MIDI_recByte)(char& b);

class MidiReceiver {
public:
	MidiReceiver(_MIDI_recByte recFunc, MidiSender* midiThru);
	virtual ~MidiReceiver();

	bool nextEvent(char& b1, char& b2, char& b3);
	void tick();
private:
	_MIDI_recByte recFunc;
    MidiSender* midiThru;
	volatile char runningStatusByte;
	volatile uint32_t expectedBytesCount;
	char buf[3];
	volatile uint32_t bytesCount;
	volatile char systemRealTimeEvent;

	uint32_t expectedChannelVoiceMsgBytesCount();
};

#endif /* MIDIRECEIVER_H_ */
