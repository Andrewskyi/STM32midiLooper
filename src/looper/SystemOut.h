/*
 * SystemOut.h
 *
 *  Created on: 04.01.2020
 *      Author: apaluch
 */

#ifndef _SYSTEM_OUT_H_
#define _SYSTEM_OUT_H_

#include "TxFifo.h"

#define SystemOut_BUFFER_LENGTH 32

class SystemOut : public TxFifo {
public:
	SystemOut(TxFifo_sendByte sendFunc);
	virtual ~SystemOut();
private:
	char buffer[SystemOut_BUFFER_LENGTH];
};

#endif /* _SYSTEM_OUT_H_ */
