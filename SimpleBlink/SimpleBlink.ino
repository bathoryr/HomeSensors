/*
 Name:		SimpleBlink.ino
 Created:	4/15/2018 2:50:00 PM
 Author:	batho
*/

#include "LoopWorker.h"

LoopWorker Loop2s(BlinkLED, 2);

void BlinkLED()
{
	if (digitalRead(LED_BUILTIN) == HIGH)
		digitalWrite(LED_BUILTIN, LOW);
	else
		digitalWrite(LED_BUILTIN, HIGH);
}
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	Loop2s.DoWork();
}
