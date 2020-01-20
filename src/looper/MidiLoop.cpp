/*
 * MidiLoop.cpp
 *
 *  Created on: 03.01.2020
 *      Author: apaluch
 */

#include "MidiLoop.h"
#include <stdio.h>

MidiLoop::MidiLoop(MidiEvent* buf, uint32_t bufLength, MidiSender& sender,
		MidiLoop const * const masterPtr) :
		currentState(state),
		currentTime(_currentTime),
		loopEndTime(_loopEndTime),
		isSecondHalf(_isSecondHalf),
		loopFull(_loopFull),
		stateChangeRequested(beginLoopRequested),
		buf(buf), bufLength(bufLength),
		_currentTime(0), currentMasterLoop(0),
		_loopEndTime(0), loopEndTimeHalf(0), _isSecondHalf(false),
		masterLoopCount(0), eventCount(0), playIdx(0), sendEventPending(false),
		sender(sender), masterPtr(masterPtr),
		state(RECORDING), noteOnOffCount(0),
		beginLoopRequested(false), deltaTime(0), lastMasterTime(0) {
	//printf("MidiLoop: buf=%u, bufLength=%u", buf, bufLength);
}

MidiLoop::~MidiLoop() {
}

uint32_t MidiLoop::midiTimeTick() {
	if(!masterPtr)
	{
		if (_currentTime < _loopEndTime) {
			_currentTime++;
			_isSecondHalf = (_currentTime >= loopEndTimeHalf);
		} else {
			_currentTime = 0;
			_isSecondHalf = false;
			playIdx = 0;
			noteOnOffCount = 0;
		}
	}
	else
	{
		slaveMidiTimeTick();
	}

	return _currentTime;
}

void MidiLoop::slaveMidiTimeTick()
{
	uint32_t masterTime = masterPtr->currentTime;

	shiftMasterTime(masterTime);

	if (masterTime < _currentTime) {
		currentMasterLoop++;

		if (currentMasterLoop >= masterLoopCount) {
			currentMasterLoop = 0;
		}
	}

	_currentTime = masterTime;

	if (eventCount > 0 && buf[0].time == _currentTime &&
			buf[0].loopIdx == currentMasterLoop)
	{
		playIdx = 0;
		noteOnOffCount = 0;
	}
}

void MidiLoop::shiftMasterTime(uint32_t& masterTime)
{
	uint32_t maxMasterTime = masterPtr->loopEndTime;
	int32_t tmp = (int32_t) masterTime + deltaTime;

	if (tmp < 0) {
		tmp = tmp + maxMasterTime + 1;
	} else if (tmp > (int32_t) maxMasterTime) {
		tmp = tmp - maxMasterTime - 1;
	}

	masterTime = (uint32_t) tmp;
}

void MidiLoop::beginLoop() {
	if (state != PLAYING) {
		return;
	}

	beginLoopRequested = true;
}

void MidiLoop::endLoop() {
	if (state != RECORDING) {
		return;
	}

	state = PLAYING;
	sendEventPending = false;
	_loopEndTime = _currentTime;
	loopEndTimeHalf = _loopEndTime >> 1;
	noteOnOffCount = 0;
	_loopFull = false;

	if (masterPtr/*this is slave*/ && eventCount > 0) {
		masterLoopCount = buf[eventCount - 1].loopIdx + 1;
		currentMasterLoop = 0;
		// calculate time shift
		deltaTime = (int32_t) (buf[0].time) - (int32_t) _currentTime;
		// play first event immediately
		_currentTime = buf[0].time;
		playIdx = 0;
	}
}

void MidiLoop::midiEvent(char b1, char b2, char b3) {
	if (state != RECORDING) {
		return;
	}

	if (eventCount < bufLength) {
		MidiEvent& event = buf[eventCount];

		event.b1 = b1;
		event.b2 = b2;
		event.b3 = b3;

		if (eventCount == 0) {
			if (!masterPtr/*this is master*/) {
				_currentTime = 0;
			}

			_loopEndTime = 0xFFFFFFFF;
			loopEndTimeHalf = _loopEndTime >> 1;
			currentMasterLoop = 0;
			masterLoopCount = 0xFFFFFFFF;
		}
		//printf("ev, t=%X, l=%X\r\n", _currentTime, currentMasterLoop);
		//printf("e=%X\r\n",b2);
		event.time = _currentTime;
		event.loopIdx = currentMasterLoop;
		eventCount++;
	} else {
		_loopFull = true;
	}
}

void MidiLoop::play() {
	MidiEvent& event = buf[playIdx];

	if (!sendEventPending) {
		bool fireCondition = ((event.time == _currentTime)
				&& (event.loopIdx == currentMasterLoop));

		if (fireCondition) {
			sendEventPending = true;
		}
	}

	if (sendEventPending) {
		if (sender.sendMidi(event.b1, event.b2, event.b3)) {

//			printf("b2=%X, b3=%X, idx=%X, ct=%X, et=%X\r\n",
//			  event.b2, event.b3, playIdx, _currentTime, event.time);

			// on/off counting
			if (((event.b1 & 0xF0) == 0x90) && event.b3 > 0) {
				noteOnOffCount++;
			} else if (((event.b1 & 0xF0) == 0x80
					|| (((event.b1 & 0xF0) == 0x90) && event.b3 == 0))) {
				noteOnOffCount--;
			}

			// ready to send next event
			sendEventPending = false;

			if ((playIdx + 1) < eventCount) {
				playIdx++;
			}
			else
			{
				playIdx = 0;
			}
		}
	}
}

void MidiLoop::tick() {
	if (state == PLAYING && eventCount > 0) {
		play();
	}

	if (beginLoopRequested && noteOnOffCount == 0) {
		beginLoopRequested = false;
		state = RECORDING;
		eventCount = 0;
		deltaTime = 0;
	}
}
