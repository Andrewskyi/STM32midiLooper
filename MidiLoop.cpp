/*
 * MidiLoop.cpp
 *
 *  Created on: 03.01.2020
 *      Author: apaluch
 */

#include "MidiLoop.h"
#include <stdio.h>

MidiLoop::MidiLoop(MidiEvent* buf, uint32_t bufLength,
		MidiSender& sender,
		bool master) :
	currentState(state),
	buf(buf), bufLength(bufLength),
	currentTime(0), currentMasterLoop(0), loopEndTime(0), /*masterLoopEndTime(0),*/
	masterLoopCount(0), eventCount(0), playIdx(0), sendEventPending(false),
	sender(sender),
	master(master),
	state(RECORDING),
	noteOnOffCount(0), beginLoopRequested(false),
	maxMasterTime(0), deltaTime(0), lastMasterTime(0)
{
	//printf("MidiLoop: buf=%u, bufLength=%u", buf, bufLength);
}

MidiLoop::~MidiLoop()
{
}

uint32_t MidiLoop::timeTick()
{
	if(currentTime < loopEndTime)
	{
		currentTime++;
	}
	else
	{
		currentTime = 0;
		playIdx = 0;
		noteOnOffCount = 0;
	}

	return currentTime;
}

void MidiLoop::timeTick(uint32_t masterTime)
{
	// find max master time
	if(masterTime < lastMasterTime) // TODO - slabizna
	{
		maxMasterTime = lastMasterTime;
	}
	lastMasterTime = masterTime;

	shiftMasterTime(masterTime);


	if(masterTime < currentTime)
	{
		currentMasterLoop++;

		if(currentMasterLoop >= masterLoopCount)
		{
			currentMasterLoop = 0;
		}
	}

	currentTime = masterTime;

	if(eventCount > 0 &&
		buf[0].time == currentTime && buf[0].loopIdx == currentMasterLoop)
	{
		playIdx = 0;
		noteOnOffCount = 0;
	}
}

void MidiLoop::shiftMasterTime(uint32_t& masterTime)
{
	int32_t tmp = (int32_t)masterTime + deltaTime;

	if(tmp < 0)
	{
		tmp = tmp + maxMasterTime + 1;
	}
	else if(tmp > (int32_t)maxMasterTime)
	{
		tmp = tmp - maxMasterTime - 1;
	}

	masterTime = (uint32_t)tmp;
}

void MidiLoop::beginLoop()
{
	if(state != PLAYING)
	{
		return;
	}

	beginLoopRequested = true;
}

void MidiLoop::endLoop()
{
	if(state != RECORDING)
	{
		return;
	}

	state = PLAYING;
	sendEventPending = false;
	loopEndTime = currentTime;
	noteOnOffCount = 0;

	if(!master && eventCount > 0)
	{
		masterLoopCount = buf[eventCount-1].loopIdx + 1;
		currentMasterLoop = 0;
		// calculate time shift
		deltaTime = (int32_t)(buf[0].time) - (int32_t)currentTime;
		// play first event immediately
		currentTime = buf[0].time;
		playIdx = 0;
	}
}

void MidiLoop::midiEvent(char b1, char b2, char b3)
{
	if(state != RECORDING)
	{
		return;
	}

	if(eventCount < bufLength)
	{
		MidiEvent& event = buf[eventCount];

		event.b1 = b1;
		event.b2 = b2;
		event.b3 = b3;

		if(eventCount == 0)
		{
			if(master)
			{
				currentTime = 0;
			}

			loopEndTime = 0xFFFFFFFF;
			currentMasterLoop = 0;
			masterLoopCount = 0xFFFFFFFF;
		}
		//printf("ev, t=%X, l=%X\r\n", currentTime, currentMasterLoop);
		//printf("e=%X\r\n",b2);
		event.time = currentTime;
		event.loopIdx = currentMasterLoop;
		eventCount++;
	}
	else
	{
		//printf("ovflw,e=%u,bl=%u\r\n", eventCount, bufLength);
	}
}

void MidiLoop::play()
{
	MidiEvent& event = buf[playIdx];

	if(!sendEventPending)
	{
		bool fireCondition = ((event.time == currentTime) &&
				(event.loopIdx == currentMasterLoop));

		if(fireCondition)
		{
			sendEventPending = true;
		}
	}

	if(sendEventPending)
	{
		if(sender.sendMidi(event.b1, event.b2, event.b3))
		{
			// on/off counting
			if(((event.b1 & 0xF0) == 0x90) && event.b3 > 0)
			{
				noteOnOffCount++;
			}
			else if(((event.b1 & 0xF0) == 0x80 ||
				   (((event.b1 & 0xF0) == 0x90) && event.b3 == 0)))
			{
				noteOnOffCount--;
			}

			// ready to send next event
			sendEventPending = false;

			if((playIdx + 1) < eventCount)
			{
				playIdx++;
			}
		}
	}
}

void MidiLoop::tick()
{
	if(state == PLAYING && eventCount > 0)
	{
		play();
	}

	if(beginLoopRequested && noteOnOffCount == 0)
	{
		beginLoopRequested = false;
		state = RECORDING;
		eventCount = 0;
		deltaTime = 0;
	}
}
