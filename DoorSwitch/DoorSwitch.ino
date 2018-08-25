/*
Door sensor/switch to control light
*/

#define MY_RADIO_NRF24
#include <MyConfig.h>
#include <MySensors.h>  

#include "LoopWorker.h"

#define PIN_SWITCH 6			// On-Off switch
#define PIN_DOOR_SENSOR 2		// Door open-close hall sensor
#define PIN_MOVE_SENSOR 3		// Move sensor PIR
#define PIN_OUT_LIGHT 7			// Light control On-Off
#define MSG_OFF_TIMEOUT 16		// To set timeout for auto light-off

LoopWorker Loop1s(CheckSensors, 1);
LoopWorker LoopTimeout(TurnLightOff, 300);

bool request_for_timeout = true;

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(PIN_SWITCH, INPUT_PULLUP);
	pinMode(PIN_DOOR_SENSOR, INPUT_PULLUP);
	pinMode(PIN_MOVE_SENSOR, INPUT);
	pinMode(PIN_OUT_LIGHT, OUTPUT);

	request(MSG_OFF_TIMEOUT, V_LEVEL);
}

void presentation() {
	sendSketchInfo("Door switch", "1.1.1");
	present(PIN_SWITCH, S_BINARY, "Light switch");			// V_STATUS
	present(PIN_DOOR_SENSOR, S_DOOR, "Door sensor");		// V_TRIPPED
	present(PIN_MOVE_SENSOR, S_MOTION, "Motion sensor");	// V_TRIPPED
	present(PIN_OUT_LIGHT, S_BINARY, "Light on/off");		// V_STATUS
	present(MSG_OFF_TIMEOUT, S_LIGHT_LEVEL, "Auto off timeout");	// V_LEVEL
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	Loop1s.DoWork();
	LoopTimeout.DoWork();
}

void receive(const MyMessage &message)
{
	switch (message.sensor)
	{
	case PIN_OUT_LIGHT:
		if (message.type == V_STATUS)
		{
			digitalWrite(PIN_OUT_LIGHT, message.getBool());
		}
		break;
	case MSG_OFF_TIMEOUT:
		if (message.type == V_LEVEL)
		{
			if (message.getInt() > 0)
			{
				LoopTimeout.SetTimeout(message.getInt());
				request_for_timeout = false;
			}
		}
		break;
	}
}

int doorState;

void CheckSensors()
{
	auto door = digitalRead(PIN_DOOR_SENSOR);
	if (door != doorState)
	{
		// Reset turn-off timer
		LoopTimeout.Reset();

		doorState = door;
		ControlLight(door);
	}
}

// Turn light off after some time
void TurnLightOff()
{
	if (request_for_timeout == true)
	{
		request(MSG_OFF_TIMEOUT, V_LEVEL);
	}
	ControlLight(LOW);
}

void ControlLight(int state)
{
	digitalWrite(PIN_OUT_LIGHT, state);
	MyMessage msg(PIN_OUT_LIGHT, V_STATUS);
	send(msg.set(state != 0));

	MyMessage msgDoor(PIN_DOOR_SENSOR, V_TRIPPED);
	send(msgDoor.set(state));
}
