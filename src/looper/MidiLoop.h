/*
 * MidiLoop.h
 *
 *  Created on: 03.01.2020
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

#ifndef MIDILOOP_H_
#define MIDILOOP_H_

#include <stdint.h>
#include "MidiSender.h"
#include "MidiEventBuffer.h"

typedef bool (*_MIDI_sendByte)(char byte);

class MidiLoop {
public:
	enum State {
		RECORDING = 0, PLAYING
	};

	const State& currentState;
	volatile const uint32_t& currentTime;
	volatile const uint32_t& loopEndTime;
	const bool& isSecondHalf;
	const bool& loopFull;
	volatile const bool& stateChangeRequested;

	MidiLoop(MidiEventBuffer& buf, MidiSender& sender,
			MidiLoop const * const masterPtr=nullptr);
	~MidiLoop();

	uint32_t midiTimeTick();
	void beginLoop();
	void endLoop();
	void midiEvent(char b1, char b2, char b3);
	void tick();

private:
	MidiEventBuffer& buf;
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
	volatile uint32_t firstEventTime;
	volatile uint32_t firstEventMasterLoop;

	void slaveMidiTimeTick();
	void play();
	void shiftMasterTime(uint32_t& masterTime);
};

#endif /* MIDILOOP_H_ */
