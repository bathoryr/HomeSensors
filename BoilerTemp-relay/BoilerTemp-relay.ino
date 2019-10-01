/* Boiler temp/control node
*
* Config: MYS bootloader, ext XTAL 16 MHz
*/

// Enable debug prints
//#define MY_DEBUG

// Enable and select radio type attached
//#define MY_REPEATER_FEATURE
#define MY_RADIO_RF24
#define MY_RX_MESSAGE_BUFFER_FEATURE
#define MY_RX_MESSAGE_BUFFER_SIZE 8
#define MY_RF24_IRQ_PIN 2
// Prevent this repeater to connect to another repeater
//#define MY_PARENT_NODE_ID 0
#define RF24_PA_MAX

#include <MyConfig.h>
#include <MySensors.h>  
#include <DallasTemperature.h>
#include <OneWire.h>
#include "LoopWorker.h"

#define CHILD_ID_TEMP 0
#define CHILD_ID_VOLTAGE 1
#define CHILD_ID_BOILER 2
#define CHILD_ID_HEATING 3
#define CHILD_ID_DEST_TEMP 4

#define EEPROM_TARGET_TEMP 1

#define TEMP_SENSOR_DIGITAL_PIN 7
#define RELAY_PIN 6
#define SOLENOID_PIN 5

OneWire oneWire(TEMP_SENSOR_DIGITAL_PIN);
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 

float lastTemperature;
int TargetTemp = 55;

//MyMessage msgVolt(CHILD_ID_VOLTAGE, V_VOLTAGE);
//MyMessage msgBoiler(CHILD_ID_BOILER, V_STATUS);
//MyMessage msgDestTemp(CHILD_ID_DEST_TEMP, V_TEMP);

//void loop60s();
LoopWorker loopWorker60(sendBoilerTemp, 60);
LoopWorker loopWorker15m(sendHeartbeat, 900);

void setup()
{
	sensors.begin();
	int tt = loadState(EEPROM_TARGET_TEMP);
	if (tt > 20 && tt < 80)
		TargetTemp = tt;
	request(CHILD_ID_DEST_TEMP, V_TEMP);

	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
}

void presentation()
{
	// Send the Sketch Version Information to the Gateway
	sendSketchInfo("BoilerTemp", "0.8");

	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_ID_TEMP, S_TEMP, "Boiler temp");
	//  present(CHILD_ID_VOLTAGE, S_MULTIMETER);
	//  present(CHILD_ID_BOILER, S_BINARY, "Boiler led");
	present(CHILD_ID_HEATING, S_BINARY, "Boiler heat");
	present(CHILD_ID_DEST_TEMP, S_TEMP, "Target temp");
}

void loop()
{
	loopWorker60.DoWork();
	loopWorker15m.DoWork();
}

void receive(const MyMessage &message)
{
	switch (message.sensor) {
	case CHILD_ID_HEATING:
		if (message.type == V_STATUS) {
			digitalWrite(RELAY_PIN, message.getBool() ? HIGH : LOW);
		}
		break;
	case CHILD_ID_DEST_TEMP:
		if (message.type == V_TEMP) {
			if (message.getInt() != TargetTemp) {
				TargetTemp = message.getInt();
				saveState(EEPROM_TARGET_TEMP, TargetTemp);
			}
#ifdef MY_DEBUG
			Serial.print("Change dest. temp request: ");
			Serial.println(TargetTemp);
#endif
		}
		break;
	}
}

void sendBoilerTemp()
{
	MyMessage msgHeating(CHILD_ID_HEATING, V_STATUS);

	sensors.requestTemperatures();
	float temperature = sensors.getTempCByIndex(0);
#ifdef MY_DEBUG
	Serial.print("Temperature: ");
	Serial.println(temperature);
#endif
	if (lastTemperature != temperature && temperature != -127.00) {
		MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

		send(msgTemp.set(temperature, 1));
		lastTemperature = temperature;

		if (temperature < TargetTemp - 10) {
			digitalWrite(RELAY_PIN, HIGH);
			send(msgHeating.set(1));
		}
		
		if (temperature >= TargetTemp) {
			digitalWrite(RELAY_PIN, LOW);
			send(msgHeating.set(0));
		}
	}
	if (temperature > TargetTemp + 5) {
		digitalWrite(RELAY_PIN, LOW);
		send(msgHeating.set(0));
	}
}