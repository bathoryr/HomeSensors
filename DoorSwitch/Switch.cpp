#include <MySensors.h>  
#include "Switch.h"


Switch::Switch(int pin)
{
	input_pin = pin;
}


Switch::~Switch()
{
}

void Switch::TurnOn()
{
}

void Switch::TurnOff()
{
}

bool Switch::GetStatus()
{
	bool st = digitalRead(input_pin);
	if (state != st)
	{
		state = st;
		MyMessage msg(input_pin, V_STATUS);
		send(msg.set(state));
	}
	return st;
}
