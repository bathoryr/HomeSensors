#define MY_RADIO_NRF24
#include <MyConfig.h>
#include <MySensors.h>  
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void presentation() {
	sendSketchInfo("Doorbell control", "0.9");
	present(LED_BUILTIN, S_BINARY, "Doorbell switch");
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	mySwitch.enableTransmit(7);
	mySwitch.enableReceive(0);
	mySwitch.setPulseLength(155);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}

void receive(const MyMessage& message) {
	if (message.sensor == LED_BUILTIN) {
		if (message.type == V_STATUS) {
			digitalWrite(LED_BUILTIN, message.getBool());
			Serial.write("Received command");
			Serial.write(message.getBool() ? "true" : "false");
			Serial.write("\r\n");

			for (int i = 0; i < 5; i++)
			{
				mySwitch.send(16080895, 24);
			}
		}
	}
}
