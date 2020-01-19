/*
 * MidiReceiver.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "MidiReceiver.h"
#include <stdio.h>

MidiReceiver::MidiReceiver(_MIDI_recByte recFunc, MidiSender* midiThru) :
		recFunc(recFunc), midiThru(midiThru), runningStatusByte(0), bytesCount(
				0), systemRealTimeEvent(0) {
}

MidiReceiver::~MidiReceiver() {
}

bool MidiReceiver::nextEvent(char& b1, char& b2, char& b3) {
	char b;

	if (recFunc(b)) {
		// Channel Voice Message
		if (b >= 0x80 && b <= 0xEF) {
			runningStatusByte = b;
			bytesCount = 1;

			expectedBytesCount = expectedChannelVoiceMsgBytesCount();
		}
		// Channel Voice Message

		// System Common Messages
		else if (b >= 0xF0 && b <= 0xF7) {
			runningStatusByte = 0;
			buf[0] = b;

			switch (b) {
			case 0xF0:
			case 0xF7:
				// ignore message
				expectedBytesCount = 0;
				bytesCount = 0;
				break;
			case 0xF1:
			case 0xF3:
				expectedBytesCount = 2;
				bytesCount = 1;
				break;
			case 0xF2:
				expectedBytesCount = 3;
				bytesCount = 1;
				break;
			case 0xF4:
			case 0xF5:
			case 0xF6:
				expectedBytesCount = 1;
				bytesCount = 1;
				break;
			}
		}
		// System Common Messages

		// System Real-Time Messages
		else if (b >= 0xF8) {
			systemRealTimeEvent = b;
		}
		// System Real-Time Messages

		else if (b <= 127 && bytesCount < expectedBytesCount) {
			buf[bytesCount] = b;
			bytesCount++;
		}

		bool eventForLooper = false;

		if (bytesCount > 0 && bytesCount == expectedBytesCount) {
			if (runningStatusByte > 0) {
				buf[0] = runningStatusByte;
			}

			//printf("%X %X %X\r\n", buf[0], buf[1], buf[2]);
			//printf("%X\r\n", buf[0]);

			if (midiThru) {
				midiThru->sendMidi(buf, bytesCount);
			}

			if (bytesCount == 3
					&& ((runningStatusByte & 0xF0) == 0x80
							|| (runningStatusByte & 0xF0) == 0x90)) {
				b1 = buf[0];
				b2 = buf[1];
				b3 = buf[2];

				eventForLooper = true;
			}

			if (runningStatusByte > 0) {
				bytesCount = 1;
				expectedBytesCount = expectedChannelVoiceMsgBytesCount();
			} else {
				bytesCount = 0;
				expectedBytesCount = 0;
			}
		}

		return eventForLooper;
	}

	return false;
}

void MidiReceiver::tick() {
	if (systemRealTimeEvent) {
		if (midiThru) {
			if (midiThru->sendRealTimeMidi(systemRealTimeEvent)) {
				systemRealTimeEvent = 0;
			}
		} else {
			systemRealTimeEvent = 0;
		}
	}
}

uint32_t MidiReceiver::expectedChannelVoiceMsgBytesCount() {
	switch (runningStatusByte & 0xF0) {
	case 0xC0:
	case 0xD0:
		return 2;
	default:
		return 3;
	}
}
