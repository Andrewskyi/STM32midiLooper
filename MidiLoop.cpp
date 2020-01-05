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
	currentTime(0), currentMasterLoop(0), loopEndTime(0), masterLoopEndTime(0),
	masterLoopCount(0), eventCount(0), playIdx(0), sendEventPending(false),
	sender(sender),
	master(master),
	state(RECORDING),
	noteOnOffCount(0), beginLoopRequested(false)
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
	if(masterTime <= currentTime && masterLoopCount > 0)
	{
		if((currentMasterLoop + 1) >= masterLoopCount)
		{
			currentMasterLoop = 0;
		}
		else
		{
			currentMasterLoop++;
		}
	}

	currentTime = masterTime;

	if(currentTime == loopEndTime && currentMasterLoop == masterLoopEndTime)
	{
		playIdx = 0;
		noteOnOffCount = 0;
	}
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
	//playIdx = 0;
	sendEventPending = false;
//	if(eventCount > 0)
//	  buf[0].time = currentTime;
	loopEndTime = currentTime;
	masterLoopEndTime = currentMasterLoop;
	//currentTime = 0;

	noteOnOffCount = 0;

	if(!master)
	{
		masterLoopCount = currentMasterLoop==0 ? 1 : currentMasterLoop ;
		currentMasterLoop = 0;
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
			masterLoopEndTime = 0xFFFFFFFF;
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
		printf("ovflw,e=%u,bl=%u\r\n", eventCount, bufLength);
	}
}

void MidiLoop::play()
{
	MidiEvent& event = buf[playIdx];

	if(!sendEventPending)
	{
		bool fireCondition = master ?
			(event.time == currentTime) :
		    ((event.time == currentTime) && event.loopIdx == currentMasterLoop);

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

			// redy to send next event
			sendEventPending = false;

			if((playIdx + 1) >= eventCount)
			{
				playIdx = 0;
			}
			else
			{
				playIdx++;
			}

//			if(playIdx == 0 && master)
//			{
//				currentTime = 0;
//			}
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
	}
}
