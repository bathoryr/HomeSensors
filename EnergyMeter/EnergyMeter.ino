/* Node for reading value from electricity meter and from high/low tariff switch
 * Sensors are defined according to MySensors OpenHAB2 binding
 * Compile with MYSBootloader, XTAL, 16MHz
 * Based on EnergyMeter MySensors example
 */

#include <MyConfig.h>
 // Enable debug prints
 //#define MY_DEBUG

 // Enable and select radio type attached
#define MY_RADIO_NRF24
#include <MySensors.h>  

#define POWER_SENSOR_PIN 2      // Power sensor
#define HDO_SENSOR_PIN 3        // HDO sensor
#define PULSE_FACTOR 10000      // Nummber of blinks per KWH of your meter
#define MAX_WATT 15000          // Max watt value to report. This filetrs outliers.
#define CHILD_ID_POWER_PULSE 1  // Custom sensor thing: VAR1 - pulse count for HI, VAR2 - pulse count for LO
#define CHILD_ID_POWER_HI 3     // Power thing: HI (Watt, KWH)
#define CHILD_ID_POWER_LO 4     // Power thing: LO (KWH only)
#define CHILD_ID_POWER_HDO 2    // Door thing: HDO sensor (ON/OFF)

double ppwh = ((double)PULSE_FACTOR) / 1000; // Pulses per watt hour
bool pcReceivedHI = false;
bool pcReceivedLO = false;
volatile unsigned long pulseCountHI = 0L;
volatile unsigned long pulseCountLO = 0L;
volatile unsigned long lastBlink = 0L;
volatile unsigned long watt = 0L;
unsigned long oldPulseCountHI = 0L;
unsigned long oldPulseCountLO = 0L;
unsigned long oldWatt = 0L;

volatile bool HDOState;
bool LastHDOState;

MyMessage wattMsg(CHILD_ID_POWER_HI, V_WATT);    // Instant consumption is only one
//MyMessage pcMsgHI(CHILD_ID_POWER_PULSE, V_VAR1);
//MyMessage pcMsgLO(CHILD_ID_POWER_PULSE, V_VAR2);
MyMessage HDOmsg(CHILD_ID_POWER_HDO, V_TRIPPED);

void setup()
{
	// Fetch last known pulse count value from gw
	request(CHILD_ID_POWER_PULSE, V_VAR1);    // HI
	request(CHILD_ID_POWER_PULSE, V_VAR2);    // LO

	pinMode(POWER_SENSOR_PIN, INPUT_PULLUP);
	pinMode(HDO_SENSOR_PIN, INPUT_PULLUP);

	attachInterrupt(digitalPinToInterrupt(POWER_SENSOR_PIN), onPulse, RISING);
	attachInterrupt(digitalPinToInterrupt(HDO_SENSOR_PIN), readHDOState, CHANGE);
	readHDOState();
}

void presentation()
{
	sendSketchInfo("Energy Meter", "1.21");

	present(CHILD_ID_POWER_HI, S_POWER, "Power HI, instant");
	present(CHILD_ID_POWER_LO, S_POWER, "Power LO");
	present(CHILD_ID_POWER_HDO, S_DOOR, "HDO signal");
	present(CHILD_ID_POWER_PULSE, S_CUSTOM, "Pulse counters");
}

void loop()
{
	// When HDO status changes, send message immediately
	if (LastHDOState != HDOState) {
		send(HDOmsg.set(HDOState));
		LastHDOState = HDOState;
	}
	loop15s();  // Send instant power consumption often
	loop60s();  // Send HDO status and power meter data regularly
}

// Every 15s send instant consumption value
unsigned long timerPower = 0L;
void loop15s()
{
	if (millis() - timerPower > 15000) {
		timerPower = millis();
		// New watt value has been calculated  
		if (watt != oldWatt) {
			oldWatt = watt;
			// Check that we dont get unresonable large watt value. 
			// could hapen when long wraps or false interrupt triggered
			if (watt < ((unsigned long)MAX_WATT)) {
					send(wattMsg.set((uint32_t)oldWatt));
			}
#ifdef MY_DEBUG
			Serial.print("Watt:");
			Serial.println(watt);
#endif
		}
	}
}

unsigned int kwhCounter = 0;   // Counter for sendig kwh value once per 5 cycles (minutes)
void SendPulse()
{
	if (pcReceivedHI) {
		if (pulseCountHI != oldPulseCountHI) {
			oldPulseCountHI = pulseCountHI;
			MyMessage pcMsgHI(CHILD_ID_POWER_PULSE, V_VAR1);
				send(pcMsgHI.set((uint32_t)pulseCountHI));  // Send pulse count value to gw 
				if (kwhCounter % 5 == 0) {
					MyMessage kwhMsgHI(CHILD_ID_POWER_HI, V_KWH);
					double kwh = (double)pulseCountHI / (double)PULSE_FACTOR;
					send(kwhMsgHI.set(kwh, 4));  // Send kwh value to gw 
				}
		}
	}
	else {
		request(CHILD_ID_POWER_PULSE, V_VAR1);
	}
	if (pcReceivedLO) {
		if (pulseCountLO != oldPulseCountLO) {
			oldPulseCountLO = pulseCountLO;
			MyMessage pcMsgLO(CHILD_ID_POWER_PULSE, V_VAR2);
				send(pcMsgLO.set((uint32_t)pulseCountLO));  // Send pulse count value
				if (kwhCounter % 5 == 0) {
					MyMessage kwhMsgLO(CHILD_ID_POWER_LO, V_KWH);
					double kwh = (double)pulseCountLO / (double)PULSE_FACTOR;
					send(kwhMsgLO.set(kwh, 4));  // Send kwh value
				}
		}
	}
	else {
		request(CHILD_ID_POWER_PULSE, V_VAR2);
	}
	kwhCounter++;
}

// Every 1 minute send HDO status
unsigned long timer60s = 0L;
void loop60s()
{
	if (millis() - timer60s > 60000) {
		timer60s = millis();
		// Send power meter pulse count
		SendPulse();
		// Send HDO state
		readHDOState();
		send(HDOmsg.set(HDOState));
	}
}

void receive(const MyMessage &message) {
	if (message.sensor == CHILD_ID_POWER_PULSE) {
		if (message.type == V_VAR1) {
			pulseCountHI = oldPulseCountHI = message.getLong();
#ifdef MY_DEBUG
			Serial.print("Received last HI pulse count from gw:");
			Serial.println(pulseCountHI);
#endif
			pcReceivedHI = true;
		}
		if (message.type == V_VAR2) {
			pulseCountLO = oldPulseCountLO = message.getLong();
#ifdef MY_DEBUG
			Serial.print("Received last LO pulse count from gw:");
			Serial.println(pulseCountLO);
#endif
			pcReceivedLO = true;
		}
	}
}

// ISR to count LO/HI pulses
void onPulse()
{
	unsigned long newBlink = micros();
	unsigned long interval = newBlink - lastBlink;
	if (interval < 10000L) {
		// Sometimes we get interrupt on RISING
		return;
	}
	watt = (3600000000.0 / interval) / ppwh;
	lastBlink = newBlink;
	if (HDOState)
		pulseCountLO++;
	else
		pulseCountHI++;
}

// ISR to change HDO status immediately
void readHDOState()
{
	HDOState = (digitalRead(HDO_SENSOR_PIN) == HIGH);
}
