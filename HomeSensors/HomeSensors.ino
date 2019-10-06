#include <SPI.h>
#define MY_SPLASH_SCREEN_DISABLED
#define MY_RADIO_RF24

#include <MyConfig.h>
#include <MySensors.h>

String inputString;
const uint8_t CHILD_ID_1 = 1;

// the setup function runs once when you press reset or power the board
void setup() 
{
	inputString.reserve(10);
	request(CHILD_ID_1, V_LEVEL);
}

void presentation()
{
	sendSketchInfo("Test sensor", "0.0.1");

	present(CHILD_ID_1, S_SOUND, "Test control");
}

// the loop function runs over and over again until power down or reset
void loop() 
{
  
}

void receive(const MyMessage &message)
{
	switch (message.sensor)
	{
	case CHILD_ID_1:
		if (message.type == V_LEVEL) 
		{
			Serial.print("Got message V_LEVEL: ");
			Serial.println(message.getInt());
			Serial.println(message.getByte());
			Serial.println(message.getLong());
			Serial.println(message.getFloat());
			Serial.println(message.getString());

		}
	}
}

void serialEvent()
{
	while (Serial.available()) 
	{
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag so the main loop can
		// do something about it:
		if (inChar == '\n') 
		{
			MyMessage msg(CHILD_ID_1, V_LEVEL);
			send(msg.set((int)inputString.toInt()));
			inputString = "";
		}
	}
}