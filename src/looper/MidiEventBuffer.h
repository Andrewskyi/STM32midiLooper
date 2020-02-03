/*
 * MidiEventBuffer.h
 *
 *  Created on: 01.02.2020
 *      Author: apaluch
 */

#ifndef LOOPER_MIDIEVENTBUFFER_H_
#define LOOPER_MIDIEVENTBUFFER_H_

#include "MidiEvent.h"

class MidiEventBuffer {
public:
    MidiEvent event;
    volatile bool eof;

    MidiEventBuffer() : eof(false){};
    virtual ~MidiEventBuffer(){}

    virtual void nextEventToWrite() = 0;
    virtual void nextEventToRead() = 0;
    virtual void reset() = 0;
    virtual void saveEvent() = 0;
};

#endif /* LOOPER_MIDIEVENTBUFFER_H_ */
