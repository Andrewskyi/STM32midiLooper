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
	enum State
	{
		RECORDING=0,
		PLAYING
	};

	struct MidiEvent
	{
		uint32_t b1 : 8;
		uint32_t b2 : 8;
		uint32_t b3 : 8;
		uint32_t loopIdx : 10;
		uint32_t time : 20;
	};

	const State& currentState;

	MidiLoop(MidiEvent*const buf, const uint32_t bufLength,
			MidiSender& sender, bool master);
	~MidiLoop();

	uint32_t timeTick();
	void timeTick(uint32_t masterTime);
	void beginLoop();
	void endLoop();
	void midiEvent(char b1, char b2, char b3);
	void tick();

private:
	MidiEvent*const buf;
	const uint32_t bufLength;
	volatile uint32_t currentTime;
	volatile uint32_t currentMasterLoop;
	volatile uint32_t loopEndTime;
	//volatile uint32_t masterLoopEndTime;
	volatile uint32_t masterLoopCount;
	volatile uint32_t eventCount;
	volatile uint32_t playIdx;
	volatile bool sendEventPending;
	MidiSender& sender;
	bool master;
	State state;
	volatile uint32_t noteOnOffCount;
	volatile bool beginLoopRequested;
	volatile uint32_t maxMasterTime;
	volatile int32_t deltaTime;
	volatile uint32_t lastMasterTime;

	void play();
};

#endif /* MIDILOOP_H_ */
