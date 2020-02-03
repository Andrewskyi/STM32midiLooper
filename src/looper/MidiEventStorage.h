/*
 * MidiEventStorage.h
 *
 *  Created on: 30.01.2020
 *      Author: apaluch
 *
 *   MIT License

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

#ifndef LOOPER_MIDIEVENTSTORAGE_H_
#define LOOPER_MIDIEVENTSTORAGE_H_

#include "MidiEvent.h"

struct MidiEventStorage
{
    uint8_t b1;
    uint8_t b2;
    uint8_t b3 : 7;
    uint8_t variant : 2;
    uint8_t : 7;
} __attribute__((packed));

struct MidiEventWithoutStatusStorage
{
    uint8_t b2;
    uint8_t b3 : 7;
    uint8_t variant : 2;
    uint8_t : 7;
} __attribute__((packed));

// master variants

struct M_V00
{
    uint8_t v : 1;
    uint8_t time : 7;
} __attribute__((packed));

struct M_V01
{
    uint8_t v : 1;
    uint16_t time : 15;
} __attribute__((packed));

struct M_V10
{
    uint8_t v : 1;
    uint32_t time : 23;
} __attribute__((packed));

// slave variants

struct S_V00
{
    uint8_t v : 1;
    uint8_t time : 7;
} __attribute__((packed));

struct S_V01
{
    uint8_t v : 1;
    uint8_t loop : 2;
    int16_t time : 13;
} __attribute__((packed));

struct S_V10
{
    uint8_t v : 1;
    uint8_t loop : 5;
    int32_t time : 18;
} __attribute__((packed));

struct S_V11
{
    uint8_t v : 1;
    uint16_t loop : 15;
    int32_t time : 24;
} __attribute__((packed));

inline void assign(MidiEvent& ev1, const MidiEventStorage& ev2)
{
    ev1.b1 = ev2.b1;
    ev1.b2 = ev2.b2;
    ev1.b3 = ev2.b3;
}

inline void assign(MidiEvent& ev1, const MidiEventWithoutStatusStorage& ev2)
{
    ev1.b2 = ev2.b2;
    ev1.b3 = ev2.b3;
}

inline void assign(MidiEventStorage& ev1, const MidiEvent& ev2)
{
    ev1.b1 = ev2.b1;
    ev1.b2 = ev2.b2;
    ev1.b3 = ev2.b3;
}

inline void assign(MidiEventWithoutStatusStorage& ev1, const MidiEvent& ev2)
{
    ev1.b2 = ev2.b2;
    ev1.b3 = ev2.b3;
}

#endif /* LOOPER_MIDIEVENTSTORAGE_H_ */
