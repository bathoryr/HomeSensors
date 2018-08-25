#include <Arduino.h>
#include "LoopWorker.h"


LoopWorker::LoopWorker(void * ptrFunc, unsigned int seconds)
{
	this->ptrFunc = ptrFunc;
	this->timeout = (unsigned long)seconds * 1000;
	counter = millis();
}

void LoopWorker::DoWork()
{
	if (millis() - counter > timeout)
	{
		if (ptrFunc != NULL)
		{
			ptrFunc();
		}
		counter = millis();
	}
}

LoopWorker::~LoopWorker()
{
}
