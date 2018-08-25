#pragma once
class LoopWorker
{
public:
	LoopWorker(void (*ptrFunc)(), unsigned int seconds);
	void DoWork();
	unsigned int GetTimeout();
	void SetTimeout(unsigned int newTimeout);
	void Reset();
	~LoopWorker();
private:
	unsigned long counter;
	unsigned long timeout;
	void (*_ptrFunc)();
};

