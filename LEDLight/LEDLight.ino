// Sensor config: MYSBootloader, 16MHz Xtal

//#define MY_DEBUG
//#define MY_DEBUG_VERBOSE_RF24
#include <Wire.h>
#define MY_SPLASH_SCREEN_DISABLED
#define MY_RADIO_RF24
#define MY_RF24_PA_LEVEL (RF24_PA_MAX)
#include <MyConfig.h>
#include <MySensors.h>

#define OUTPUT_PIN 5
#define MOTION_LED 7 // Move indication LED
#define MOTION_PIN 4

#define CHILD_ID_LIGHT 1
#define CHILD_ID_MOTION 2
#define CHILD_ID_LUX 3
#define CHILD_ID_TIMEOUT 4
#define CHILD_ID_MOVE_DETECT 5
#define CHILD_ID_INFO 99

#include "LoopWorker.h"
#include "Light.h"

Light light;
LoopWorker Loop1s(CheckMotion, 1);
LoopWorker Loop15m(SendIllumination, 900);

void setup()
{
	light.Setup();
	pinMode(OUTPUT_PIN, OUTPUT);
	pinMode(MOTION_LED, OUTPUT);
	pinMode(MOTION_PIN, INPUT);

	request(CHILD_ID_LIGHT, V_DIMMER);
	// DON'T wait for longer time (200ms), or values in messages are overwritten!!! Maybe don't wait at all
	//wait(10);
	request(CHILD_ID_LIGHT, V_STATUS);
	//wait(10);
	request(CHILD_ID_MOVE_DETECT, V_STATUS);
	//wait(10);
	request(CHILD_ID_TIMEOUT, V_LEVEL);
	//wait(10);
}

void presentation()
{
	sendSketchInfo("Dimmable Light", "2.9.5");

	present(CHILD_ID_LIGHT, S_DIMMER, "Light control");
	present(CHILD_ID_MOTION, S_MOTION, "Motion activity");
	present(CHILD_ID_LUX, S_LIGHT_LEVEL, "Illumination level");
	present(CHILD_ID_TIMEOUT, S_SOUND, "Light timeout");
	present(CHILD_ID_MOVE_DETECT, S_BINARY, "Move detect switch");
#ifdef MY_DEBUG
	present(CHILD_ID_INFO, S_INFO, "Text info");
#endif
}

void loop()
{
	Loop1s.DoWork();
	Loop15m.DoWork();
}

void CheckMotion()
{
	light.CheckMotion();
}

void SendIllumination()
{
	MyMessage luxMsg(CHILD_ID_LUX, V_LEVEL);
	send(luxMsg.set(light.GetAvgIllumination()));
}

void receive(const MyMessage &message)
{
#ifdef MY_DEBUG
	MyMessage msg(CHILD_ID_INFO, V_TEXT);
#endif
	switch (message.sensor)
	{
	case CHILD_ID_LIGHT:
		if (message.type == V_STATUS) {
			// Message from light switch (ON/OFF)
			message.getBool() == true ? light.TurnOn() : light.TurnOff();
		}
		if (message.type == V_DIMMER) {
			// Message from dimmer (intensity 0 - 100)
			light.SetIntensity(message.getInt());
#ifdef MY_DEBUG
			String s = "Got V_DIMMER val: ";
			send(msg.set((s + message.getString()).c_str()));
#endif
		}
		break;
	case CHILD_ID_TIMEOUT:
		if (message.type == V_LEVEL) {
			// Light timeout - if not set on controller, can be 0
			if (message.getInt() > 0) {
				light.SetTimeout(message.getInt());
#ifdef MY_DEBUG

				send(msg.set("Got V_LEVEL val"));
#endif
			}
			else
			{
				// Set to default 15 minutes
				light.SetTimeout(15);
				// Send message to controller to store the value so we can request it later
				MyMessage msgTm(CHILD_ID_TIMEOUT, V_LEVEL);
				send(msgTm.set(15));
#ifdef MY_DEBUG
				send(msg.set("Set V_LEVEL default"));
#endif
			}
		}
		break;
	case CHILD_ID_MOVE_DETECT:
		if (message.type == V_STATUS) {
			light.SetMotionDetector(message.getBool());
#ifdef MY_DEBUG
			String s = "Got V_STATUS msg: ";
			send(msg.set((s += message.getBool() ? "ON" : "OFF").c_str()));
#endif
		}
		break;
	}
}
