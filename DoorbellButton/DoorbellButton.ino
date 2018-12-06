#define MY_RADIO_NRF24
#define RF24_PA_MAX
#include <MyConfig.h>
#include <MySensors.h>  
#undef DEBUG

#define PIN_BUTTON_1	2
#define PIN_BUTTON_2	3
#define ID_VOLTAGE		9

const long InternalReferenceVoltage = 1084;  // Adjust this value to your board's specific internal BG voltage
static const float VMIN = 1.8, VMAX = 3.5;

MyMessage msgBtn(PIN_BUTTON_1, V_TRIPPED);
bool msgAck;
uint8_t counter;

void presentation() {
	sendSketchInfo("Doorbell buttons", "1.0");
	present(PIN_BUTTON_1, S_DOOR, "Doorbell button 1");
	present(PIN_BUTTON_2, S_DOOR, "Doorbell button 2");
	present(ID_VOLTAGE, S_MULTIMETER, "Battery voltage");
}

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#endif
	pinMode(PIN_BUTTON_1, INPUT);
	pinMode(PIN_BUTTON_2, INPUT);
	// Call twice - one for voltage, one for percent level
	sendBatteryMsg();
	sendBatteryMsg();
}

void loop() 
{
	int8_t btn = sleep(0, CHANGE, 1, CHANGE, 3600000);
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
			unsigned long t = millis();
#endif
			send(msgBtn.set(digitalRead(PIN_BUTTON_1 + btn) == LOW), true);
			wait(500, C_SET, V_TRIPPED);
#ifdef DEBUG
			Serial.print("Wait took ");
			Serial.println(millis() - t);
#endif
		}
	}
	else
	{
#ifdef DEBUG
		Serial.println("Sleep timeout");
#endif
		sendBatteryMsg();
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

void sendBatteryMsg()
{
	float volt = getBandgap();
	if (counter++ % 2 == 1)
	{
		sendBatteryLevel(MIN(100, MAX((volt - VMIN) / (VMAX - VMIN) * 100.0, 0)));
	}
	else
	{
		MyMessage msgVolt(ID_VOLTAGE, V_VOLTAGE);
		send(msgVolt.set(volt, 2));
	}
}

// From example by Nick Gammon (http://www.gammon.com.au/power)
float getBandgap()
{
	// Add some delay for stabilize after sleep
	delay(50);
	// REFS0 : Selects AVcc external reference
	// MUX3 MUX2 MUX1 : Selects 1.1V (VBG)  
	ADMUX = bit(REFS0) | bit(MUX3) | bit(MUX2) | bit(MUX1);
	ADCSRA |= bit(ADSC);  // start conversion
	while (ADCSRA & bit(ADSC))
	{
	}  // wait for conversion to complete
	float results = (float)(((InternalReferenceVoltage * 1024) / ADC) + 5) / 1000;
	return results;
} // end of getBandgap