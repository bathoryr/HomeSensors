/*
 Name:		rc_switch.ino
 Created:	11/17/2018 10:41:45 AM
 Author:	batho
*/

// the setup function runs once when you press reset or power the board
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
	Serial.begin(115200);
	mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (mySwitch.available()) {
		output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(), mySwitch.getReceivedProtocol());
		mySwitch.resetAvailable();
	}
}
