/*
 * MidiEvent.h
 *
 *  Created on: 23.01.2020
 *      Author: apaluch
 */

#ifndef LOOPER_MIDIEVENT_H_
#define LOOPER_MIDIEVENT_H_

#include <stdint.h>

struct MidiEvent
{
	uint8_t b1;
	uint8_t b2;
	uint8_t b3;
	uint16_t loopIdx;
	uint32_t time;
} __attribute__((packed));

#endif /* LOOPER_MIDIEVENT_H_ */
