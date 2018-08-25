#include <Arduino.h>
#include "LoopWorker.h"


LoopWorker::LoopWorker(void (*ptrFunc)(), unsigned int seconds)
{
	_ptrFunc = ptrFunc;
	timeout = (unsigned long)seconds * 1000;
	counter = millis();
}

LoopWorker::~LoopWorker()
{
}

void LoopWorker::DoWork()
{
	if (timeout > 0ul)
	{
		if (millis() - counter > timeout)
		{
			if (_ptrFunc != NULL)
			{
				_ptrFunc();
			}
			counter = millis();
		}
	}
}

// Set new timeout interval in seconds. When is set to 0, no function is called in DoWork()
unsigned int LoopWorker::GetTimeout()
{
	return timeout > 0ul ? timeout / 1000 : 0;
}

void LoopWorker::SetTimeout(unsigned int newTimeout)
{
	timeout = (unsigned long)newTimeout * 1000;
}

// Reset timeout - set counter to start state
void LoopWorker::Reset()
{
	counter = millis();
}
