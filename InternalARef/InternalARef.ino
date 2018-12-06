// To measure internal AREF voltage for calculating low voltage
// As in http://www.gammon.com.au/power
/*
const long InternalReferenceVoltage = 1062;  // Adjust this value to your board's specific internal BG voltage

// Code courtesy of "Coding Badly" and "Retrolefty" from the Arduino forum
// results are Vcc * 100
// So for example, 5V would be 500.
int getBandgap ()
  {
  // REFS0 : Selects AVcc external reference
  // MUX3 MUX2 MUX1 : Selects 1.1V (VBG)
   ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
   ADCSRA |= bit( ADSC );  // start conversion
   while (ADCSRA & bit (ADSC))
	 { }  // wait for conversion to complete
   int results = (((InternalReferenceVoltage * 1024) / ADC) + 5) / 10;
   return results;
  } // end of getBandgap
  */

void setup()
{
	analogReference(INTERNAL);
	analogRead(A0);  // force voltage reference to be turned on
}
void loop() { }