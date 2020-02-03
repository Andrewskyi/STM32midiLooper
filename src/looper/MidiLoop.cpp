/*
 * MidiLoop.cpp
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

#include "MidiLoop.h"
#include <stdio.h>

MidiLoop::MidiLoop(MidiEventBuffer& buf, MidiSender& sender,
		MidiLoop const * const masterPtr) :
		currentState(state),
		currentTime(_currentTime),
		loopEndTime(_loopEndTime),
		isSecondHalf(_isSecondHalf),
		loopFull(_loopFull),
		stateChangeRequested(beginLoopRequested),
		buf(buf),
		_currentTime(0), currentMasterLoop(0),
		_loopEndTime(0), loopEndTimeHalf(0), _isSecondHalf(false),
		masterLoopCount(0), eventCount(0), playIdx(0), sendEventPending(false),
		sender(sender), masterPtr(masterPtr),
		state(RECORDING), noteOnOffCount(0),
		beginLoopRequested(false), deltaTime(0),
		firstEventTime(0), firstEventMasterLoop(0)
{
	//printf("MidiLoop: buf=%u, bufLength=%u", buf, bufLength);
}

MidiLoop::~MidiLoop()
{
}

uint32_t MidiLoop::midiTimeTick()
{
	if(!masterPtr)
	{
		if (_currentTime < _loopEndTime)
		{
			_currentTime++;
			_isSecondHalf = (_currentTime >= loopEndTimeHalf);
		}
		else
		{
			_currentTime = 0;
			_isSecondHalf = false;
			buf.reset();
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

	if (masterTime < _currentTime)
	{
		currentMasterLoop++;

		if (currentMasterLoop >= masterLoopCount)
		{
			currentMasterLoop = 0;
		}
	}

	_currentTime = masterTime;

	if (eventCount > 0 && firstEventTime == _currentTime &&
			firstEventMasterLoop == currentMasterLoop)
	{
		buf.reset();
		playIdx = 0;
		noteOnOffCount = 0;
	}
}

void MidiLoop::shiftMasterTime(uint32_t& masterTime)
{
	uint32_t maxMasterTime = masterPtr->loopEndTime;
	int32_t tmp = (int32_t) masterTime + deltaTime;

	if (tmp < 0)
	{
		tmp = tmp + maxMasterTime + 1;
	}
	else if (tmp > (int32_t) maxMasterTime)
	{
		tmp = tmp - maxMasterTime - 1;
	}

	masterTime = (uint32_t) tmp;
}

void MidiLoop::beginLoop()
{
	if (state != PLAYING)
	{
		return;
	}

	beginLoopRequested = true;
}

void MidiLoop::endLoop()
{
	if (state != RECORDING)
	{
		return;
	}

	state = PLAYING;
	sendEventPending = false;
	_loopEndTime = _currentTime;

	if (masterPtr/*this is slave*/ && eventCount > 0)
	{
		const auto& lastEvent = buf.event;
		currentMasterLoop = 0;
		// calculate time shift
		deltaTime = (int32_t) (firstEventTime) - (int32_t) _currentTime;
		// loop as short as possible
		_loopEndTime = lastEvent.time;
		masterLoopCount = lastEvent.loopIdx + 1;
		// play first event immediately
		_currentTime = firstEventTime;
		playIdx = 0;
		buf.reset();
	}

	loopEndTimeHalf = _loopEndTime >> 1;
	noteOnOffCount = 0;
	_loopFull = false;
}

void MidiLoop::midiEvent(char b1, char b2, char b3)
{
	if (state != RECORDING)
	{
		return;
	}

	if (!buf.eof)
	{
		if (eventCount == 0)
		{
			if (!masterPtr/*this is master*/)
			{
				_currentTime = 0;
			}

			_loopEndTime = 0xFFFFFFFF;
			loopEndTimeHalf = _loopEndTime >> 1;
			currentMasterLoop = 0;
			masterLoopCount = 0xFFFFFFFF;
			// memorize first event timing parameters
			firstEventTime = _currentTime;
			firstEventMasterLoop = currentMasterLoop;

			buf.reset();
		}
		else
		{
			buf.nextEventToWrite();
		}

		MidiEvent& event = buf.event;

		event.b1 = b1;
		event.b2 = b2;
		event.b3 = b3;

		//printf("ev, t=%X, l=%X\r\n", _currentTime, currentMasterLoop);
		//printf("e=%X\r\n",b2);
		event.time = _currentTime;
		event.loopIdx = currentMasterLoop;
		buf.saveEvent();
		eventCount++;
	}
	else
	{
		_loopFull = true;
	}
}

void MidiLoop::play() {
	MidiEvent& event = buf.event;

	if (!sendEventPending)
	{
		bool fireCondition = ((event.time == _currentTime)
				&& (event.loopIdx == currentMasterLoop));

		if (fireCondition)
		{
			sendEventPending = true;
		}
	}

	if (sendEventPending)
	{
		if (sender.sendMidi(event.b1, event.b2, event.b3))
		{
			//printf("play(): time=%X\r\n", _currentTime);

			// on/off counting
			if (((event.b1 & 0xF0) == 0x90) && event.b3 > 0)
			{
				noteOnOffCount++;
			} else if (((event.b1 & 0xF0) == 0x80
					|| (((event.b1 & 0xF0) == 0x90) && event.b3 == 0)))
			{
				noteOnOffCount--;
			}

			// ready to send next event
			sendEventPending = false;

			if ((playIdx + 1) < eventCount)
			{
				buf.nextEventToRead();
				playIdx++;
			}
			else
			{
				buf.reset();
				playIdx = 0;
			}
		}
	}
}

void MidiLoop::tick()
{
	if (state == PLAYING && eventCount > 0)
	{
		play();
	}

	if (beginLoopRequested && noteOnOffCount == 0)
	{
		beginLoopRequested = false;
		state = RECORDING;
		eventCount = 0;
		deltaTime = 0;
	}
}
