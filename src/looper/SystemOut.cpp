/*
 * SystemOut.cpp
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#include "SystemOut.h"

SystemOut::SystemOut(TxFifo_sendByte sendFunc) :
	TxFifo(buffer, SystemOut_BUFFER_LENGTH, sendFunc)
{

}

SystemOut::~SystemOut()
{
}
