/*
 * MidiLoop.h
 *
 *  Created on: 03.01.2020
 *      Author: apaluch
 */

#ifndef MIDILOOP_H_
#define MIDILOOP_H_

#include <stdint.h>
#include "MidiSender.h"

typedef bool (*_MIDI_sendByte)(char byte);

class MidiLoop {
public:
	enum State {
		RECORDING = 0, PLAYING
	};

	struct MidiEvent {
		uint32_t b1 :8;
		uint32_t b2 :8;
		uint32_t b3 :8;
		uint32_t loopIdx :10;
		uint32_t time :20;
	};

	const State& currentState;
	volatile const uint32_t& currentTime;
	volatile const uint32_t& loopEndTime;
	const bool& isSecondHalf;
	const bool& loopFull;
	volatile const bool& stateChangeRequested;

	MidiLoop(MidiEvent* const buf, const uint32_t bufLength, MidiSender& sender,
			MidiLoop const * const masterPtr=nullptr);
	~MidiLoop();

	uint32_t midiTimeTick();
	void beginLoop();
	void endLoop();
	void midiEvent(char b1, char b2, char b3);
	void tick();

private:
	MidiEvent* const buf;
	const uint32_t bufLength;
	volatile uint32_t _currentTime;
	volatile uint32_t currentMasterLoop;
	volatile uint32_t _loopEndTime;
	volatile uint32_t loopEndTimeHalf;
	bool _isSecondHalf;
	bool _loopFull;
	volatile uint32_t masterLoopCount;
	volatile uint32_t eventCount;
	volatile uint32_t playIdx;
	volatile bool sendEventPending;
	MidiSender& sender;
	MidiLoop const * const masterPtr;
	State state;
	volatile uint32_t noteOnOffCount;
	volatile bool beginLoopRequested;
	volatile int32_t deltaTime;
	volatile uint32_t lastMasterTime;

	void slaveMidiTimeTick();
	void play();
	void shiftMasterTime(uint32_t& masterTime);
};

#endif /* MIDILOOP_H_ */
