#pragma once
class Switch
{
public:
	Switch(int pin);
	~Switch();

	void TurnOn();
	void TurnOff();

	bool GetStatus();

private:
	int input_pin;
	bool state;
};

