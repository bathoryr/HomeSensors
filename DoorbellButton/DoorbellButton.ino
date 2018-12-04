#define MY_RADIO_NRF24
#define RF24_PA_MAX
#include <MyConfig.h>
#include <MySensors.h>  

#define PIN_BUTTON_1	2
#define PIN_BUTTON_2	3

MyMessage msgBtn(PIN_BUTTON_1, V_TRIPPED);
bool msgAck;

void presentation() {
	sendSketchInfo("Doorbell buttons", "1.0");
	present(PIN_BUTTON_1, S_DOOR, "Doorbell button 1");
	present(PIN_BUTTON_2, S_DOOR, "Doorbell button 2");
}

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#endif
	pinMode(PIN_BUTTON_1, INPUT);
	pinMode(PIN_BUTTON_2, INPUT);
}

void loop() 
{
	int8_t btn = sleep(0, CHANGE, 1, CHANGE, 600000);
	if (btn >= 0) {
		delay(20);
		msgBtn.sensor = PIN_BUTTON_1 + btn;
		msgAck = false;
		for (int i = 0; msgAck == false && i < 3; i++)
		{
#ifdef DEBUG
			if (i > 0)
			{
				Serial.println("Send retry...");
			}
#endif
			send(msgBtn.set(digitalRead(PIN_BUTTON_1 + btn) == LOW), true);
			unsigned long t = millis();
			wait(500, C_SET, V_TRIPPED);
#ifdef DEBUG
			Serial.print("Wait took ");
			Serial.println(millis() - t);
#endif
		}
	}
	else
	{
		sleep(100, true);
	}
}

void receive(const MyMessage& msg)
{
	if (msg.isAck())
	{ 
		msgAck = true;
#ifdef DEBUG
		Serial.print("ACK received: ");
		Serial.print(msg.type);
		Serial.print(msg.sensor);
		Serial.println(msg.getCommand());
#endif
	}
}