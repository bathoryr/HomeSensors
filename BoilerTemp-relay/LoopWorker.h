#pragma once
class LoopWorker
{
private:
	unsigned long counter;
	unsigned long timeout;
	void (*ptrFunc)();
public:
	LoopWorker(void* ptrFunc, unsigned int seconds);
	void DoWork();
	~LoopWorker();
};

